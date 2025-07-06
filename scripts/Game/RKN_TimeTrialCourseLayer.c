[EntityEditorProps(category: "GameScripted/ScenarioFramework/Layer", description: "")]
class RKN_TimeTrialCourseLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialCourseLayer : SCR_ScenarioFrameworkLayerBase
{	
	[Attribute(params: "conf", category: "Time trial")]
	ResourceName m_CourseConfig;
	
	ref array<RKN_TimeTrialSectionLayer> m_aSections = {};
	ref RKN_TimeTrialCourseConfig m_Config;
	int m_iActiveSection;
	[RplProp()]
	ref RKN_TimeTrialScoreInfo m_CurrentScoreInfo;
	[RplProp()]
	ref array<ref RKN_TimeTrialScoreInfo> m_aScoreInfoHistory = new array<ref RKN_TimeTrialScoreInfo>;
	
	override void FinishInit()
	{
		Resource configContainer = BaseContainerTools.LoadContainer(m_CourseConfig);
		if (configContainer && configContainer.IsValid())
			m_Config = RKN_TimeTrialCourseConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(configContainer.GetResource().ToBaseContainer()));
		super.FinishInit();
	}
	
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
		ResetRun();
	}
	
	void ScheduleCourse(int playerId, int delay, bool competitive)
	{
		Rpc(RpcAsk_ScheduleCourse, playerId, delay, competitive);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_ScheduleCourse(int playerId, int delay, bool competitive)
	{
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
		Replication.BumpMe();
	}
	
	private void StartCourse()
	{
		m_CurrentScoreInfo.m_iStart = GetGame().GetWorld().GetTimestamp();
		ActivateNextSectionOrFinish();
		Replication.BumpMe();
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
	
	private void ResetCourse()
	{
		m_CurrentScoreInfo = null;
		m_iActiveSection = 0;
		foreach(RKN_TimeTrialSectionLayer section : m_aSections)
			section.ResetSection();
	}
	
	private void FinishCourse()
	{
		m_CurrentScoreInfo.m_iEnd = GetGame().GetWorld().GetTimestamp();
		if (m_CurrentScoreInfo.m_eType == RKN_TimeTrialScoreType.COMPETITIVE)
			SubmitScore();
		GetGame().GetCallqueue().CallLater(ResetRun, 5000, false);
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
		if (mod > 0)
			m_CurrentScoreInfo.m_iPenalty += mod;
		if (mod < 0)
			m_CurrentScoreInfo.m_iBonus += -mod;
		Replication.BumpMe();
	}
	
	private void ResetRun()
	{
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(m_CurrentScoreInfo.m_iID)).RemoveScoreTable(this, true);
		ResetCourse();
		Replication.BumpMe();
	}
	
	RKN_TimeTrialScoreTablePlayerComponent FindPlayerUIComponent(IEntity player)
	{
		return RKN_TimeTrialScoreTablePlayerComponent.Cast(player.FindComponent(RKN_TimeTrialScoreTablePlayerComponent));
	}
	
	void ApplyCompetitiveLoadout(IEntity player)
	{
		if (!m_Config.m_sCompetitiveLoadout)
			return;
		SCR_BasePlayerLoadout loadout = GetGame().GetLoadoutManager().GetLoadoutByName(m_Config.m_sCompetitiveLoadout);
		if (!loadout)
		{
			Print("Loadout not found: " + m_Config.m_sCompetitiveLoadout, LogLevel.ERROR);
			return;
		}
		InventoryStorageManagerComponent comp = InventoryStorageManagerComponent.Cast(player.FindComponent(InventoryStorageManagerComponent));
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
			
			comp.TryDeleteItem(item);
		}
	}
}