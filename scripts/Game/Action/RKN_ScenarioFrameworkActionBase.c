[BaseContainerProps(), SCR_ContainerActionTitle()]
modded class SCR_ScenarioFrameworkActionBase
{
	override bool ValidateInputEntity(IEntity object, SCR_ScenarioFrameworkGet getter, out IEntity entity)
	{
		if (!getter && object && !object.FindComponent(SCR_ScenarioFrameworkLayerBase))
		{
			entity = object;
			return true;
		}
		return super.ValidateInputEntity(object, getter, entity);
	}
}