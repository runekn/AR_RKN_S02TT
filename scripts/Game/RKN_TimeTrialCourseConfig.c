[BaseContainerProps(configRoot: true)]
class RKN_TimeTrialCourseConfig
{
	[Attribute()]
	string m_sId;
	[Attribute()]
	LocalizedString m_sName;
	[Attribute()]
	LocalizedString m_sDescription;
	[Attribute()]
	ref array<ref RKN_TimeTrialLoadoutItem> m_aCompetitiveLoadout;
	[Attribute()]
	float m_fBronzeSeconds;
	[Attribute()]
	float m_fSilverSeconds;
	[Attribute()]
	float m_fGoldSeconds;
	
	int GetGoldMilliseconds()
	{
		return m_fGoldSeconds * 1000;
	}
	
	int GetSilverMilliseconds()
	{
		return m_fSilverSeconds * 1000;
	}
	
	int GetBronzeMilliseconds()
	{
		return m_fBronzeSeconds * 1000;
	}
}

[BaseContainerProps()]
class RKN_TimeTrialLoadoutItem
{
	[Attribute(params: "et")]
	ResourceName m_sPrefab;
	
	[Attribute("1")]
	int m_iCount;
	
	[Attribute("false")]
	bool m_bEquip;
}