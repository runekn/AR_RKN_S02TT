[BaseContainerProps(configRoot: true)]
class RKN_TimeTrialCourseConfig
{
	[Attribute()]
	LocalizedString m_sName;
	[Attribute()]
	ref array<ref RKN_TimeTrialLoadoutItem> m_aCompetitiveLoadout;
	[Attribute()]
	int m_iBronze;
	[Attribute()]
	int m_iSilver;
	[Attribute()]
	int m_iGold;
}

[BaseContainerProps()]
class RKN_TimeTrialLoadoutItem
{
	[Attribute(params: "et")]
	ResourceName m_sPrefab;
	
	[Attribute("1")]
	int m_iCount;
}