[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_Get
{	
	SCR_ScenarioFrameworkParamBase Get(IEntity owner);
	
	protected SCR_ScenarioFrameworkParam<IEntity> findComponentInChildren(IEntity parent, typename component)
	{
		IEntity entity = parent.GetChildren();
		while (entity)
		{
			Managed slot = entity.FindComponent(component);
			if (slot)
				return new SCR_ScenarioFrameworkParam<IEntity>(entity);
			SCR_ScenarioFrameworkParam<IEntity> r = findComponentInChildren(entity, component);
			if (r)
				return r;
			entity = entity.GetSibling();
		}
		return null;
	}
	
	protected SCR_ScenarioFrameworkParam<IEntity> findComponentInParents(IEntity owner, typename component)
	{
		while (owner != null)
		{
			if (owner.FindComponent(component))
				return new SCR_ScenarioFrameworkParam<IEntity>(owner);
			owner = owner.GetParent();
		}
		
		return null;
	}
}

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_GetSF : RKN_Get
{
	[Attribute()]
	ref SCR_ScenarioFrameworkGet m_Getter;
	
	override SCR_ScenarioFrameworkParamBase Get(IEntity owner)
	{
		return m_Getter.Get();
	}
}

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_GetParent : RKN_Get
{	
	override SCR_ScenarioFrameworkParamBase Get(IEntity owner)
	{
		
		return new SCR_ScenarioFrameworkParam<IEntity>(owner.GetParent());
	}
}

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialGetSectionParent : RKN_Get
{	
	override SCR_ScenarioFrameworkParamBase Get(IEntity owner)
	{
		return findComponentInParents(owner, RKN_TimeTrialSectionLayer);
	}
}

[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_TimeTrialGetShooterPosition : RKN_TimeTrialGetSectionParent
{	
	override SCR_ScenarioFrameworkParamBase Get(IEntity owner)
	{
		SCR_ScenarioFrameworkParamBase parentBase = super.Get(owner);
		if (!parentBase)
			return null;
		IEntity entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(parentBase).GetValue();
		return findComponentInChildren(entity, RKN_TimeTrialShooterPositionSlot);
	}
}