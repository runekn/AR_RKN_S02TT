[BaseContainerProps()]
class RKN_TimeTrialStruct : SCR_JsonApiStruct
{
	//ref array<RKN_TimeTrialCourseStruct> m_aCourseStructs = {};
	
	void RKN_TimeTrialStruct()
	{
		//RegV("m_aCourseStructs");
	}
	
	override bool Serialize()
	{
		/*foreach (SCR_ScenarioFrameworkArea area : SCR_ScenarioFrameworkSystem.GetInstance().GetAreas())
		{
			array<SCR_ScenarioFrameworkLayerBase> layers = {};
			area.GetAllLayers(layers);
			foreach (SCR_ScenarioFrameworkLayerBase layer : layers)
			{
				RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(layer.FindComponent(RKN_TimeTrialCourseLayer));
				if (!course)
					continue;
				m_aCourseStructs.Insert(new RKN_TimeTrialCourseStruct(course));
			}
		}*/
		return true;
	}
	
	override bool Deserialize()
	{
		/*foreach (RKN_TimeTrialCourseStruct courseStruct : m_aCourseStructs)
		{
			IEntity entity = GetGame().GetWorld().FindEntityByName(courseStruct.m_sName);
			if (!entity)
				continue;
			RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
			if (!course)
				continue;
			foreach (RKN_TimeTrialPlayerInfoStruct infoStruct : courseStruct.m_aScoreInfoHistory)
			{
				RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
				info.m_iID = infoStruct.m_iID;
				info.m_eType = infoStruct.m_eType;
				info.m_iStart = infoStruct.m_iStart;
				info.m_iEnd = infoStruct.m_iEnd;
				info.m_iPenalty = infoStruct.m_iPenalty;
				info.m_iBonus = infoStruct.m_iBonus;
				course.m_aScoreInfoHistory.Insert(info)
			}
		}*/
		return true;
	}
}

/*class RKN_TimeTrialCourseStruct : SCR_JsonApiStruct
{
	string m_sName;
	ref array<ref RKN_TimeTrialPlayerInfoStruct> m_aScoreInfoHistory = {};
	
	void RKN_TimeTrialCourseStruct(RKN_TimeTrialCourseLayer course)
	{
		m_sName = course.GetName();
		foreach (RKN_TimeTrialScoreInfo info : course.m_aScoreInfoHistory)
			m_aScoreInfoHistory.Insert(new RKN_TimeTrialPlayerInfoStruct(info));
		RegV("m_sName");
		RegV("m_aScoreInfoHistory");
	}
}

class RKN_TimeTrialPlayerInfoStruct : SCR_JsonApiStruct
{
	RKN_TimeTrialScoreType m_eType
	int m_iID = -1;
	WorldTimestamp m_iStart;
	WorldTimestamp m_iEnd;
	int m_iPenalty;
	int m_iBonus;
	
	void RKN_TimeTrialPlayerInfoStruct(RKN_TimeTrialScoreInfo info)
	{
		m_eType = info.m_eType;
		m_iID = info.m_iID;
		m_iStart = info.m_iStart;
		m_iEnd = info.m_iEnd;
		m_iPenalty = info.m_iPenalty;
		m_iBonus = info.m_iBonus;
		RegV("m_eType");
		RegV("m_iID");
		RegV("m_iStart");
		RegV("m_iEnd");
		RegV("m_iPenalty");
		RegV("m_iBonus");
	}
}*/