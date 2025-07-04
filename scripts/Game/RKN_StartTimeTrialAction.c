class RKN_StartTimeTrialAction : ScriptedUserAction
{
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	
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
		RKN_TimeTrialControlPanelEntity.Cast(pOwnerEntity).StartCountdown(pUserEntity, course);
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
		return course.m_iPlayer <= 0;
	}
	
	
}