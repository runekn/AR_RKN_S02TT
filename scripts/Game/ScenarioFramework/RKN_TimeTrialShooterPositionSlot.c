[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialShooterPositionSlotClass : RKN_TimeTrialObjectiveSlotClass
{
}

class RKN_TimeTrialShooterPositionSlot : RKN_TimeTrialObjectiveSlot
{	
	[Attribute(category: "Time trial trigger")]
	bool m_bFailIfExitBeforeSectionCompletion;
	
	[Attribute(category: "Time trial trigger")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnFailActions;
	
	SCR_ScenarioFrameworkTriggerEntity m_Trigger;
	bool m_bShooterWithin;
	
	//------------------------------------------------------------------------------------------------
	//! Initializes trigger entities, disables periodic queries, and sets init sequence done to false.
	override void FinishInit()
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(m_Entity);
		if (trigger)
		{
			trigger.EnablePeriodicQueries(false);
			
			m_Trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(trigger);
			if (m_Trigger)
			{
				m_Trigger.GetOnActivate().Insert(ShooterEntered);
				m_Trigger.SetInitSequenceDone(false);
				if (m_bFailIfExitBeforeSectionCompletion)
					m_Trigger.GetOnDeactivate().Insert(FailCourse);
			}
		}
		
		super.FinishInit();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Initializes scenario layer, checks parent layer, sets area, and removes self from onAllChildrenSpawned list.
	//! \param[in] layer for which this is called.
	override void AfterAllChildrenSpawned(SCR_ScenarioFrameworkLayerBase layer)
	{
		m_bInitiated = true;
		
		if (m_ParentLayer)
			m_ParentLayer.CheckAllChildrenSpawned(this);
		
		if (!m_Area)
			m_Area = GetParentArea();
		
		if (m_Area)
		{
			m_Area.GetOnAllChildrenSpawned().Insert(AfterParentAreaChildrenSpawned);
			m_Area.CheckAllChildrenSpawned(this);
		}

		GetOnAllChildrenSpawned().Remove(AfterAllChildrenSpawned);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Initializes plugins, actions, and triggers, checks for repeated spawn timer, removes event handler, enables periodic queries,
	//! \param[in] layer Initializes plugins, actions, and triggers for scenario framework layer after parent area children spawned.
	protected void AfterParentAreaChildrenSpawned(SCR_ScenarioFrameworkLayerBase layer)
	{
		foreach (SCR_ScenarioFrameworkPlugin plugin : m_aPlugins)
		{
			plugin.Init(this);
		}
		
		m_Trigger.AddCustomTriggerCondition(new RKN_TimeTrialPlayerTriggerCondition(m_Section.m_Course.m_iCourseIndex));
		
		if (m_bFailIfExitBeforeSectionCompletion)
			m_Trigger.SetOnce(false);
		
		foreach (SCR_ScenarioFrameworkActionBase activationAction : m_aActivationActions)
		{
			activationAction.Init(GetOwner());
		}

		if (m_fRepeatedSpawnTimer >= 0)
			RepeatedSpawn();
		
		if (m_Area)
			m_Area.GetOnAllChildrenSpawned().Remove(AfterParentAreaChildrenSpawned);
	}
	
	override void RegisterObjective()
	{
		super.RegisterObjective();
		if (m_bFailIfExitBeforeSectionCompletion)
			m_Section.m_OnFinish.Insert(DisableTrigger);
	}
	
	override void ResetObjective()
	{
		super.ResetObjective();
		DisableTrigger();
		m_bShooterWithin = false;
	}
	
	override void ActivateObjective()
	{
		super.ActivateObjective();
		m_Trigger.EnablePeriodicQueries(true);
		m_Trigger.SetInitSequenceDone(true);
	}
	
	void ShooterEntered()
	{
		if (m_bShooterWithin)
			return;
		if (!m_bFailIfExitBeforeSectionCompletion)
			DisableTrigger();
		else
			m_bShooterWithin = true;
		FinishObjective();
	}
	
	void FailCourse()
	{
		if (!m_bShooterWithin)
			return;
		DisableTrigger();
		m_Section.m_Course.FailCourse(false);
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnFailActions)
			action.Init(GetOwner());
	}
	
	void DisableTrigger()
	{
		m_Trigger.EnablePeriodicQueries(false);
		m_Trigger.SetInitSequenceDone(false);
	}
}