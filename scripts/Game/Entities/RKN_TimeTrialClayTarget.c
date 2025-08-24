class RKN_TimeTrialClayTargetClass : RKN_TimeTrialTargetEntityClass
{
}

class RKN_TimeTrialClayTarget : RKN_TimeTrialTargetEntity
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Event_TargetHit.Insert(Poof);
	}
	
	void Poof()
	{
		Rpc(RpcDo_Poof, false);
		RpcDo_Poof(false);
	}
	
	override void ResetTarget()
	{
		super.ResetTarget();
		Rpc(RpcDo_Poof, true);
		RpcDo_Poof(true);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_Poof(bool unpoof)
	{
		Show(unpoof);
		Update();
	}
	
}