[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_Get
{	
	SCR_ScenarioFrameworkParamBase Get(IEntity owner);
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
class RKN_TimeTrialGetShooterPosition : RKN_Get
{	
	override SCR_ScenarioFrameworkParamBase Get(IEntity owner)
	{
		IEntity entity = owner.GetParent().GetChildren();
		while (entity)
		{
			RKN_TimeTrialShooterPositionSlot slot = RKN_TimeTrialShooterPositionSlot.Cast(entity.FindComponent(RKN_TimeTrialShooterPositionSlot));
			if (slot)
				return new SCR_ScenarioFrameworkParam<IEntity>(entity);
			entity = entity.GetSibling();
		}
		return null;
	}
}