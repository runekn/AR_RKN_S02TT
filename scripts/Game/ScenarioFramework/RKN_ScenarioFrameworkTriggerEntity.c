modded class SCR_ScenarioFrameworkTriggerEntity
{
	void AddCustomTriggerCondition(SCR_ScenarioFrameworkActivationConditionBase condition)
	{
		m_aCustomTriggerConditions.Insert(condition);
		SCR_CustomTriggerConditions triggerCondition = SCR_CustomTriggerConditions.Cast(condition);
		if (triggerCondition)
			triggerCondition.Prepare(this);
	}
}