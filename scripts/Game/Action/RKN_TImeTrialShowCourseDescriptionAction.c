class RKN_TimeTrialShowCourseDescriptionAction : ScriptedUserAction
{	
	[Attribute("30")]
	int m_iHintTimeoutSeconds;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		RKN_TimeTrialCourseConfig config = GetConfig();
		if (!config)
			return;
		
		if (!config.m_sDescription)
			return;
		
		SCR_ScenarioFrameworkSystem scenarioFrameworkSystem = SCR_ScenarioFrameworkSystem.GetInstance();
		if (!scenarioFrameworkSystem)
			return;
		
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		scenarioFrameworkSystem.ShowHint(config.m_sDescription, config.m_sName, m_iHintTimeoutSeconds, "", playerID);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		RKN_TimeTrialCourseConfig config = GetConfig();
		if (!config)
			return false;
		return config.m_sDescription;
	}
	
	RKN_TimeTrialCourseConfig GetConfig()
	{
		int courseId = RKN_TimeTrialControlPanelEntity.Cast(GetOwner()).m_iCourseId;
		if (courseId < 0)
			return null;
		RKN_TimeTrialScoreRepository repo = RKN_TimeTrialUtils.GetCourseDataRepo();
		RKN_TimeTrialCourseData data = repo.GetData(courseId);
		if (!data)
			return null;
		return data.m_Config;
	}
}