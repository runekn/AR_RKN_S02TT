class RKN_TimeTrialObjectiveSlotClass : SCR_ScenarioFrameworkSlotBaseClass
{
}

class RKN_TimeTrialObjectiveSlot : SCR_ScenarioFrameworkSlotBase
{
	[Attribute(category: "Time trial")]
	ref RKN_Get m_SectionGetter;
	
	[Attribute(category: "Time trial")]
	ref RKN_Get m_DependentObjectiveGetter;
	
	[Attribute(category: "Time trial")]
	float m_fActivationDelaySeconds;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnActivationActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnResetActions;
	
	[Attribute("false", category: "Time trial")]
	bool m_bBonusObjective;
	
	[Attribute("0", category: "Time trial")]
	float m_fBonusAwardSeconds;
	
	RKN_TimeTrialSectionLayer m_Section;
	ref array<RKN_TimeTrialObjectiveSlot> m_DependentObjectives = {};
	int m_iDependentFinished;
	bool m_bFinished;
	
	ref ScriptInvoker m_OnFinish = new ScriptInvoker();
	
	override bool InitOtherThings()
	{
		RegisterObjective();
		if (m_DependentObjectiveGetter)
		{
			SCR_ScenarioFrameworkParamBase param = m_DependentObjectiveGetter.Get(GetOwner());
			SCR_ScenarioFrameworkParam<IEntity> single = SCR_ScenarioFrameworkParam<IEntity>.Cast(param);
			SCR_ScenarioFrameworkParam<array<IEntity>> arr = SCR_ScenarioFrameworkParam<array<IEntity>>.Cast(param);
			if (single)
			{
				IEntity entity = single.GetValue();
				RKN_TimeTrialObjectiveSlot obj = RKN_TimeTrialObjectiveSlot.Cast(entity.FindComponent(RKN_TimeTrialObjectiveSlot));
				obj.m_OnFinish.Insert(DependentFinished);
				m_DependentObjectives.Insert(obj);
			}
			else if (arr)
			{
				foreach (IEntity entity : arr.GetValue())
				{
					RKN_TimeTrialObjectiveSlot obj = RKN_TimeTrialObjectiveSlot.Cast(entity.FindComponent(RKN_TimeTrialObjectiveSlot));	
					obj.m_OnFinish.Insert(DependentFinished);
					m_DependentObjectives.Insert(obj);
				}
			}
		}
		return super.InitOtherThings();
	}
	
	void RegisterObjective()
	{
		IEntity entity;
		if (m_SectionGetter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = m_SectionGetter.Get(GetOwner());
			entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
		}
		
		if (!entity)
		{
			Print("Section not found!", LogLevel.ERROR);
			return;
		}
		m_Section = RKN_TimeTrialSectionLayer.Cast(entity.FindComponent(RKN_TimeTrialSectionLayer));
		if (!m_Section)
		{
			Print("Section entity does not have section component!", LogLevel.ERROR);
			return;
		}
		m_Section.RegisterObjective(this, m_bBonusObjective);
	}
	
	void TryActivateObjective()
	{
		if (!m_DependentObjectives.IsEmpty())
			return;
		if (m_fActivationDelaySeconds > 0)
			GetGame().GetCallqueue().CallLater(ActivateObjective, m_fActivationDelaySeconds * 1000, false);
		else
			ActivateObjective();
	}
	
	void ActivateObjective()
	{
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnActivationActions)
			action.Init(GetOwner());
	}
	
	void DependentFinished()
	{
		if (++m_iDependentFinished >= m_DependentObjectives.Count())
		{
			if (m_fActivationDelaySeconds > 0)
				GetGame().GetCallqueue().CallLater(ActivateObjective, m_fActivationDelaySeconds * 1000, false);
			else
				ActivateObjective();
		}
	}
	
	void FinishObjective()
	{
		Print("Finished objective " + GetName());
		if (m_bFinished)
			return;
		m_bFinished = true;
		if (!m_bBonusObjective)
			m_Section.FinishObjective(this);
		else
			if (m_fBonusAwardSeconds != 0)
				m_Section.m_Course.ApplyScoreModifier(-m_fBonusAwardSeconds * 1000);
		m_OnFinish.Invoke();
	}
	
	void ResetObjective()
	{
		GetGame().GetCallqueue().Remove(ActivateObjective);
		m_bFinished = false;
		m_iDependentFinished = 0;
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnResetActions)
			action.Init(GetOwner());
	}
}