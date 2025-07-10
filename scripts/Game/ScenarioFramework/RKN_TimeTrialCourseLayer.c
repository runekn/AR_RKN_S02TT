[EntityEditorProps(category: "GameScripted/ScenarioFramework/Layer", description: "")]
class RKN_TimeTrialCourseLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialCourseLayer : SCR_ScenarioFrameworkLayerBase
{	
	[Attribute(category: "Time trial")]
	ref RKN_TimeTrialCourseConfig m_Config;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnScheduleActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnCancelActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnFinishActions;
	
	[Attribute("{62AA1DC9919E6E61}Prefabs/TimeTrial_Radio_ANPRC68.et", params: "et", category: "Time trial")]
	private ResourceName m_sRadioPrefab;
	
	ref ScriptInvoker m_OnCancel = new ScriptInvoker();
	ref ScriptInvoker m_OnReset = new ScriptInvoker();
	ref ScriptInvoker m_OnSchedule = new ScriptInvoker();
	ref ScriptInvoker m_OnActive = new ScriptInvoker();
	ref ScriptInvoker m_OnFinish = new ScriptInvoker();
	
	ref array<RKN_TimeTrialSectionLayer> m_aSections = {};
	int m_iActiveSection;
	[RplProp()]
	ref RKN_TimeTrialScoreInfo m_CurrentScoreInfo;
	[RplProp()]
	ref array<ref RKN_TimeTrialScoreInfo> m_aScoreInfoHistory = new array<ref RKN_TimeTrialScoreInfo>;
	
	void CancelCourse()
	{
		Rpc(RpcAsk_CancelCourse);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_CancelCourse()
	{
		m_CurrentScoreInfo.m_iEnd = GetGame().GetWorld().GetTimestamp();
		GetGame().GetCallqueue().Remove(StartCourse);
		GetGame().GetCallqueue().Remove(ResetRun);
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(m_CurrentScoreInfo.m_iID)).RemoveScoreTable(this, true);
		ResetCourse();
		m_OnCancel.Invoke();
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnCancelActions)
			action.Init(GetOwner());
		Replication.BumpMe();
	}
	
	void ScheduleCourse(int playerId, int delay, bool competitive)
	{
		Rpc(RpcAsk_ScheduleCourse, playerId, delay, competitive);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_ScheduleCourse(int playerId, int delay, bool competitive)
	{
		Print("RpcAsk_ScheduleCourse");
		if (m_CurrentScoreInfo)
		{
			Print("Only one player can use the course at a time", LogLevel.ERROR);
			return;
		}
		m_CurrentScoreInfo = new RKN_TimeTrialScoreInfo();
		if (competitive)
			m_CurrentScoreInfo.m_eType = RKN_TimeTrialScoreType.COMPETITIVE;
		else
			m_CurrentScoreInfo.m_eType = RKN_TimeTrialScoreType.TRAINING;
		m_CurrentScoreInfo.m_iID = playerId;
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		FindPlayerUIComponent(player).ShowScoreTable(this, true);
		
		if (competitive)
			ApplyCompetitiveLoadout(player);
		
		GetGame().GetCallqueue().CallLater(StartCourse, delay, false);
		ResetCourse(resetScore: false);
		m_OnSchedule.Invoke();
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnScheduleActions)
			action.Init(GetOwner());
		Replication.BumpMe();
	}
	
	private void StartCourse()
	{
		m_OnActive.Invoke();
		m_CurrentScoreInfo.m_iStart = GetGame().GetWorld().GetTimestamp();
		ActivateNextSectionOrFinish();
		Replication.BumpMe();
	}
	
	void FailCourse()
	{
		Print("Fail!");
		RpcAsk_CancelCourse();
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
	
	private void ResetCourse(bool resetScore = true)
	{
		if (resetScore)
			m_CurrentScoreInfo = null;
		m_iActiveSection = 0;
	}
	
	private void FinishCourse()
	{
		m_CurrentScoreInfo.m_iEnd = GetGame().GetWorld().GetTimestamp();
		if (m_CurrentScoreInfo.m_eType == RKN_TimeTrialScoreType.COMPETITIVE)
			SubmitScore();
		GetGame().GetCallqueue().CallLater(ResetRun, 5000, false);
		m_OnFinish.Invoke();
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnFinishActions)
			action.Init(GetOwner());
		Replication.BumpMe();
	}
	
	private void SubmitScore()
	{
		bool newBest = true;
		int prevIndex = -1;
		int bestIndex = -1;
		for (int i = 0; i < m_aScoreInfoHistory.Count(); i++)
		{
			RKN_TimeTrialScoreInfo info = m_aScoreInfoHistory[i];
			if (info.m_iID == m_CurrentScoreInfo.m_iID)
			{
				if (info.m_eType == RKN_TimeTrialScoreType.PREVIOUS)
				{
					prevIndex = i;
				}
				else
				{
					if (info.GetTotal() > m_CurrentScoreInfo.GetTotal())
						bestIndex = i;
					else
						newBest = false;
				}
			}
			
			// Prepare for sorting
			if (info.m_iTotal == 0)
				info.m_iTotal = info.GetTotal();
		}
		
		if (prevIndex != -1)
			m_aScoreInfoHistory[prevIndex] = m_CurrentScoreInfo.CopyAs(RKN_TimeTrialScoreType.PREVIOUS);
		else
			m_aScoreInfoHistory.Insert(m_CurrentScoreInfo.CopyAs(RKN_TimeTrialScoreType.PREVIOUS));
			
		if (newBest)
		{
			if (bestIndex != -1)
				m_aScoreInfoHistory[bestIndex] = m_CurrentScoreInfo.CopyAs(RKN_TimeTrialScoreType.BEST);
			else
				m_aScoreInfoHistory.Insert(m_CurrentScoreInfo.CopyAs(RKN_TimeTrialScoreType.BEST));
		}
		
		m_aScoreInfoHistory.Sort(true);
	}
	
	void ApplyScoreModifier(int mod)
	{
		if (m_CurrentScoreInfo.m_iEnd)
			return;
		if (mod > 0)
			m_CurrentScoreInfo.m_iPenalty += mod;
		if (mod < 0)
			m_CurrentScoreInfo.m_iBonus += -mod;
		Replication.BumpMe();
	}
	
	private void ResetRun()
	{
		Print("ResetRun");
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(m_CurrentScoreInfo.m_iID)).RemoveScoreTable(this, true);
		ResetCourse();
		m_OnReset.Invoke();
		Replication.BumpMe();
	}
	
	RKN_TimeTrialScoreTablePlayerComponent FindPlayerUIComponent(IEntity player)
	{
		return RKN_TimeTrialScoreTablePlayerComponent.Cast(player.FindComponent(RKN_TimeTrialScoreTablePlayerComponent));
	}
	
	void ApplyCompetitiveLoadout(IEntity player)
	{
		if (m_Config.m_aCompetitiveLoadout.IsEmpty())
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
				LoadoutPantsArea.Cast(cloth.GetAreaType()) || 
				LoadoutPantsArea.Cast(cloth.GetAreaType()))
			)
				continue;
			
			if (item.GetPrefabData().GetPrefabName() == m_sRadioPrefab)
				continue;
			
			comp.TryDeleteItem(item);
		}
		
		// Insert competitive loadout
		foreach (RKN_TimeTrialLoadoutItem loadoutItem : m_Config.m_aCompetitiveLoadout)
		{
			InventoryOperationCallback cb = null;		
			if (loadoutItem.m_bEquip)
				cb = new RKN_EquipItemCallback(CharacterControllerComponent.Cast(player.FindComponent(CharacterControllerComponent)));
			comp.TrySpawnPrefabToStorage(loadoutItem.m_sPrefab, count: loadoutItem.m_iCount, cb: cb);
		}
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
		Print("Failed to equip item", LogLevel.ERROR);
	}
}