[BaseContainerProps()]
class RKN_CharacterStanceTriggerCondition : SCR_CustomTriggerConditions
{
	[Attribute(".STAND", UIWidgets.ComboBox, enumType: ECharacterStance)]
	ECharacterStance m_Stance;
	
	override bool Init(IEntity entity)
	{
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(entity);
		array<IEntity> players = {};
		trigger.GetPlayersByFactionInsideTrigger(players);
		foreach (IEntity pEntity : players)
		{
			SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pEntity);
			if (player && player.GetCharacterController().GetStance() != m_Stance)
				return false;
		}
		return true;
	}
}