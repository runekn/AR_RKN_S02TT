class RKN_FakeBuildingDestructionEntityClass : GenericEntityClass
{
}

class RKN_FakeBuildingDestructionEntity : GenericEntity
{	
	[Attribute("1")]
	float m_fRadius;
	
	IEntity m_Entity;
	SCR_DestructibleBuildingComponent m_Comp;
	
	void RKN_FakeBuildingDestructionEntity(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		#ifdef WORKBENCH
		if (!GetGame().InPlayMode())
			return;
		#endif
		BaseWorld world = GetWorld();
		world.QueryEntitiesBySphere(GetOrigin(), m_fRadius, QueryEntities);
		if (m_Comp)
		{
			m_Comp.InitFakeDetruction();
			GetGame().GetCallqueue().CallLater(StartOnFrame, 1000 * m_Comp.GetDelayPublic());
		}
	}
		
	private bool QueryEntities(IEntity e)
	{
		SCR_DestructibleBuildingComponent comp = SCR_DestructibleBuildingComponent.Cast(e.FindComponent(SCR_DestructibleBuildingComponent));
		if (comp && !m_Entity)
		{
			Print("Starting fake building destruction");
			m_Entity = e;
			m_Comp = comp;
		}
		
		return true;
	}
	
	void StartOnFrame()
	{
		SetEventMask(EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_Comp.m_bFakeDone)
		{
			Print("Ending fake building destruction. Deleting self.");
			ClearEventMask(EntityEvent.FRAME);
			delete owner;	
			return;
		}
		
		m_Comp.OnFrameFake(m_Entity, timeSlice);
	}
}