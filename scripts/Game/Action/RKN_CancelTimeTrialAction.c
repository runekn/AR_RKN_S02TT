class RKN_CancelTimeTrialAction : SCR_ScriptedUserAction
{	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		RKN_TimeTrialControlPanelEntity.Cast(pOwnerEntity).CancelCourse(pUserEntity);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		int courseId = RKN_TimeTrialControlPanelEntity.Cast(GetOwner()).m_iCourseId;
		if (courseId < 0)
			return false;
		return RKN_TimeTrialUtils.GetCourseDataRepo().HasActiveCompetitor(courseId);
	}
}