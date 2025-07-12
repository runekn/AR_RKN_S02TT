class RKN_StartTimeTrialAction : SCR_ScriptedUserAction
{
	[Attribute("false")]
	bool m_bCompetitive;
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		RKN_TimeTrialControlPanelEntity.Cast(pOwnerEntity).StartCountdown(pUserEntity, m_bCompetitive);
	}
	
	override bool CanBeShownScript(IEntity user)
	{
		int courseId = RKN_TimeTrialControlPanelEntity.Cast(GetOwner()).m_iCourseId;
		if (courseId < 0)
			return false;
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		RKN_TimeTrialScoreRepository repo = RKN_TimeTrialUtils.GetCourseDataRepo();
		return !repo.HasActiveCompetitor(courseId) && !repo.IsActiveCompetitor(playerId);
	}
}