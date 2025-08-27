[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialTargetSlotClass : RKN_TimeTrialObjectiveSlotClass
{
}

class RKN_TimeTrialTargetSlot : RKN_TimeTrialObjectiveSlot
{
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnHitActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref RKN_TimeTrialTargetHitArea> m_aHitAreas;
	
	[Attribute(uiwidget: UIWidgets.Object, category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActivationConditionBase> m_aHitConditions;
	
	[Attribute("1", category: "Time trial")]
	int m_iRequiredHits;
	
	[Attribute("-1", category: "Time trial")]
	float m_iTimeoutSeconds;
	
	[Attribute("2", category: "Time trial")]
	float m_iTimeoutPenaltySeconds;
	
	[Attribute(uiwidget: UIWidgets.Object, category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aTimeoutActions;
	
	[Attribute(category: "Time trial Movement")]
	ref PointInfo m_MovePoint;
	
	[Attribute("1", category: "Time trial Movement")]
	float m_fMoveSpeedMetersPerSecond;
	
	[Attribute("false", category: "Time trial Movement")]
	bool m_bMoveCycle;
	
	[Attribute("false", desc: "Auto-generate track sections to move position", params: "et", category: "Time trial Movement")]
	bool m_bGenerateTracks;
	
	[Attribute(params: "et", category: "Time trial Movement")]
	ResourceName m_sTrackMidSectionPrefabName;
	
	[Attribute(params: "et", category: "Time trial Movement")]
	ResourceName m_sTrackEndSectionPrefabName;
	
	[Attribute("false", params: "et", category: "Time trial Movement")]
	float m_fTrackWidth;
	
	[Attribute("false", params: "et", category: "Time trial Movement")]
	float m_fTrackHeight;
	
	[Attribute("true", desc: "End section should have same position as neighboring mid-section", params: "et", category: "Time trial Movement")]
	bool m_bTrackEndSectionSamePosition;
	
	ref map<string, int> m_mHitAreasMap = new map<string, int>();
	RKN_TimeTrialTargetEntity m_Target;
	ref array<IEntity> m_aTrackEntities = {};
	
	override void FinishInit()
	{
		foreach (RKN_TimeTrialTargetHitArea area : m_aHitAreas)
			m_mHitAreasMap.Set(area.m_sKey, area.m_iScore * 1000);
		
		m_Target = RKN_TimeTrialTargetEntity.Cast(m_Entity);
		if (m_Target)
		{
			m_Target.Event_TargetHit.Insert(OnTargetHit);
			ResetObjective();
			m_Target.m_iRequiredHits = m_iRequiredHits;
		}
		if (m_MovePoint)
			m_MovePoint.Init(GetOwner());
		
		super.FinishInit();
	}
	
	override void ResetObjective()
	{
		super.ResetObjective();
		m_Target.ResetTarget();
		GetGame().GetCallqueue().Remove(Timeout);
		if (m_MovePoint)
		{
			m_Target.StartMovement(GetOwner().GetOrigin(), false, GetOwner().GetOrigin(), m_fMoveSpeedMetersPerSecond);
		}
	}
	
	override void ActivateObjective()
	{
		super.ActivateObjective();
		if (m_iTimeoutSeconds > 0)
		{
			GetGame().GetCallqueue().CallLater(Timeout, m_iTimeoutSeconds * 1000.0, false);
		}
		if (m_MovePoint)
		{
			m_Target.StartMovement(GetMovePosition(), m_bMoveCycle, GetOwner().GetOrigin(), m_fMoveSpeedMetersPerSecond);
		}
		m_Target.ActivateTarget(m_Section.m_Course.GetPlayerId());
	}
	
	void Timeout()
	{
		m_Target.SetState(ETargetState.TARGET_DOWN);
		if (!m_bBonusObjective)
			m_Section.m_Course.ApplyScoreModifier(m_iTimeoutPenaltySeconds * 1000.0);
		foreach (SCR_ScenarioFrameworkActionBase action : m_aTimeoutActions)
			action.Init(GetOwner());
		FinishObjective();
	}
	
	void OnTargetHit(string hitKey, int playerID)
	{
		GetGame().GetCallqueue().Remove(Timeout);
		if (!m_aHitConditions.IsEmpty())
		{
			RKN_TimeTrialCourseManagerComponent manager = m_Section.m_Course.GetCourseManager();
			IEntity player = manager.GetData(m_Section.m_Course.m_iCourseIndex).m_CurrentScoreInfo.GetPlayer();
			if (!SCR_ScenarioFrameworkActivationConditionBase.EvaluateEmptyOrConditions(SCR_EScenarioFrameworkLogicOperators.AND, m_aHitConditions, player))
			{
				m_Section.m_Course.FailCourse();
				return;
			}
		}
		int score = m_mHitAreasMap.Get(hitKey);
		if (score > 0)
			m_Section.m_Course.ApplyScoreModifier(-score);
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnHitActions)
			action.Init(GetOwner());
		FinishObjective();
	}
	
	override IEntity SpawnAsset()
	{
		IEntity entity = super.SpawnAsset();
		SpawnMoveTrack(entity);
		return entity;
	}
	
	void SpawnMoveTrack(IEntity parent)
	{
		if (m_bGenerateTracks && m_MovePoint && m_sTrackMidSectionPrefabName && m_fTrackWidth > 0)
		{
			vector dir = GetMovePosition();
			float distance = dir.Length();
			dir.Normalize();
			float sectionsFloat = (distance / m_fTrackWidth) + 1;
			int sections = sectionsFloat;
			if (Math.Mod(sectionsFloat, 1) > 0)
				sections++;
			for (int i = 0; i < sections; i++)
			{
				vector pos = dir * (m_fTrackWidth * i);
				SpawnTrackAt(parent, pos, dir, m_sTrackMidSectionPrefabName);
			}
			if (m_sTrackEndSectionPrefabName)
			{
				if (m_bTrackEndSectionSamePosition)
				{
					vector endPos = dir * (m_fTrackWidth * (sections - 1));
					SpawnTrackAt(parent, endPos, dir * -1, m_sTrackEndSectionPrefabName);
					SpawnTrackAt(parent, vector.Zero, dir, m_sTrackEndSectionPrefabName);
				}
				else
				{
					Print("m_bTrackEndSectionSamePosition = false not supported", LogLevel.ERROR);
				}
			}
		}
	}
	
	private void SpawnTrackAt(IEntity parent, vector position, vector direction, ResourceName prefab)
	{
		Resource p = Resource.Load(prefab);
		EntitySpawnParams params = new EntitySpawnParams();
		position[1] = position[1] + m_fTrackHeight;
		params.Transform[3] = position;
		Math3D.DirectionAndUpMatrix(direction, {0, 1, 0}, params.Transform);
		params.Parent = parent;
		IEntity entity = GetGame().SpawnEntityPrefab(p, null, params);
		m_aTrackEntities.Insert(entity);
	}
	
#ifdef WORKBENCH
	override void SpawnEntityPreview(IEntity owner, Resource resource)
	{
		super.SpawnEntityPreview(owner, resource);
		SpawnMoveTrack(m_PreviewEntity);
	}
	
	void ~RKN_TimeTrialTargetSlot()
	{
		foreach (IEntity e : m_aTrackEntities)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(e);
		}
	}
#endif
	
	vector GetMovePosition()
	{
		vector mat[4];
		m_MovePoint.GetWorldTransform(mat);
		return mat[3];
	}
}

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialTargetHitArea
{
	[Attribute()]
	string m_sKey;
	
	[Attribute()]
	float m_iScore;
}