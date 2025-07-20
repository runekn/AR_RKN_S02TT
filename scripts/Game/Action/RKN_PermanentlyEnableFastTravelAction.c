[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_PermanentlyEnableFastTravelAction : SCR_ScenarioFrameworkActionBase
{
	override void OnActivate(IEntity object)
	{
		int id = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(object);
		if (id == 0)
		{
			Print("RKN_PermanentlyEnableFastTravelAction: Player not found", LogLevel.ERROR);
			return;
		}
		SCR_FastTravelComponent comp = SCR_FastTravelComponent.Cast(GetGame().GetPlayerManager().GetPlayerController(id).FindComponent(SCR_FastTravelComponent));
		comp.ToggleMapDestinationSelectionOwner(true, false);
	}
}