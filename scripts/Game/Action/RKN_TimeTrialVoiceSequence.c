// Something that can find the player's radio

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialVoiceSequence : SCR_ScenarioFrameworkActionVoiceOverPlaySequence
{
	[Attribute("{62AA1DC9919E6E61}Prefabs/TimeTrial_Radio_ANPRC68.et", params: "et")]
	private ResourceName m_sRadioPrefab;
	
	override void OnActivate(IEntity object)
	{
		if (!m_sVoiceOverDataConfig || m_sVoiceOverDataConfig.IsEmpty())
		{
			SCR_ScenarioFrameworkSystem scenarioFrameworkSystem = SCR_ScenarioFrameworkSystem.GetInstance();
			if (!scenarioFrameworkSystem)
				return;
			
			m_sVoiceOverDataConfig = scenarioFrameworkSystem.m_sVoiceOverDataConfig;
			
			if (!m_sVoiceOverDataConfig || m_sVoiceOverDataConfig.IsEmpty())
				return;
		}
		
		if (!CanActivate())
			return;

		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;

		array<IEntity> entities = {};
		if (m_PlayerGetter)
		{
			SCR_ScenarioFrameworkParam<array<IEntity>> entityArrayWrapper = SCR_ScenarioFrameworkParam<array<IEntity>>.Cast(m_PlayerGetter.Get());
			if (entityArrayWrapper)
			{
				entities = entityArrayWrapper.GetValue();
				if (!entities || entities.IsEmpty())
					return;
			}
			else
			{
				SCR_ScenarioFrameworkParam<IEntity> entityWrapper = SCR_ScenarioFrameworkParam<IEntity>.Cast(m_PlayerGetter.Get());
				if (!entityWrapper)
				{
					if (object)
						Print(string.Format("ScenarioFramework Action: Issue with Getter detected for Action %1 attached on %2.", this, object.GetName()), LogLevel.ERROR);
					else
						Print(string.Format("ScenarioFramework Action: Issue with Getter detected for Action %1.", this), LogLevel.ERROR);
				
					return;
				}
	
				IEntity playerEntity = entityWrapper.GetValue();
				
				entities.Insert(playerEntity);
			}
		}	
		
		IEntity entity;
		if (!ValidateInputEntity(object, null, entity))
			return;
		array<IEntity> actorEntities = { entity };
		
		array<int> playerIDs = {};
		foreach(IEntity possiblePlayerEntity : entities)
		{
			if (EntityUtils.IsPlayer(possiblePlayerEntity))
				playerIDs.Insert(playerManager.GetPlayerIdFromControlledEntity(possiblePlayerEntity))
		}
		
		SCR_VoiceoverSystem voiceoverSystem = SCR_VoiceoverSystem.GetInstance();
		if (!voiceoverSystem)
			return;
		
		voiceoverSystem.SetData(m_sVoiceOverDataConfig);
		voiceoverSystem.GetOnFinished().Insert(OnFinished);
		
		if (RplSession.Mode() == RplMode.None)
		{
			voiceoverSystem.PlaySequenceActorsArray(m_sSequenceName, actorEntities, m_bPlayImmediately);
		}
		else
		{
			foreach (IEntity actor : actorEntities)
			{
				if (!Replication.FindId(actor))
					PrintFormat("ScenarioFramework Action %1 detected %2 without RplComponent thus VO won't work!", object, actor);
			}
		
			voiceoverSystem.PlaySequenceFor(m_sSequenceName, actorEntities, playerIDs, m_bPlayImmediately);
		}
	}
	
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
			if (obj)
				course = obj.m_Section.m_Course;
			
			if (!course)
			{
				Print("No course found", LogLevel.ERROR);
				return false;
			}
			
			RKN_TimeTrialCourseData data = RKN_TimeTrialUtils.GetCourseManager().GetData(course.m_iCourseIndex);
			
			if (!data.m_CurrentScoreInfo)
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
}