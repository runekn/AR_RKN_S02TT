class RKN_TimeTrialScoreTablePlayerComponentClass : ScriptComponentClass
{
}

class RKN_TimeTrialScoreTablePlayerComponent : ScriptComponent
{
	[Attribute("{3440F82E4043655C}UI/layouts/HUD/TimeTrial/TimeTrialScoreTable.layout", params: ".layout")]
	ResourceName m_sTableWidget;
	[Attribute("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", params: ".layout")]
	ResourceName m_sRowWidget;
	[Attribute("5")]
	int m_iHistoryMaxOtherPlayers;
		
	int m_iActiveCourse = -1;
	RplComponent m_RplComponent;
	Widget m_wRoot;
	
	Widget m_wCourseLayout;
	TextWidget m_wCourseNameWidget;
	TextWidget m_wGoldTimeWidget;
	TextWidget m_wSilverTimeWidget;
	TextWidget m_wBronzeTimeWidget;
	
	int m_iGoldColor;
	int m_iSilverColor;
	int m_iBronzeColor;
	
	Widget m_wPlayerLayout;
	TextWidget m_wPlayerNameWidget;
	ImageWidget m_wPlayerTrophyWidget;
	TextWidget m_wPlayerTimeWidget;
	TextWidget m_wPlayerPenaltyWidget;
	TextWidget m_wPlayerBonusWidget;
	TextWidget m_wPlayerTargetsRemainingWidget;
	
	Widget m_wTableWidget;
	ref array<Widget> m_aPlayerRowWidgets = {};
	
	bool m_bOverride;
	
	void ~RKN_TimeTrialScoreTablePlayerComponent()
	{
		if (m_wRoot)
			m_wRoot.RemoveFromHierarchy();
	}
	
	void ShowScoreTable(int courseId, bool overrideTable)
	{
		Rpc(RpcDo_ShowScoreTable, courseId, overrideTable);
	}
	
	void RemoveScoreTable(int courseId, bool overrideTable)
	{
		Rpc(RpcDo_RemoveScoreTable, courseId, overrideTable);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_ShowScoreTable(int courseId, bool overrideTable)
	{
		if (m_bOverride)
			return;
		if (m_iActiveCourse != courseId && m_wRoot)
		{
			m_wRoot.RemoveFromHierarchy();
			m_wRoot = null;
			m_aPlayerRowWidgets.Clear();
		}
		m_iActiveCourse = courseId;
		m_bOverride = overrideTable;
		SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_RemoveScoreTable(int courseId, bool overrideTable)
	{
		if (m_bOverride && !overrideTable)
			return;
		
		m_wRoot.RemoveFromHierarchy();
		m_wRoot = null;
		m_iActiveCourse = -1;
		m_aPlayerRowWidgets.Clear();
		m_bOverride = false;
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_RplComponent)
			m_RplComponent = BaseGameEntity.Cast(GetOwner()).GetRplComponent();
		if (m_iActiveCourse > -1 && m_RplComponent.IsOwner())
		{
			UpdateTable();
		}
	}
	
	void UpdateTable()
	{
		int me = GetGame().GetPlayerController().GetPlayerId();	
		RKN_TimeTrialCourseData data = RKN_TimeTrialUtils.GetCourseManager().GetData(m_iActiveCourse);
		
		if (!m_wRoot)
		{
			m_wRoot = GetGame().GetWorkspace().CreateWidgets(m_sTableWidget);	
			if (!m_wRoot)
				return;
			
			m_wCourseLayout = m_wRoot.FindAnyWidget("CourseInfoLayout");
			m_wCourseNameWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("CourseNameText"));
			m_wCourseNameWidget.SetText(data.m_Config.m_sName);
			m_wGoldTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("GoldTimeText"));
			m_wGoldTimeWidget.SetText(FormatTime(data.m_Config.GetGoldMilliseconds()));
			m_wSilverTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("SilverTimeText"));
			m_wSilverTimeWidget.SetText(FormatTime(data.m_Config.GetSilverMilliseconds()));
			m_wBronzeTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("BronzeTimeText"));
			m_wBronzeTimeWidget.SetText(FormatTime(data.m_Config.GetBronzeMilliseconds()));
			
			m_iBronzeColor = ImageWidget.Cast(m_wCourseLayout.FindAnyWidget("BronzeImage")).GetColorInt();
			m_iSilverColor = ImageWidget.Cast(m_wCourseLayout.FindAnyWidget("SilverImage")).GetColorInt();
			m_iGoldColor = ImageWidget.Cast(m_wCourseLayout.FindAnyWidget("GoldImage")).GetColorInt();
			
			m_wPlayerLayout = m_wRoot.FindAnyWidget("RunInfoLayout");
			m_wPlayerNameWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("PlayerNameText"));
			m_wPlayerTrophyWidget = ImageWidget.Cast(m_wPlayerLayout.FindAnyWidget("TrophyImage"));
			m_wPlayerTimeWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("TimeText"));
			m_wPlayerPenaltyWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("PenaltyText"));
			m_wPlayerBonusWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("BonusText"));
			m_wPlayerTargetsRemainingWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("TargetsRemainingText"));
			
			m_wTableWidget = m_wRoot.FindAnyWidget("Table");
		}
		
		// Current attempt info
		if (!data.m_CurrentScoreInfo.IsEmpty())
		{	
			m_wPlayerLayout.SetVisible(true);
			string name = data.m_CurrentScoreInfo.GetName();
			if (data.m_CurrentScoreInfo.m_eType == RKN_TimeTrialScoreType.TRAINING)
				name += " (Training)";
			m_wPlayerNameWidget.SetText(name);
			m_wPlayerTimeWidget.SetText(FormatTime(data.m_CurrentScoreInfo.GetTime()));
			m_wPlayerPenaltyWidget.SetText(MillisToSeconds(data.m_CurrentScoreInfo.m_iPenalty).ToString(lenDec: 2));
			m_wPlayerBonusWidget.SetText(MillisToSeconds(data.m_CurrentScoreInfo.m_iBonus).ToString(lenDec: 2));
			m_wPlayerTrophyWidget.SetColorInt(GetTrophyColor(data));
			m_wPlayerTargetsRemainingWidget.SetText(data.m_CurrentScoreInfo.m_iSectionTargetsRemaining.ToString());
		}
		else
		{
			m_wPlayerLayout.SetVisible(false);
		}
		
		
		// History table
		if ((!data.m_CurrentScoreInfo.IsEmpty() && data.m_CurrentScoreInfo.m_iID == me && data.m_CurrentScoreInfo.m_iTime == 0) || data.m_aScoreInfoHistory.IsEmpty())
		{
			m_wTableWidget.SetVisible(false);
		}
		else
		{
			m_wTableWidget.SetVisible(true);
			if (m_aPlayerRowWidgets.Count() != data.m_aScoreInfoHistory.Count())
			{
				// Delete and generate new rows for current players
				foreach (Widget w : m_aPlayerRowWidgets)
				{
					w.RemoveFromHierarchy();
				}
				m_aPlayerRowWidgets.Clear();
				
				foreach (RKN_TimeTrialScoreInfo info : data.m_aScoreInfoHistory)
				{
					Widget w = GetGame().GetWorkspace().CreateWidgets(m_sRowWidget, m_wTableWidget);
					m_aPlayerRowWidgets.Insert(w);
				}
			}
			
			int otherPlayersHistory;
			for (int i = 0; i < data.m_aScoreInfoHistory.Count(); i++)
			{
				Widget w = m_aPlayerRowWidgets[i];
				RKN_TimeTrialScoreInfo info = data.m_aScoreInfoHistory[i];
				
				if (info.m_iID == me)
				{
					if (info.m_eType == RKN_TimeTrialScoreType.BEST)
						UpdateRow(w, "Personal best", info.GetTime(), info.GetTotal());
					else
						UpdateRow(w, "Personal previous", info.GetTime(), info.GetTotal());
				}
				else if (info.m_eType == RKN_TimeTrialScoreType.BEST && otherPlayersHistory++ < m_iHistoryMaxOtherPlayers)
				{
					UpdateRow(w, info.GetName(), info.GetTime(), info.GetTotal());
				}
				else
				{
					w.SetVisible(false);
				}
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
		float seconds = MillisToSeconds(time);
		int minutes = seconds / 60;
		return minutes.ToString(2) + ":" + Math.Repeat(seconds, 60).ToString(5, 2);
	}
	
	float MillisToSeconds(float time)
	{
		return time / 1000.0;
	}
	
	int GetTrophyColor(RKN_TimeTrialCourseData data)
	{
		int totalTime = data.m_CurrentScoreInfo.GetTotal();
		if (totalTime < data.m_Config.GetGoldMilliseconds())
			return m_iGoldColor;
		if (totalTime < data.m_Config.GetSilverMilliseconds())
			return m_iSilverColor;
		if (totalTime < data.m_Config.GetBronzeMilliseconds())
			return m_iBronzeColor;
		else
			return Color.BLACK;
	}
}