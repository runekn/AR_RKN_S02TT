// Something that can find the player's radio

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialVoiceLine : SCR_ScenarioFrameworkActionVoiceOverPlayLine
{
	[Attribute("{62AA1DC9919E6E61}Prefabs/TimeTrial_Radio_ANPRC68.et", params: "et")]
	ResourceName m_sRadioPrefab;
	
	[Attribute("false")]
	bool m_bResetNumberOfActivations;
	
	RKN_TimeTrialCourseLayer m_Course;
	
	override bool ValidateInputEntity(IEntity object, SCR_ScenarioFrameworkGet getter, out IEntity entity)
	{
		SCR_ChimeraCharacter player;
		if (getter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = getter.Get();
			if (paramBase)
				object = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
		}
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(object);
		if (trigger)
		{
			// Just get whomever is in the trigger
			array<IEntity> players = {};
			trigger.GetPlayersByFactionInsideTrigger(players);
			if (!players.IsEmpty())
				player = SCR_ChimeraCharacter.Cast(players[0]);
		}
		if (SCR_ChimeraCharacter.Cast(object))
		{
			player = SCR_ChimeraCharacter.Cast(object);
		}
		else
		{
			if (!m_Course)
			{
				m_Course = RKN_TimeTrialCourseLayer.Cast(object.FindComponent(RKN_TimeTrialCourseLayer));
				RKN_TimeTrialObjectiveSlot obj = RKN_TimeTrialObjectiveSlot.Cast(object.FindComponent(RKN_TimeTrialObjectiveSlot));
				RKN_TimeTrialSectionLayer section = RKN_TimeTrialSectionLayer.Cast(object.FindComponent(RKN_TimeTrialSectionLayer));
				if (obj)
					m_Course = obj.m_Section.m_Course;
				else if (section)
					m_Course = section.m_Course;
				
				if (!m_Course)
				{
					SCR_ScenarioFrameworkParam<IEntity> param = RKN_Get.FindComponentInParents(object, RKN_TimeTrialCourseLayer);
					if (param)
						m_Course = RKN_TimeTrialCourseLayer.Cast(param.GetValue().FindComponent(RKN_TimeTrialCourseLayer));
				}
				
				if (!m_Course)
				{
					Print("No course found", LogLevel.ERROR);
					return false;
				}
				
				if (m_bResetNumberOfActivations)
				{
					m_Course.m_OnReset.Insert(ResetNumberOfActivations);
					m_Course.m_OnCancel.Insert(ResetNumberOfActivations);
				}
			}
				
			RKN_TimeTrialCourseData data = RKN_TimeTrialUtils.GetCourseManager().GetData(m_Course.m_iCourseIndex);
			
			if (data.m_CurrentScoreInfo.IsEmpty())
			{
				Print("No active player on course", LogLevel.ERROR);
				return false;
			}
			
			player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(data.m_CurrentScoreInfo.m_iID));
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
	
	void ResetNumberOfActivations()
	{
		m_iNumberOfActivations = 0;
	}
}