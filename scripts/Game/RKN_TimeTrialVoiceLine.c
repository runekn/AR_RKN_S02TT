// Something that can find the player's radio

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialVoiceLine : SCR_ScenarioFrameworkActionVoiceOverPlayLine
{
	[Attribute("{62AA1DC9919E6E61}Prefabs/TimeTrial_Radio_ANPRC68.et", params: "et")]
	private ResourceName m_sRadioPrefab;
	
	override bool ValidateInputEntity(IEntity object, SCR_ScenarioFrameworkGet getter, out IEntity entity)
	{
		SCR_ChimeraCharacter player;
		if (SCR_ChimeraCharacter.Cast(object))
		{
			player = SCR_ChimeraCharacter.Cast(object);
		}
		else
		{
			RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(object.FindComponent(RKN_TimeTrialCourseLayer));
			RKN_TimeTrialObjectiveSlot obj = RKN_TimeTrialObjectiveSlot.Cast(object.FindComponent(RKN_TimeTrialObjectiveSlot));
			RKN_TimeTrialSectionLayer section = RKN_TimeTrialSectionLayer.Cast(object.FindComponent(RKN_TimeTrialSectionLayer));
			if (obj)
				course = obj.m_Section.m_Course;
			else if (section)
				course = section.m_Course;
			
			if (!course)
			{
				Print("No course found", LogLevel.ERROR);
				return false;
			}
			
			if (!course.m_CurrentScoreInfo)
			{
				Print("No active player on course", LogLevel.ERROR);
				return false;
			}
			
			player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(course.m_CurrentScoreInfo.m_iID));
		}
		
		if (!player)
		{
			Print("No player found", LogLevel.ERROR);
			return false;
		}
		
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		entity = inv.FindItem(new SCR_ResourceNamePredicate(m_sRadioPrefab));
		if (!entity)
		{
			Print("No radio found on player", LogLevel.ERROR);
			return false;
		}
		
		return true;
	}
}