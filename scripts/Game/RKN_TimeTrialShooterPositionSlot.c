[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialShooterPositionSlotClass : RKN_TimeTrialObjectiveSlotClass
{
}

class RKN_TimeTrialShooterPositionSlot : RKN_TimeTrialObjectiveSlot
{	
	SCR_ScenarioFrameworkTriggerEntity m_Trigger;
	
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
		
		foreach (SCR_ScenarioFrameworkActionBase activationAction : m_aActivationActions)
		{
			activationAction.Init(GetOwner());
		}

		if (m_fRepeatedSpawnTimer >= 0)
			RepeatedSpawn();
		
		if (m_Area)
			m_Area.GetOnAllChildrenSpawned().Remove(AfterParentAreaChildrenSpawned);
	}
	
	override void ActivateObjectiveImpl()
	{
		m_Trigger.EnablePeriodicQueries(true);
		m_Trigger.SetInitSequenceDone(true);
	}
	
	void ShooterEntered()
	{
		m_Trigger.EnablePeriodicQueries(false);
		m_Trigger.SetInitSequenceDone(false);
		FinishObjective();
	}
}