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
		Show(false);
		Update();
	}
	
	override void ResetTarget()
	{
		super.ResetTarget();
		Show(true);
		Update();
	}
}