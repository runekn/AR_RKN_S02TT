[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialTargetSlotClass : RKN_TimeTrialObjectiveSlotClass
{
}

class RKN_TimeTrialTargetSlot : RKN_TimeTrialObjectiveSlot
{
	[Attribute("-1")]
	int m_iTimeoutSeconds;
	
	[Attribute("2")]
	int m_iTimeoutPenaltySeconds;
	
	[Attribute()]
	ref PointInfo m_MovePoint;
	
	[Attribute("1")]
	float m_fMoveSpeedMetersPerSecond;
	
	[Attribute("false")]
	bool m_bMoveCycle;
	
	RKN_TimeTrialStandardTarget m_Target;
	vector m_vDesiredPosition;
	bool m_bMoveCycleActive;
	
	override void FinishInit()
	{
		m_Target = RKN_TimeTrialStandardTarget.Cast(m_Entity);
		if (m_Target)
		{
			m_Target.Event_TargetHit.Insert(OnTargetHit);
			ResetObjective();
		}
		if (m_MovePoint)
			m_MovePoint.Init(GetOwner());
		super.FinishInit();
	}
	
	override void ResetObjective()
	{
		super.ResetObjective();
		m_Target.SetState(ETargetState.TARGET_DOWN);
		GetGame().GetCallqueue().Remove(Timeout);
		if (m_MovePoint)
		{
			m_vDesiredPosition = GetOwner().GetOrigin();
			SetEventMask(GetOwner(), EntityEvent.FRAME);
			m_bMoveCycleActive = false;
		}
	}
	
	override void ActivateObjectiveImpl()
	{
		if (m_iTimeoutSeconds > 0)
		{
			GetGame().GetCallqueue().CallLater(Timeout, m_iTimeoutSeconds * 1000, false);
		}
		if (m_MovePoint)
		{
			SetEventMask(GetOwner(), EntityEvent.FRAME);
			m_vDesiredPosition = GetMovePosition();
			m_bMoveCycleActive = m_bMoveCycle;
		}
		m_Target.SetState(ETargetState.TARGET_UP);
	}
	
	void Timeout()
	{
		m_Target.SetState(ETargetState.TARGET_DOWN);
		if (!m_bBonusObjective)
			m_Section.m_Course.ApplyScoreModifier(m_iTimeoutPenaltySeconds * 1000);
		FinishObjective();
	}
	
	void OnTargetHit(float score)
	{
		if (score > 0)
			m_Section.m_Course.ApplyScoreModifier(-score);
		FinishObjective();
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_vDesiredPosition != vector.Zero)
		{
			vector target = m_Target.GetOrigin();
			vector dir = vector.Direction(target, m_vDesiredPosition);
			float dist = dir.Length();
			dir = dir.Normalized();
			float component = m_fMoveSpeedMetersPerSecond * timeSlice;
			vector moveVector = dir * component;
			vector newPos;
			if (moveVector.Length() >= dist)
			{
				newPos = m_vDesiredPosition;
				if (m_bMoveCycleActive)
				{
					vector origin = GetOwner().GetOrigin();
					if (m_vDesiredPosition == origin)
						m_vDesiredPosition = GetMovePosition();
					else
						m_vDesiredPosition = origin;
				}
				else
				{
					m_vDesiredPosition = vector.Zero;
					ClearEventMask(GetOwner(), EntityEvent.FRAME);
				}
			}
			else
			{
				newPos = target + moveVector;
				//newPos = { target[0] + moveVector[0], target[1] + moveVector[1], target[2] + moveVector[2] };
			}
			m_Target.SetOrigin(newPos);
			m_Target.Update();
		}
	}
	
	vector GetMovePosition()
	{
		vector mat[4];
		m_MovePoint.GetWorldTransform(mat);
		return mat[3];
	}
}