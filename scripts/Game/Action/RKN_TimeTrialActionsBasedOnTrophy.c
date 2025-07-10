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
		array<ref SCR_ScenarioFrameworkActionBase> actions;
		
		int total = course.m_CurrentScoreInfo.GetTotal();
		
		if (total < course.m_Config.GetGoldMilliseconds())
			actions = m_aOnGoldActions;
		else if (total < course.m_Config.GetSilverMilliseconds())
			actions = m_aOnSilverActions;
		else if (total < course.m_Config.GetBronzeMilliseconds())
			actions = m_aOnBronzeActions;
		else
			actions = m_aOnNoneActions;
		
		if (actions)
			foreach (SCR_ScenarioFrameworkActionBase action : actions)
				action.Init(object);
	}
}