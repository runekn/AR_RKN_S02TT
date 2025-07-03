[BaseContainerProps()]
class RKN_TimeTrialStruct : SCR_JsonApiStruct
{
	ref array<RKN_TimeTrialCourseStruct> m_aCourseStructs = {};
	
	void RKN_TimeTrialStruct()
	{
		RegV("m_aCourseStructs");
	}
	
	override bool Serialize()
	{
		foreach (SCR_ScenarioFrameworkArea area : SCR_ScenarioFrameworkSystem.GetInstance().GetAreas())
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
		}
		return true;
	}
	
	override bool Deserialize()
	{
		foreach (RKN_TimeTrialCourseStruct courseStruct : m_aCourseStructs)
		{
			IEntity entity = GetGame().GetWorld().FindEntityByName(courseStruct.m_sName);
			if (!entity)
				continue;
			RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
			if (!course)
				continue;
			foreach (RKN_TimeTrialPlayerInfoStruct infoStruct : courseStruct.m_aAllPlayerInfo)
			{
				RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
				info.m_iID = infoStruct.m_iID;
				info.m_iPrevTime = infoStruct.m_iPrevTime;
				info.m_iPrevTotal = infoStruct.m_iPrevTotal;
				info.m_iBestTime = infoStruct.m_iBestTime;
				info.m_iBestTotal = infoStruct.m_iBestTotal;
				course.m_aAllPlayersInfo.Insert(info)
			}
		}
		return true;
	}
}

class RKN_TimeTrialCourseStruct : SCR_JsonApiStruct
{
	string m_sName;
	ref array<ref RKN_TimeTrialPlayerInfoStruct> m_aAllPlayerInfo = {};
	
	void RKN_TimeTrialCourseStruct(RKN_TimeTrialCourseLayer course)
	{
		m_sName = course.GetName();
		foreach (RKN_TimeTrialScoreInfo info : course.m_aAllPlayersInfo)
			m_aAllPlayerInfo.Insert(new RKN_TimeTrialPlayerInfoStruct(info));
		RegV("m_sName");
		RegV("m_aAllPlayerInfo");
	}
}

class RKN_TimeTrialPlayerInfoStruct : SCR_JsonApiStruct
{
	int m_iID;
	int m_iPrevTime;
	int m_iPrevTotal;
	int m_iBestTime;
	int m_iBestTotal;
	
	void RKN_TimeTrialPlayerInfoStruct(RKN_TimeTrialScoreInfo info)
	{
		m_iID = info.m_iID;
		m_iPrevTime = info.m_iPrevTime;
		m_iPrevTotal = info.m_iPrevTotal;
		m_iBestTime = info.m_iBestTime;
		m_iBestTotal = info.m_iBestTotal;
		RegV("m_iID");
		RegV("m_iPrevTime");
		RegV("m_iPrevTotal");
		RegV("m_iBestTime");
		RegV("m_iBestTotal");
	}
}