[EntityEditorProps(category: "GameScripted/ScenarioFramework/Layer", description: "")]
class RKN_TimeTrialCourseLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialCourseLayer : SCR_ScenarioFrameworkLayerBase
{		
	[Attribute(desc: "Must be the same defined in RKN_TimeTrialScoreRepository config", category: "Time trial")]
	string m_sCourseId;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnScheduleActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnCancelActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnFinishActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnFailActions;
	
	ref ScriptInvoker m_OnCancel = new ScriptInvoker();
	ref ScriptInvoker m_OnReset = new ScriptInvoker();
	ref ScriptInvoker m_OnSchedule = new ScriptInvoker();
	ref ScriptInvoker m_OnActive = new ScriptInvoker();
	ref ScriptInvoker m_OnFinish = new ScriptInvoker();
	
	ref array<RKN_TimeTrialSectionLayer> m_aSections = {};
	int m_iActiveSection;
	RKN_TimeTrialCourseManagerComponent m_Manager;
	int m_iCourseIndex;
	
	override bool InitOtherThings()
	{
		m_iCourseIndex = GetCourseManager().RegisterCourse(this);
		return super.InitOtherThings();
	}
	
	void CancelCourse(bool actions = true)
	{
		GetCourseManager().StopTime(m_iCourseIndex, true);
		GetGame().GetCallqueue().Remove(StartCourse);
		GetGame().GetCallqueue().Remove(ResetRun);
		RKN_TimeTrialCourseData data = GetCourseManager().GetData(m_iCourseIndex);
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(data.m_CurrentScoreInfo.m_iID)).RemoveScoreTable(m_iCourseIndex, true);
		m_iActiveSection = 0;
		m_OnCancel.Invoke();
		if (actions)
			foreach (SCR_ScenarioFrameworkActionBase action : m_aOnCancelActions)
				action.Init(GetOwner());
		GetCourseManager().ClearCurrentScore(m_iCourseIndex);
	}
	
	void ScheduleCourse(int playerId, int delay, bool competitive)
	{
		Print("RKN_TimeTrialCourseLayer: RpcAsk_ScheduleCourse");
		if (GetCourseManager().HasActiveCompetitor(m_iCourseIndex))
		{
			Print("RKN_TimeTrialCourseLayer: Only one player can use the course at a time", LogLevel.ERROR);
			return;
		}
		
		if (GetCourseManager().IsActiveCompetitor(playerId))
		{
			Print("RKN_TimeTrialCourseLayer: Player already has active course", LogLevel.ERROR);
			return;
		}
		
		RKN_TimeTrialScoreType type = RKN_TimeTrialScoreType.TRAINING;
		if (competitive)
			type = RKN_TimeTrialScoreType.COMPETITIVE;
		GetCourseManager().CreateActiveScore(m_iCourseIndex, playerId, type);
		
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		FindPlayerUIComponent(player).ShowScoreTable(m_iCourseIndex, true);
		
		if (competitive)
			ApplyCompetitiveLoadout(player);
		
		m_iActiveSection = 0;
		
		GetGame().GetCallqueue().CallLater(StartCourse, delay, false);
		
		m_OnSchedule.Invoke();
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnScheduleActions)
			action.Init(GetOwner());
	}
	
	private void StartCourse()
	{
		GetCourseManager().StartTime(m_iCourseIndex);
		m_OnActive.Invoke();
		ActivateNextSectionOrFinish();
	}
	
	void FailCourse()
	{
		Print("RKN_TimeTrialCourseLayer: Fail!");
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnFailActions)
			action.Init(GetOwner());
		CancelCourse(false);
	}
	
	void RegisterSection(RKN_TimeTrialSectionLayer section)
	{
		m_aSections.Insert(section);
	}
	
	void FinishSection(RKN_TimeTrialSectionLayer section)
	{
		ActivateNextSectionOrFinish();
	}
	
	void ActivateNextSectionOrFinish()
	{
		if (m_iActiveSection >= m_aSections.Count())
			FinishCourse();
		else
			m_aSections[m_iActiveSection++].ActivateSection();
	}
	
	private void FinishCourse()
	{
		GetCourseManager().StopTime(m_iCourseIndex, false);
		GetGame().GetCallqueue().CallLater(ResetRun, 5000, false);
		m_OnFinish.Invoke();
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnFinishActions)
			action.Init(GetOwner());
	}
	
	void ApplyScoreModifier(int mod)
	{
		if (mod > 0)
			GetCourseManager().ApplyPenalty(m_iCourseIndex, mod);
		if (mod < 0)
			GetCourseManager().ApplyBonus(m_iCourseIndex, -mod);
	}
	
	private void ResetRun()
	{
		Print("RKN_TimeTrialCourseLayer: ResetRun");
		RKN_TimeTrialCourseData data = GetCourseManager().GetData(m_iCourseIndex);
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(data.m_CurrentScoreInfo.m_iID)).RemoveScoreTable(m_iCourseIndex, true);
		GetCourseManager().ClearCurrentScore(m_iCourseIndex);
		m_iActiveSection = 0;
		m_OnReset.Invoke();
	}
	
	RKN_TimeTrialScoreTablePlayerComponent FindPlayerUIComponent(IEntity player)
	{
		return RKN_TimeTrialScoreTablePlayerComponent.Cast(player.FindComponent(RKN_TimeTrialScoreTablePlayerComponent));
	}
	
	void ApplyCompetitiveLoadout(IEntity player)
	{
		RKN_TimeTrialCourseData data = GetCourseManager().GetData(m_iCourseIndex);
		if (data.m_Config.m_aCompetitiveLoadout.IsEmpty())
			return;
		
		// Delete current loadout - except clothing
		SCR_InventoryStorageManagerComponent comp = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		array<IEntity> items = {};
		comp.GetItems(items);
		foreach (IEntity item : items)
		{
			if (!item)
				continue;
			BaseLoadoutClothComponent cloth = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
			if (cloth && 
				(LoadoutHandwearSlotArea.Cast(cloth.GetAreaType()) || 
				LoadoutBootsArea.Cast(cloth.GetAreaType()) || 
				LoadoutHeadCoverArea.Cast(cloth.GetAreaType()) ||
				LoadoutJacketArea.Cast(cloth.GetAreaType()) || 
				LoadoutPantsArea.Cast(cloth.GetAreaType()))
			)
				continue;
			
			SCR_GadgetComponent gadget = SCR_GadgetComponent.Cast(item.FindComponent(SCR_GadgetComponent));
			if (gadget)
				continue;
			
			comp.TryDeleteItem(item);
		}
		
		// Insert competitive loadout
		foreach (RKN_TimeTrialLoadoutItem loadoutItem : data.m_Config.m_aCompetitiveLoadout)
		{
			InventoryOperationCallback cb = null;		
			if (loadoutItem.m_bEquip)
				cb = new RKN_EquipItemCallback(CharacterControllerComponent.Cast(player.FindComponent(CharacterControllerComponent)));
			comp.TrySpawnPrefabToStorage(loadoutItem.m_sPrefab, count: loadoutItem.m_iCount, cb: cb);
		}
	}
	
	RKN_TimeTrialCourseManagerComponent GetCourseManager()
	{
		if (!m_Manager)
		{
			m_Manager = RKN_TimeTrialUtils.GetCourseManager();
		}
		return m_Manager;
	}
}

class RKN_EquipItemCallback : ScriptedInventoryOperationCallback
{
	CharacterControllerComponent m_pController;
	
	void RKN_EquipItemCallback(CharacterControllerComponent pController)
	{
		m_pController = pController;
	}
	
	override void OnComplete()
	{
		RplId id = GetItem();
		IEntity e = RplComponent.Cast(Replication.FindItem(id)).GetEntity();
		m_pController.TryEquipRightHandItem(e, EEquipItemType.EEquipTypeWeapon);
	}
	
	override void OnFailed()
	{
		Print("RKN_TimeTrialCourseLayer: Failed to equip item", LogLevel.ERROR);
	}
}