// Something that can find the player's radio

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialVoiceLine : SCR_ScenarioFrameworkActionBase
{	
	[Attribute("false")]
	bool m_bResetNumberOfActivations;
	
	[Attribute(desc: "Name of the sound as defined .acp file")]
	ref array<string> m_sSoundEvents;
	
	[Attribute(desc: "Name of the sound as defined .acp file")]
	string m_sLineName;
	
	[Attribute(desc: "Character playing the voiceover. Must have RKN_TimeTrialRadioComponent. If empty, action will look for parent course and choose active competitor.")]
	ref SCR_ScenarioFrameworkGet m_ActorGetter;
	
	[Attribute()]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aActions
	
	RKN_TimeTrialCourseLayer m_Course;
	RKN_TimeTrialRadioComponent m_Radio;

	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object)
	{
		if (!CanActivate())
			return;
		
		if (!ValidateInputEntity(object, m_ActorGetter, m_Entity))
			return;
		
		m_Radio = RKN_TimeTrialRadioComponent.Cast(m_Entity.FindComponent(RKN_TimeTrialRadioComponent));
		if (!m_Radio)
			return;
		
		m_Radio.QueueSoundEvents(m_sSoundEvents);
	}
	
	override bool ValidateInputEntity(IEntity object, SCR_ScenarioFrameworkGet getter, out IEntity entity)
	{	
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
			{
				entity = SCR_ChimeraCharacter.Cast(players[0]);
				return true;
			}
		}
		if (SCR_ChimeraCharacter.Cast(object))
		{
			entity = SCR_ChimeraCharacter.Cast(object);
			return true;
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
			
			entity = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(data.m_CurrentScoreInfo.m_iID));
		}
		
		return true;
	}
	
	void ResetNumberOfActivations()
	{
		m_iNumberOfActivations = 0;
	}
}