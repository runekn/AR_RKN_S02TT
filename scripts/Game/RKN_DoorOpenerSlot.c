class RKN_DoorOpenerSlotClass : SCR_ScenarioFrameworkSlotBaseClass
{
}

class RKN_DoorOpenerSlot : SCR_ScenarioFrameworkSlotBase
{
	[Attribute(defvalue: "10", uiwidget: UIWidgets.Slider, desc: "Radius in which to open door.", "0 1000 1")]
	protected float m_fRadius;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Slider, desc: "How much the doors should be opened", "0. 1 0.01")]
	protected float m_fControlValue;

	
	private ref array<ref DoorStruct> m_aQueriedDoors;
	
	#ifdef WORKBENCH	
	private bool m_bVisualize;
	#endif
	
	//------------------------------------------------------------------------------------------------
	private bool QueryEntities(IEntity e)
	{
		DoorComponent door = DoorComponent.Cast(e.FindComponent(DoorComponent));
		if (door)
			m_aQueriedDoors.Insert(new DoorStruct(e, door));
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	private void GetDoors(float radius)
	{
		BaseWorld world = GetOwner().GetWorld();
		world.QueryEntitiesBySphere(GetOwner().GetOrigin(), radius, QueryEntities);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool InitOtherThings()
	{		
		// capture doors around
		if (!m_aQueriedDoors)
		{
			m_aQueriedDoors = {};
			GetDoors(m_fRadius);
		}
		// set state
		foreach (DoorStruct door : m_aQueriedDoors)
			if (door.component.GetControlValue() != m_fControlValue)
				door.component.SetControlValue(m_fControlValue);
		
		return super.InitOtherThings();
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] object of layer base from which entity is further retrieved
	override bool _WB_OnKeyChanged(IEntity owner, BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		SetDebugShapeSize(m_fRadius);
		return super._WB_OnKeyChanged(owner, src, key, ownerContainers, parent);
	}
}