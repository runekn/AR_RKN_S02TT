[BaseContainerProps()]
class RKN_TimeTrialStruct : SCR_JsonApiStruct
{
	ref array<ref RKN_TimeTrialCourseStruct> m_aCourseStructs = {};
	
	void RKN_TimeTrialStruct()
	{
		RegV("m_aCourseStructs");
	}
	
	override bool Serialize()
	{
		RKN_TimeTrialCourseManagerComponent manager = RKN_TimeTrialUtils.GetCourseManager();
		if (!manager)
			return false;
		foreach (string id, int index : manager.m_mDataIndices)
		{
			RKN_TimeTrialCourseData data = manager.GetData(index);
			if (data.m_aScoreInfoHistory.IsEmpty())
				continue;
			m_aCourseStructs.Insert(new RKN_TimeTrialCourseStruct(data));
		}
		return true;
	}
	
	override bool Deserialize()
	{
		RKN_TimeTrialCourseManagerComponent manager = RKN_TimeTrialUtils.GetCourseManager();
		if (!manager)
			return false;
		foreach (RKN_TimeTrialCourseStruct courseStruct : m_aCourseStructs)
		{
			array<ref RKN_TimeTrialScoreInfo> history = {};
			foreach (RKN_TimeTrialPlayerInfoStruct infoStruct : courseStruct.m_aScoreInfoHistory)
			{
				history.Insert(infoStruct.ToInfo());
			}
			manager.LoadSavedHistory(courseStruct.m_sId, history);
		}
		return true;
	}
}

class RKN_TimeTrialCourseStruct : SCR_JsonApiStruct
{
	string m_sId;
	ref array<ref RKN_TimeTrialPlayerInfoStruct> m_aScoreInfoHistory = {};
	
	void RKN_TimeTrialCourseStruct(RKN_TimeTrialCourseData data)
	{
		m_sId = data.m_Config.m_sId;
		foreach (RKN_TimeTrialScoreInfo info : data.m_aScoreInfoHistory)
			m_aScoreInfoHistory.Insert(new RKN_TimeTrialPlayerInfoStruct(info));
		RegV("m_sId");
		RegV("m_aScoreInfoHistory");
	}
}

class RKN_TimeTrialPlayerInfoStruct : SCR_JsonApiStruct
{
	RKN_TimeTrialScoreType m_eType
	int m_iID = -1;
	int m_iTime;
	int m_iPenalty;
	int m_iBonus;
	
	void RKN_TimeTrialPlayerInfoStruct(RKN_TimeTrialScoreInfo info)
	{
		m_eType = info.m_eType;
		m_iID = info.m_iID;
		m_iTime = info.m_iTime;
		m_iPenalty = info.m_iPenalty;
		m_iBonus = info.m_iBonus;
		RegV("m_eType");
		RegV("m_iID");
		RegV("m_iTime");
		RegV("m_iPenalty");
		RegV("m_iBonus");
	}
	
	RKN_TimeTrialScoreInfo ToInfo()
	{
		RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
		info.m_iID = m_iID;
		info.m_eType = m_eType;
		info.m_iTime = m_iTime;
		info.m_iPenalty = m_iPenalty;
		info.m_iBonus = m_iBonus;
		return info;
	}
}