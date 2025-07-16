class RKN_CallbackDoorUserAction : SCR_DoorUserAction
{
	RKN_CallbackDoorComponent m_Component;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_Component = RKN_CallbackDoorComponent.Cast(GetOwner().FindComponent(RKN_CallbackDoorComponent));
	}

	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		if (m_Component && Replication.IsServer())
			m_Component.m_OnAction.Invoke();
	}
}