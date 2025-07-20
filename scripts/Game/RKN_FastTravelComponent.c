modded class SCR_FastTravelComponent
{
	void ToggleMapDestinationSelectionOwner(bool enable, bool disableAfterUse = true)
	{
		Rpc(Rpc_ToggleMapDestinationSelectionOwner, enable, disableAfterUse);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 	void Rpc_ToggleMapDestinationSelectionOwner(bool enable, bool disableAfterUse)
	{
		ToggleMapDestinationSelection(enable, disableAfterUse);
	}
}