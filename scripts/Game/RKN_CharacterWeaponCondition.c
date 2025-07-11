[BaseContainerProps()]
class RKN_CharacterWeaponTriggerCondition : SCR_CustomTriggerConditions
{
	[Attribute(".WT_RIFLE", UIWidgets.ComboBox, enumType: EWeaponType)]
	EWeaponType m_eWeaponType;
	
	override bool Init(IEntity entity)
	{
		SCR_ScenarioFrameworkTriggerEntity trigger = SCR_ScenarioFrameworkTriggerEntity.Cast(entity);
		array<IEntity> players = {};
		trigger.GetPlayersByFactionInsideTrigger(players);
		foreach (IEntity pEntity : players)
		{
			SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(pEntity);
			if (player)
			{
				BaseWeaponComponent weap = player.GetCharacterController().GetWeaponManagerComponent().GetCurrentWeapon();
				return weap && weap.GetWeaponType() == m_eWeaponType;
			}
		}
		return true;
	}
}