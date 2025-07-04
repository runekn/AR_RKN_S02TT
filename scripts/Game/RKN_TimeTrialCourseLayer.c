[EntityEditorProps(category: "GameScripted/ScenarioFramework/Layer", description: "")]
class RKN_TimeTrialCourseLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialCourseLayer : SCR_ScenarioFrameworkLayerBase
{	
	[Attribute(params: "conf")]
	ResourceName m_CourseConfig;
	
	// Execution related
	[RplProp()]
	int m_iPlayer;
	ref array<RKN_TimeTrialSectionLayer> m_aSections = {};
	int m_iActiveSection;
	
	// Score related
	[RplProp()]
	WorldTimestamp m_StartTimestamp;
	[RplProp()]
	int m_iModifiers;
	[RplProp()]
	ref array<ref RKN_TimeTrialScoreInfo> m_aAllPlayersInfo = new array<ref RKN_TimeTrialScoreInfo>;
	ref RKN_TimeTrialCourseConfig m_Config;
	
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
		GetGame().GetCallqueue().Remove(StartCourse);
		RemoveScoreTable(m_iPlayer);
		ResetCourse();
	}
	
	void ScheduleCourse(int playerId, int delay)
	{
		Rpc(RpcAsk_ScheduleCourse, playerId, delay);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_ScheduleCourse(int playerId, int delay)
	{
		if (m_iPlayer > 0)
		{
			Print("Only one player can use the course at a time", LogLevel.ERROR);
			return;
		}
		m_iPlayer = playerId;
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		FindPlayerUIComponent(player).ShowScoreTable(this, true);
		
		GetGame().GetCallqueue().CallLater(StartCourse, delay, false);
		Replication.BumpMe();
	}
	
	private void StartCourse()
	{
		m_StartTimestamp = GetGame().GetWorld().GetTimestamp();
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
		m_StartTimestamp = null;
		m_iModifiers = 0;
		m_iActiveSection = 0;
		m_iPlayer = 0;
		foreach(RKN_TimeTrialSectionLayer section : m_aSections)
			section.ResetSection();
	}
	
	private void FinishCourse()
	{
		WorldTimestamp timestamp = GetGame().GetWorld().GetTimestamp();
		float time = timestamp.DiffMilliseconds(m_StartTimestamp);
		SubmitScore(m_iPlayer, time, time + m_iModifiers);
		Print("Finished course in " + time + " ms");
		GetGame().GetCallqueue().CallLater(RemoveScoreTable, 5000, false, m_iPlayer);
		ResetCourse();
		Replication.BumpMe();
	}
	
	private void SubmitScore(int player, int time, int total)
	{
		bool n = true;
		foreach (RKN_TimeTrialScoreInfo info : m_aAllPlayersInfo)
		{
			if (info.m_iID == m_iPlayer)
			{
				info.m_iPrevTime = time;
				info.m_iPrevTotal = total;
				if (info.m_iBestTotal > total)
				{
					info.m_iBestTime = time;
					info.m_iBestTotal = total;
				}
				n = false;
				break;
			}
		}
		
		if (n)
		{
			RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
			info.m_iPrevTime = time;
			info.m_iPrevTotal = total;
			info.m_iBestTime = time;
			info.m_iBestTotal = total;
			info.m_iID = player;
			m_aAllPlayersInfo.Insert(info);
		}
		
		// TODO: sort
	}
	
	void ApplyScoreModifier(int mod)
	{
		m_iModifiers += mod;
		Replication.BumpMe();
	}
	
	private void RemoveScoreTable(int playerId)
	{
		FindPlayerUIComponent(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId)).RemoveScoreTable(this, true);
	}
	
	RKN_TimeTrialScoreTablePlayerComponent FindPlayerUIComponent(IEntity player)
	{
		return RKN_TimeTrialScoreTablePlayerComponent.Cast(player.FindComponent(RKN_TimeTrialScoreTablePlayerComponent));
	}
}