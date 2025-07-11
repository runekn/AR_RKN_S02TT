[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_StartVehicleEngineAction : SCR_ScenarioFrameworkActionBase
{
	[Attribute()]
	ref RKN_Get m_VehicleGetter;
	
	override void OnActivate(IEntity object)
	{
		IEntity entity = object;
		if (m_VehicleGetter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = m_VehicleGetter.Get(object);
			if (paramBase)
			{
				entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
			}
		}
		
		if (!entity)
		{
			Print("No entity found!", LogLevel.ERROR);
			return;
		}
		
		BaseVehicleControllerComponent controller = BaseVehicleControllerComponent.Cast(entity.FindComponent(BaseVehicleControllerComponent));
		if (!controller)
		{
			Print("Entity is no vehicle!", LogLevel.ERROR);
			return;
		}
		controller.ForceStartEngine();
	}
}