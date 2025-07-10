class RKN_StartTimeTrialAction : SCR_ScriptedUserAction
{
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	[Attribute("false")]
	bool m_bCompetitive;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		IEntity entity;
		if (m_CourseGetter)
		{
			SCR_ScenarioFrameworkParam<IEntity> param = SCR_ScenarioFrameworkParam<IEntity>.Cast(m_CourseGetter.Get(pOwnerEntity));
			entity = param.GetValue();
		}
		if (!entity)
		{
			Print("Course not found", LogLevel.ERROR);
			return;
		}
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
		RKN_TimeTrialControlPanelEntity.Cast(pOwnerEntity).StartCountdown(pUserEntity, course, m_bCompetitive);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		IEntity entity;
		if (m_CourseGetter)
		{
			SCR_ScenarioFrameworkParam<IEntity> param = SCR_ScenarioFrameworkParam<IEntity>.Cast(m_CourseGetter.Get(GetOwner()));
			entity = param.GetValue();
		}
		if (!entity)
			return false;
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
		if (!course.m_CurrentScoreInfo)
		{
			SetCannotPerformReason("This will erase your current loadout!"); // Doesn't work
			return true;
		}
		return false;
	}
	
	
}