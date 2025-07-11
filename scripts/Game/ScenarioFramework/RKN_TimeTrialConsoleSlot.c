class RKN_TimeTrialConsoleSlotClass : SCR_ScenarioFrameworkSlotBaseClass
{
}

class RKN_TimeTrialConsoleSlot : SCR_ScenarioFrameworkSlotBase
{	
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	
	RKN_TimeTrialCourseLayer m_Course;
	
	override bool InitOtherThings()
	{
		if (!super.InitOtherThings())
			return false;
		
		IEntity entity = GetSpawnedEntity();
		if (!entity)
			return false;
		
		RKN_TimeTrialControlPanelEntity controlPanel = RKN_TimeTrialControlPanelEntity.Cast(entity);
		if (!controlPanel)
			return false;
		
		m_Course = RKN_TimeTrialUtils.FindCourse(m_CourseGetter, GetOwner());
		if (m_Course)
			controlPanel.SetCourseId(m_Course.m_iCourseIndex);
		
		controlPanel.m_OnStart.Insert(ScheduleCourse);
		controlPanel.m_OnCancel.Insert(CancelCourse);
		
		return true;
	}
	
	void ScheduleCourse(int playerId, float delay, bool competitive)
	{
		m_Course.ScheduleCourse(playerId, delay, competitive);
	}
	
	void CancelCourse()
	{
		m_Course.CancelCourse();
	}
}