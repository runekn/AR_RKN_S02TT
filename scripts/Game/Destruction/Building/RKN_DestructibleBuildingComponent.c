modded class SCR_DestructibleBuildingComponent
{
	bool m_bFakeDestruction;
	bool m_bFakeDone;
	vector original[4];
	int m_iResponseIndex;
	
	void InitFakeDetruction()
	{	
		IEntity owner = GetOwner();
		
		m_bFakeDone = false;
		m_bFakeDestruction = true;
		owner.GetTransform(original);
		
		SCR_BuildingDestructionData data = GetData();
		if (!data)
			return;
		
		owner.GetWorldTransform(data.m_vStartMatrix);
		
		CalculateAndStoreVolume();
		SpawnEffects(0, owner, false);

		vector mins, maxs;
		owner.GetBounds(mins, maxs);

		maxs[0] = 0;
		maxs[2] = 0;

		vector sinkVector = GetSinkVector();
		if (sinkVector == vector.Zero)
			sinkVector = -maxs;

		data.m_vTargetOrigin = owner.GetOrigin() + sinkVector;
		data.m_vStartAngles = owner.GetLocalAngles();
		
		m_iResponseIndex = owner.GetPhysics().GetResponseIndex();
		//owner.GetPhysics().SetResponseIndex(NO_COLLISION_RESPONSE_INDEX);
		data.m_CameraShake.SetParams(0.15, 0.15, 0.01, 400, 0.24);
		data.m_CameraShake.SetCurve(GetCameraShakeCurve());
		data.m_CameraShake.SetStartOrigin(data.m_vStartMatrix[3]);
		data.m_CameraShake.SetSizeMultiplier(data.m_fSizeMultiplier);
		
		SCR_CameraShakeManagerComponent.AddCameraShake(data.m_CameraShake);
		
		// TODO: not MP friendly! Look for players in radius. (or in case this runs on all client, check if this player is close by)
		PlayerController controller = GetGame().GetPlayerController();
		if (controller)
		{
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(controller.GetPlayerId());
			if (player)
				GetGame().GetCallqueue().CallLater(DamageOccupantsDelayed, (GetDelay() + 0.5) * 1000, param1: player);
		}
	}
	
	float GetDelayPublic()
	{
		return GetDelay();
	}
	
	void EndFakeDestruction()
	{
		m_bFakeDone = true;
		IEntity owner = GetOwner();
		// Reverse building transformation
		owner.SetTransform(original);
		owner.GetPhysics().SetResponseIndex(m_iResponseIndex);
		// Kill occupants
		// TODO
		// Stop effects
		SCR_BuildingDestructionData data = GetData();
		if (data)
		{
			foreach (IEntity entity : data.m_aExcludeList)
				SCR_EntityHelper.DeleteEntityAndChildren(entity);
		}
		data.m_CameraShake.SetParams(0.15, 0.15, 0.01, 0.3, 0.24);
		data.m_CameraShake = null;
		data.m_aQueriedEntities = null;
		FreeData();
	}
	
	void OnFrameFake(IEntity owner, float timeSlice)
	{
		LerpPosition(owner, timeSlice);
		LerpRotation(owner, timeSlice);	
	}
	
	override protected array<ref SCR_TimedEffect> GetEffects()
	{
		array<ref SCR_TimedEffect> effects = super.GetEffects();
		if (m_bFakeDestruction)
		{
			for (int i = 0; i < effects.Count(); i++)
			{
				if (SCR_TimedPrefab.Cast(effects[i]))
				{
					effects.Remove(i);
					break;
				}
			}
		}
		return effects;
	}
	
	override protected void FinishLerp(IEntity owner, bool immediate, bool updateEntity)
	{
		if (m_bFakeDestruction)
		{
			EndFakeDestruction();
			return;
		}
		super.FinishLerp(owner, immediate, updateEntity)
	}
}