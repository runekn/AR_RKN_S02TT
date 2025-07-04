class RKN_TimeTrialScoreTablePlayerComponentClass : ScriptComponentClass
{
}

class RKN_TimeTrialScoreTablePlayerComponent : ScriptComponent
{
	RKN_TimeTrialCourseLayer m_ActiveCourse;
	RplComponent m_RplComponent;
	Widget m_wRoot;
	Widget m_wTable;
	ref array<Widget> m_aPlayerWidgets = {};
	Widget m_wActivePlayerWidget;
	Widget m_wGoldWidget;
	Widget m_wSilverWidget;
	Widget m_wBronzeWidget;
	bool m_bActive;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}
	
	void ShowScoreTable(RKN_TimeTrialCourseLayer courseLayer, bool active)
	{
		RplId courseLayerId = Replication.FindId(courseLayer);
		Rpc(RpcDo_ShowScoreTable, courseLayerId, active);
	}
	
	void RemoveScoreTable(RKN_TimeTrialCourseLayer courseLayer, bool active)
	{
		RplId courseLayerId = Replication.FindId(courseLayer);
		Rpc(RpcDo_RemoveScoreTable, courseLayerId, active);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ShowScoreTable(RplId courseLayerId, bool active)
	{
		m_ActiveCourse = RKN_TimeTrialCourseLayer.Cast(Replication.FindItem(courseLayerId));
		if (active && !m_bActive)
			m_bActive = active;
		SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_RemoveScoreTable(RplId courseLayerId, bool active)
	{
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(Replication.FindItem(courseLayerId));
		if ((active || !m_bActive) && m_ActiveCourse == course)
		{
			m_wRoot.RemoveFromHierarchy();
			m_wRoot = null;
			m_ActiveCourse = null;
			m_aPlayerWidgets.Clear();
			if (m_bActive)
				m_bActive = false;
			ClearEventMask(GetOwner(), EntityEvent.FRAME);
		}
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_RplComponent)
			m_RplComponent = BaseGameEntity.Cast(GetOwner()).GetRplComponent();
		if (m_ActiveCourse && m_RplComponent.IsOwner())
		{
			UpdateTable();
		}
	}
	
	void UpdateTable()
	{
		int me = GetGame().GetPlayerController().GetPlayerId();
		
		if (!m_wRoot)
		{
			m_wRoot = GetGame().GetWorkspace().CreateWidgets("{3440F82E4043655C}UI/layouts/HUD/TimeTrial/TimeTrialScoreTable.layout");	
			if (!m_wRoot)
				return;
			
			m_wTable = m_wRoot.FindAnyWidget("Table");
			
			m_wActivePlayerWidget = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
			m_wGoldWidget = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
			m_wSilverWidget = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
			m_wBronzeWidget = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
		}
		
		if (m_aPlayerWidgets.Count() != m_ActiveCourse.m_aAllPlayersInfo.Count()) // TODO: do something smarter
		{
			// Delete and generate new rows for current players
			foreach (Widget w : m_aPlayerWidgets)
			{
				w.RemoveFromHierarchy();
			}
			m_aPlayerWidgets.Clear();
			
			foreach (RKN_TimeTrialScoreInfo info : m_ActiveCourse.m_aAllPlayersInfo)
			{
				if (info.m_iID == me)
				{
					// Insert an additional row for previous score
					Widget w = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
					m_aPlayerWidgets.Insert(w);
				}
				Widget w = GetGame().GetWorkspace().CreateWidgets("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", m_wTable);
				m_aPlayerWidgets.Insert(w);
			}
		}
		
		if (m_ActiveCourse.m_iPlayer > 0)
		{
			string name = SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(m_ActiveCourse.m_iPlayer);
			WorldTimestamp timestamp = GetGame().GetWorld().GetTimestamp();
			int time = 0;
			if (m_ActiveCourse.m_StartTimestamp)
				time = timestamp.DiffMilliseconds(m_ActiveCourse.m_StartTimestamp);
			UpdateRow(m_wActivePlayerWidget, name, time, m_ActiveCourse.m_iModifiers);
		}
		else
		{
			UpdateRow(m_wActivePlayerWidget, "<inactive>", 0, 0);
		}
		
		UpdateRow(m_wGoldWidget, "Gold", 0, m_ActiveCourse.m_Config.m_iGold);
		UpdateRow(m_wSilverWidget, "Silver", 0, m_ActiveCourse.m_Config.m_iSilver);
		UpdateRow(m_wBronzeWidget, "Bronze", 0, m_ActiveCourse.m_Config.m_iBronze);
		
		for (int i = 0; i < m_ActiveCourse.m_aAllPlayersInfo.Count(); i++)
		{
			Widget w = m_aPlayerWidgets[i];
			RKN_TimeTrialScoreInfo info = m_ActiveCourse.m_aAllPlayersInfo[i];
			
			if (info.m_iID == me)
			{
				UpdateRow(w, info.GetName() + " (best)", info.m_iBestTime, info.m_iBestTotal);
				UpdateRow(m_aPlayerWidgets[++i], info.GetName() + " (prev)", info.m_iPrevTime, info.m_iPrevTotal);
			}
			else
			{
				UpdateRow(w, info.GetName(), info.m_iBestTime, info.m_iBestTotal);
			}
		}
	}
	
	void UpdateRow(Widget w, string name, int time, int total)
	{
		TextWidget nameWidget = TextWidget.Cast(w.FindAnyWidget("Name"));
		if (nameWidget)
			nameWidget.SetText(name);

		TextWidget timeWidget = TextWidget.Cast(w.FindAnyWidget("Time"));
		if (timeWidget)
			timeWidget.SetText(FormatTime(time));


		TextWidget totalWidget = TextWidget.Cast(w.FindAnyWidget("Total"));
		if (totalWidget)
			totalWidget.SetText(FormatTime(total));
	}
	
	string FormatTime(float time)
	{
		return (time / 1000.0).ToString(lenDec: 2) + " s";
	}
}