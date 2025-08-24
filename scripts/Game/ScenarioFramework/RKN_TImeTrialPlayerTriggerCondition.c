[BaseContainerProps()]
class RKN_TimeTrialPlayerTriggerCondition : SCR_CustomTriggerConditions
{
	int m_iCourseId;
	
	void RKN_TimeTrialPlayerTriggerCondition(int courseId)
	{
		m_iCourseId = courseId;
	}
	
	override bool Init(IEntity entity)
	{
		/* QueryEntityInside does not seem to work 
		int expectedId = RKN_TimeTrialUtils.GetCourseManager().GetData(m_iCourseId).m_CurrentScoreInfo.m_iID;
		if (expectedId == 0)
			return false;
		IEntity expected = GetGame().GetPlayerManager().GetPlayerControlledEntity(expectedId);
		if (!expected)
			return false;
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(entity);
		return trigger.QueryEntityInside(expected);
		*/
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(entity);
		array<IEntity> players = {};
		trigger.GetPlayersByFactionInsideTrigger(players);
		int expectedId = RKN_TimeTrialUtils.GetCourseManager().GetData(m_iCourseId).m_CurrentScoreInfo.m_iID;
		if (expectedId == 0)
			return false;
		IEntity expected = GetGame().GetPlayerManager().GetPlayerControlledEntity(expectedId);
		if (!expected)
			return false;
		foreach (IEntity player : players)
		{
			if (player == expected)
				return true;
		}
		return false;
	}
}