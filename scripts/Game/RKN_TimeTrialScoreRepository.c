class RKN_TimeTrialCourseManagerComponentClass : SCR_BaseGameModeComponentClass
{
}

class RKN_TimeTrialCourseManagerComponent : SCR_BaseGameModeComponent
{
	[Attribute()]
	ref array<ref RKN_TimeTrialCourseConfig> m_aCourseConfigs;

	ref map<string, int> m_mDataIndices = new map<string, int>;
	ref array<RKN_TimeTrialCourseLayer> m_aCourses = {};
	[RplProp()]
	ref array<ref RKN_TimeTrialScoreInfo> m_aCurrentScores = {};
	[RplProp()]
	ref array<ref array<ref RKN_TimeTrialScoreInfo>> m_aScoreInfoHistory = {};
	
	void RKN_TimeTrialCourseManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		SetEventMask(GetOwner(), EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		for (int i = 0; i < m_aCourseConfigs.Count(); i++)
		{
			RKN_TimeTrialCourseConfig config = m_aCourseConfigs[i];
			m_mDataIndices.Set(config.m_sId, i);
			m_aCurrentScores.Insert(RKN_TimeTrialScoreInfo.empty());
			m_aScoreInfoHistory.Insert({});
			m_aCourses.Insert(null);
		}
	}
	
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		StopCourseForPlayer(playerId);
	}
	
	override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		StopCourseForPlayer(instigatorContextData.GetVictimPlayerID());
	}
	
	int RegisterCourse(RKN_TimeTrialCourseLayer course)
	{
		int i = m_mDataIndices.Get(course.m_sCourseId);
		m_aCourses[i] = course;
		return i;
	}
	
	void LoadSavedHistory(string courseId, array<ref RKN_TimeTrialScoreInfo> history)
	{
		int i = m_mDataIndices.Get(courseId);
		m_aScoreInfoHistory[i] = history;
	}
	
	RKN_TimeTrialCourseData GetData(int i)
	{
		RKN_TimeTrialCourseData data = new RKN_TimeTrialCourseData();
		data.m_CurrentScoreInfo = m_aCurrentScores[i];
		data.m_aScoreInfoHistory = m_aScoreInfoHistory[i];
		data.m_Config = m_aCourseConfigs[i];
		return data;
	}
	
	RKN_TimeTrialCourseData CreateActiveScore(int i, int playerId, RKN_TimeTrialScoreType type)
	{		
		RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
		info.m_eType = type;
		info.m_iID = playerId;
		m_aCurrentScores[i] = info;
		
		RKN_TimeTrialCourseData data = new RKN_TimeTrialCourseData();
		data.m_CurrentScoreInfo = info;
		data.m_aScoreInfoHistory = m_aScoreInfoHistory[i];
		data.m_Config = m_aCourseConfigs[i];
		
		Replication.BumpMe();
		return data;
	}
	
	void ClearCurrentScore(int i)
	{
		m_aCurrentScores[i] = RKN_TimeTrialScoreInfo.empty();
		Replication.BumpMe();
	}
	
	bool HasActiveCompetitor(int i)
	{
		return !m_aCurrentScores[i].IsEmpty();
	}
	
	bool IsActiveCompetitor(int playerId)
	{
		foreach (RKN_TimeTrialScoreInfo info : m_aCurrentScores)
			if (info && info.m_iID == playerId)
				return true;
		return false;
	}
	
	void ApplyPenalty(int i, int amount)
	{
		RKN_TimeTrialScoreInfo info = m_aCurrentScores[i];
		if (!info)
			return;
		info.m_iPenalty += amount;
		Replication.BumpMe();
	}
	
	void ApplyBonus(int i, int amount)
	{
		RKN_TimeTrialScoreInfo info = m_aCurrentScores[i];
		if (!info)
			return;
		info.m_iBonus += amount;
		Replication.BumpMe();
	}
	
	void StartTime(int i)
	{
		m_aCurrentScores[i].m_iStart = GetGame().GetWorld().GetTimestamp();
		Replication.BumpMe();
	}
	
	void TargetsRemaining(int i, int remaining)
	{
		m_aCurrentScores[i].m_iSectionTargetsRemaining = remaining;
		Replication.BumpMe();
	}
	
	void StopTime(int i, bool cancel)
	{
		RKN_TimeTrialScoreInfo info = m_aCurrentScores[i];
		info.m_iTime = info.GetTime();
		if (!cancel && info.m_eType == RKN_TimeTrialScoreType.COMPETITIVE)
			SubmitScore(info, i);
		Replication.BumpMe();
	}
	
	private void SubmitScore(RKN_TimeTrialScoreInfo info, int j)
	{
		array<ref RKN_TimeTrialScoreInfo> history = m_aScoreInfoHistory[j];
		bool newBest = true;
		int prevIndex = -1;
		int bestIndex = -1;
		for (int i = 0; i < history.Count(); i++)
		{
			RKN_TimeTrialScoreInfo historicalInfo = history[i];
			if (historicalInfo.m_iID == info.m_iID)
			{
				if (historicalInfo.m_eType == RKN_TimeTrialScoreType.PREVIOUS)
				{
					prevIndex = i;
				}
				else
				{
					if (historicalInfo.GetTotal() > info.GetTotal())
						bestIndex = i;
					else
						newBest = false;
				}
			}
			
			// Prepare for sorting
			if (historicalInfo.m_iTotal == 0)
				historicalInfo.m_iTotal = historicalInfo.GetTotal();
		}
		
		if (prevIndex != -1)
			history[prevIndex] = info.CopyAs(RKN_TimeTrialScoreType.PREVIOUS);
		else
			history.Insert(info.CopyAs(RKN_TimeTrialScoreType.PREVIOUS));
			
		if (newBest)
		{
			if (bestIndex != -1)
				history[bestIndex] = info.CopyAs(RKN_TimeTrialScoreType.BEST);
			else
				history.Insert(info.CopyAs(RKN_TimeTrialScoreType.BEST));
		}
		
		history.Sort(true);
	}
	
	private void StopCourseForPlayer(int playerId)
	{
		if (!Replication.IsServer())
			return;
		
		for (int i = 0; i < m_aCurrentScores.Count(); i++)
		{
			RKN_TimeTrialScoreInfo info = m_aCurrentScores[i];
			if (info && info.m_iID == playerId)
			{
				m_aCourses[i].CancelCourse();
				return;
			}
		}
	}
}

class RKN_TimeTrialCourseData
{
	ref RKN_TimeTrialScoreInfo m_CurrentScoreInfo;
	ref array<ref RKN_TimeTrialScoreInfo> m_aScoreInfoHistory;
	ref RKN_TimeTrialCourseConfig m_Config;
}