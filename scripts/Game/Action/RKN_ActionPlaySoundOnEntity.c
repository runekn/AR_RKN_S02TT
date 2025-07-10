[BaseContainerProps(), SCR_ContainerActionTitle()]
class RKN_ActionPlaySoundOnEntity : SCR_ScenarioFrameworkActionBase
{
	[Attribute()]
	ref RKN_Get m_RknGetter;

	[Attribute(desc: "Sound to play.")]
	string 			m_sSound;

	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object)
	{
		if (!CanActivate())
			return;

		IEntity entity;
		if (!ValidateInputEntity(object, null, entity))
			return;

		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (soundManagerEntity)
		{
			soundManagerEntity.CreateAndPlayAudioSource(entity, m_sSound);
		}
	}
	
	override bool ValidateInputEntity(IEntity object, SCR_ScenarioFrameworkGet getter, out IEntity entity)
	{
		if (!m_RknGetter && object)
		{
			SCR_ScenarioFrameworkLayerBase layer = SCR_ScenarioFrameworkLayerBase.Cast(object.FindComponent(SCR_ScenarioFrameworkLayerBase));
			if (!layer)
			{
				Print(string.Format("ScenarioFramework Action: Action %1 attached on %2 is not called from layer and won't do anything.", this, object.GetName()), LogLevel.ERROR);
				return false;
			}

			entity = layer.GetSpawnedEntity();
		}
		else
		{
			SCR_ScenarioFrameworkParam<IEntity> entityWrapper = SCR_ScenarioFrameworkParam<IEntity>.Cast(m_RknGetter.Get(object));
			if (!entityWrapper)
			{
				if (object)
					Print(string.Format("ScenarioFramework Action: Issue with Getter detected for Action %1 attached on %2.", this, object.GetName()), LogLevel.ERROR);
				else
					Print(string.Format("ScenarioFramework Action: Issue with Getter detected for Action %1.", this), LogLevel.ERROR);

				return false;
			}

			entity = entityWrapper.GetValue();
		}

		if (!entity)
		{
			if (object)
				Print(string.Format("ScenarioFramework Action: Entity not found for Action %1 attached on %2.", this, object.GetName()), LogLevel.ERROR);
			else
				Print(string.Format("ScenarioFramework Action: Entity not found for Action %1.", this), LogLevel.ERROR);

			return false;
		}

		return true;
	}
}