[BaseContainerProps()]
class RKN_CharacterStanceTriggerCondition : SCR_CustomTriggerConditions
{
	[Attribute(".STAND", UIWidgets.ComboBox, enumType: ECharacterStance)]
	ECharacterStance m_Stance;
	
	override bool Init(IEntity entity)
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(entity);
		if (player)
			return TestStance(player);
		
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(entity);
		if (trigger)
		{
			array<IEntity> players = {};
			trigger.GetPlayersByFactionInsideTrigger(players);
			foreach (IEntity pEntity : players)
			{
				SCR_ChimeraCharacter player1 = SCR_ChimeraCharacter.Cast(pEntity);
				if (player1)
					return TestStance(player1);
			}
		}
		return true;
	}
	
	bool TestStance(SCR_ChimeraCharacter player)
	{
		return player.GetCharacterController().GetStance() == m_Stance;
	}
}