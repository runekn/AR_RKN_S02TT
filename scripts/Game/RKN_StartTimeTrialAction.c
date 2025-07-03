class RKN_StartTimeTrialAction : ScriptedUserAction
{
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	
	private int m_iPushButtonStart;
	
	const static int RESET_BUTTON_TIME = 500;
	const static float BUTTON_PRESSED_STATE = -0.005;
	const static int BUTTON_DEFAULT_STATE = 0;
	
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
		course.StartCourse(pUserEntity);
	}
}