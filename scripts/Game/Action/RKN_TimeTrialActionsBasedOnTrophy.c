[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialActionsBasedOnTrophy : SCR_ScenarioFrameworkActionBase
{
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnGoldActions;
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnSilverActions;
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnBronzeActions;
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnNoneActions;
	
	override void OnActivate(IEntity object)
	{
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(object.FindComponent(RKN_TimeTrialCourseLayer));
		RKN_TimeTrialCourseData data = RKN_TimeTrialUtils.GetCourseDataRepo().GetData(course.m_iCourseIndex);
		array<ref SCR_ScenarioFrameworkActionBase> actions;
		
		int total = data.m_CurrentScoreInfo.GetTotal();
		
		if (total < data.m_Config.GetGoldMilliseconds())
			actions = m_aOnGoldActions;
		else if (total < data.m_Config.GetSilverMilliseconds())
			actions = m_aOnSilverActions;
		else if (total < data.m_Config.GetBronzeMilliseconds())
			actions = m_aOnBronzeActions;
		else
			actions = m_aOnNoneActions;
		
		if (actions)
			foreach (SCR_ScenarioFrameworkActionBase action : actions)
				action.Init(object);
	}
}