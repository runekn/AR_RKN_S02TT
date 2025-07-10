[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialCountdownPositionSlotClass : SCR_ScenarioFrameworkSlotTriggerClass
{
}

class RKN_TimeTrialCountdownPositionSlot : SCR_ScenarioFrameworkSlotTrigger
{
	[Attribute(category: "Time trial")]
	ref RKN_Get m_CourseGetter;
	
	RKN_TimeTrialCourseLayer m_Course;
	
	override void FinishInit()
	{	
		super.FinishInit();
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(m_Entity);
		if (trigger)
		{
			trigger.GetOnDeactivate().Insert(OnPlayerExited);
			trigger.SetActivateOnEmpty(true);
		}
		
		m_Course = RKN_TimeTrialUtils.FindCourse(m_CourseGetter, GetOwner());
		if (!m_Course)
			return;
		m_Course.m_OnSchedule.Insert(ActivateTrigger);
		m_Course.m_OnCancel.Insert(DeactivateTrigger);
		m_Course.m_OnActive.Insert(DeactivateTrigger);
	}
	
	override void AfterParentAreaChildrenSpawned(SCR_ScenarioFrameworkLayerBase layer)
	{
		super.AfterParentAreaChildrenSpawned(layer);
		TriggerState(false); // Disable it again after super.AfterParentAreaChildrenSpawned enabled it
	}
	
	void ActivateTrigger()
	{
		TriggerState(true);
	}
	
	void DeactivateTrigger()
	{
		TriggerState(false);
	}
	
	void OnPlayerExited()
	{
		TriggerState(false);
		m_Course.FailCourse();
	}
	
	void TriggerState(bool state)
	{
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(m_Entity);
		if (trigger)
		{
			trigger.EnablePeriodicQueries(state);
			trigger.SetInitSequenceDone(state);
		}
	}
}