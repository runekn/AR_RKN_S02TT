class RKN_TimeTrialUtils
{
	static RKN_TimeTrialCourseLayer FindCourse(RKN_Get courseGetter, IEntity owner)
	{
		IEntity entity;
		if (courseGetter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = courseGetter.Get(owner);
			entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
		}
		
		if (!entity)
		{
			Print("Course not found!", LogLevel.ERROR);
			return null;
		}
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
		if (!course)
		{
			Print("Course entity does not have course component!", LogLevel.ERROR);
			return null;
		}
		return course;
	}
	
	static RKN_TimeTrialCourseManagerComponent GetCourseManager()
	{
		return RKN_TimeTrialCourseManagerComponent.Cast(GetGame().GetGameMode().FindComponent(RKN_TimeTrialCourseManagerComponent));
	}
}