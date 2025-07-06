class RKN_TimeTrialScoreTablePlayerComponentClass : ScriptComponentClass
{
}

class RKN_TimeTrialScoreTablePlayerComponent : ScriptComponent
{
	[Attribute("{3440F82E4043655C}UI/layouts/HUD/TimeTrial/TimeTrialScoreTable.layout", params: ".layout")]
	ResourceName m_sTableWidget;
	[Attribute("{E775E1661BF28CA9}UI/layouts/HUD/TimeTrial/TimeTrialScoreRow.layout", params: ".layout")]
	ResourceName m_sRowWidget;
	[Attribute("0.8431 0.7295 0.1621 1", UIWidgets.ColorPicker)]
	ref Color m_cGoldColor;
	[Attribute("0.8157 0.7978 0.7978 1", UIWidgets.ColorPicker)]
	ref Color m_cSilverColor;
	[Attribute("0.8114 0.5894 0.2323 1", UIWidgets.ColorPicker)]
	ref Color m_cBronzeColor;
	[Attribute("5")]
	int m_iHistoryMaxOtherPlayers;
		
	RKN_TimeTrialCourseLayer m_ActiveCourse;
	RplComponent m_RplComponent;
	Widget m_wRoot;
	
	Widget m_wCourseLayout;
	TextWidget m_wCourseNameWidget;
	TextWidget m_wGoldTimeWidget;
	TextWidget m_wSilverTimeWidget;
	TextWidget m_wBronzeTimeWidget;
	
	Widget m_wPlayerLayout;
	TextWidget m_wPlayerNameWidget;
	ImageWidget m_wPlayerTrophyWidget;
	TextWidget m_wPlayerTimeWidget;
	TextWidget m_wPlayerPenaltyWidget;
	TextWidget m_wPlayerBonusWidget;
	
	Widget m_wTableWidget;
	ref array<Widget> m_aPlayerRowWidgets = {};
	
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
			m_aPlayerRowWidgets.Clear();
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
			m_wRoot = GetGame().GetWorkspace().CreateWidgets(m_sTableWidget);	
			if (!m_wRoot)
				return;
			
			m_wCourseLayout = m_wRoot.FindAnyWidget("CourseInfoLayout");
			m_wCourseNameWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("CourseNameText"));
			m_wCourseNameWidget.SetText(m_ActiveCourse.m_Config.m_sName);
			m_wGoldTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("GoldTimeText"));
			m_wGoldTimeWidget.SetText(FormatTime(m_ActiveCourse.m_Config.m_iGold));
			m_wSilverTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("SilverTimeText"));
			m_wSilverTimeWidget.SetText(FormatTime(m_ActiveCourse.m_Config.m_iSilver));
			m_wBronzeTimeWidget = TextWidget.Cast(m_wCourseLayout.FindAnyWidget("BronzeTimeText"));
			m_wBronzeTimeWidget.SetText(FormatTime(m_ActiveCourse.m_Config.m_iBronze));
			
			m_wPlayerLayout = m_wRoot.FindAnyWidget("RunInfoLayout");
			m_wPlayerNameWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("PlayerNameText"));
			m_wPlayerTrophyWidget = ImageWidget.Cast(m_wPlayerLayout.FindAnyWidget("TrophyImage"));
			m_wPlayerTimeWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("TimeText"));
			m_wPlayerPenaltyWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("PenaltyText"));
			m_wPlayerBonusWidget = TextWidget.Cast(m_wPlayerLayout.FindAnyWidget("BonusText"));
			
			m_wTableWidget = m_wRoot.FindAnyWidget("Table");
		}
		
		// Current attempt info
		if (m_ActiveCourse.m_CurrentScoreInfo)
		{
			m_wPlayerLayout.SetVisible(true);
			string name = m_ActiveCourse.m_CurrentScoreInfo.GetName();
			if (m_ActiveCourse.m_CurrentScoreInfo.m_eType == RKN_TimeTrialScoreType.TRAINING)
				name += " (Training)";
			m_wPlayerNameWidget.SetText(name);
			m_wPlayerTimeWidget.SetText(FormatTime(m_ActiveCourse.m_CurrentScoreInfo.GetTime()));
			m_wPlayerPenaltyWidget.SetText(MillisToSeconds(m_ActiveCourse.m_CurrentScoreInfo.m_iPenalty).ToString(lenDec: 2));
			m_wPlayerBonusWidget.SetText(MillisToSeconds(m_ActiveCourse.m_CurrentScoreInfo.m_iBonus).ToString(lenDec: 2));
			m_wPlayerTrophyWidget.SetColor(GetTrophyColor());
		}
		else
		{
			m_wPlayerLayout.SetVisible(false);
		}
		
		
		// History table
		if (m_ActiveCourse.m_CurrentScoreInfo && m_ActiveCourse.m_CurrentScoreInfo.m_iID == me)
		{
			m_wTableWidget.SetVisible(false);
		}
		else
		{
			m_wTableWidget.SetVisible(true);
			if (m_aPlayerRowWidgets.Count() != m_ActiveCourse.m_aScoreInfoHistory.Count())
			{
				// Delete and generate new rows for current players
				foreach (Widget w : m_aPlayerRowWidgets)
				{
					w.RemoveFromHierarchy();
				}
				m_aPlayerRowWidgets.Clear();
				
				foreach (RKN_TimeTrialScoreInfo info : m_ActiveCourse.m_aScoreInfoHistory)
				{
					Widget w = GetGame().GetWorkspace().CreateWidgets(m_sRowWidget, m_wTableWidget);
					m_aPlayerRowWidgets.Insert(w);
				}
			}
			
			int otherPlayersHistory;
			for (int i = 0; i < m_ActiveCourse.m_aScoreInfoHistory.Count(); i++)
			{
				Widget w = m_aPlayerRowWidgets[i];
				RKN_TimeTrialScoreInfo info = m_ActiveCourse.m_aScoreInfoHistory[i];
				
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
		return minutes.ToString(2) + ":" + seconds.ToString(5, 2);
	}
	
	float MillisToSeconds(float time)
	{
		return time / 1000.0;
	}
	
	Color GetTrophyColor()
	{
		int totalTime = m_ActiveCourse.m_CurrentScoreInfo.GetTotal();
		if (totalTime < m_ActiveCourse.m_Config.m_iGold)
			return m_cGoldColor;
		if (totalTime < m_ActiveCourse.m_Config.m_iSilver)
			return m_cSilverColor;
		if (totalTime < m_ActiveCourse.m_Config.m_iBronze)
			return m_cBronzeColor;
		else
			return Color.White;
	}
}