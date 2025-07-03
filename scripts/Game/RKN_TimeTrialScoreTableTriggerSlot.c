[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RKN_TimeTrialScoreTableTriggerSlotClass : SCR_ScenarioFrameworkSlotTriggerClass
{
}

class RKN_TimeTrialScoreTableTriggerSlot : SCR_ScenarioFrameworkSlotTrigger
{
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	
	SCR_ScenarioFrameworkTriggerEntity m_Trigger;
	RKN_TimeTrialCourseLayer m_CourseLayer;
	
	private int m_iPlayersInArea;
	
	override void FinishInit()
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(m_Entity);
		if (trigger)
		{
			m_Trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(trigger);
			if (m_Trigger)
			{
				m_Trigger.GetOnChange().Insert(OnChange);
			}
		}
		
		if (m_CourseGetter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = m_CourseGetter.Get(GetOwner());
			if (!paramBase)
				Print("Course not found!", LogLevel.ERROR);
			else
			{
				IEntity entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
				m_CourseLayer = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
			}
		}
		
		super.FinishInit();
	}
	
	void OnChange(SCR_ScenarioFrameworkParam<IEntity> param)
	{
		// On normal triggering with players in area, the param is the trigger. Pretty sure it is a bug.
		// On exiting the trigger, the param is the player. It should always be the player according to doc comments.
		if (param.GetValue() == m_Trigger)
		{
			array<IEntity> players = {};
			m_Trigger.GetPlayersByFactionInsideTrigger(players);
			foreach (IEntity player : players)
			{
				RKN_TimeTrialScoreTablePlayerComponent playerComponent = m_CourseLayer.FindPlayerUIComponent(player);
				if (playerComponent)
					playerComponent.ShowScoreTable(m_CourseLayer, false);
			}
			return;
		}
		else
		{
			RKN_TimeTrialScoreTablePlayerComponent playerComponent = m_CourseLayer.FindPlayerUIComponent(param.GetValue());
			if (playerComponent)
				playerComponent.RemoveScoreTable(m_CourseLayer, false);
		}
		
		/*int playersInArea = m_Trigger.GetCountInsideTrigger();
		if (m_iPlayersInArea > playersInArea)
		{
			RKN_TimeTrialScoreTablePlayerComponent playerComponent = m_CourseLayer.FindPlayerUIComponent(param.GetValue());
			if (playerComponent)
				playerComponent.RemoveScoreTable(m_CourseLayer);
		}
		else if (m_iPlayersInArea < playersInArea)
		{
			RKN_TimeTrialScoreTablePlayerComponent playerComponent = m_CourseLayer.FindPlayerUIComponent(param.GetValue());
			if (playerComponent)
				playerComponent.ShowScoreTable(m_CourseLayer);
		}
		m_iPlayersInArea = playersInArea;*/
	}
}