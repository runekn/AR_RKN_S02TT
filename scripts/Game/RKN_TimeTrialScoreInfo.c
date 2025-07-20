//------------------------------------------------------------------------------------------------
//! Holds scoring data of players.
class RKN_TimeTrialScoreInfo
{
	RKN_TimeTrialScoreType m_eType
	int m_iID = -1;
	WorldTimestamp m_iStart;
	int m_iTime;
	int m_iPenalty;
	int m_iBonus;
	int m_iSectionTargetsRemaining;
	
	[SortAttribute()]
	int m_iTotal; // Only used for sorting
	
	static RKN_TimeTrialScoreInfo empty()
	{
		RKN_TimeTrialScoreInfo info = new RKN_TimeTrialScoreInfo();
		info.m_eType = RKN_TimeTrialScoreType.NONE;
		return info;
	}
	
	//################################################################################################
	//! Codec methods
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, 32);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, 32);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) 
	{	
		return lhs.CompareSnapshots(rhs, 32);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(RKN_TimeTrialScoreInfo prop, SSnapSerializerBase snapshot, ScriptCtx ctx) 
	{
		if (!prop)
			return false;
		return snapshot.CompareInt(prop.m_eType)
			&& snapshot.CompareInt(prop.m_iID)
			&& snapshot.Compare(prop.m_iStart, 8)
			&& snapshot.CompareInt(prop.m_iTime)
			&& snapshot.CompareInt(prop.m_iPenalty)
			&& snapshot.CompareInt(prop.m_iBonus)
			&& snapshot.CompareInt(prop.m_iSectionTargetsRemaining);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(RKN_TimeTrialScoreInfo prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		if (!prop)
		{
			return true;
		}
		snapshot.SerializeInt(prop.m_eType);
		snapshot.SerializeInt(prop.m_iID);
		snapshot.SerializeBytes(prop.m_iStart, 8);
		snapshot.SerializeInt(prop.m_iTime);
		snapshot.SerializeInt(prop.m_iPenalty);
		snapshot.SerializeInt(prop.m_iBonus);
		snapshot.SerializeInt(prop.m_iSectionTargetsRemaining);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, RKN_TimeTrialScoreInfo prop) 
	{
		snapshot.SerializeInt(prop.m_eType);
		snapshot.SerializeInt(prop.m_iID);
		snapshot.SerializeBytes(prop.m_iStart, 8);
		snapshot.SerializeInt(prop.m_iTime);
		snapshot.SerializeInt(prop.m_iPenalty);
		snapshot.SerializeInt(prop.m_iBonus);
		snapshot.SerializeInt(prop.m_iSectionTargetsRemaining);
		
		return true;
	}
	
	//################################################################################################
	//------------------------------------------------------------------------------------------------
	//! Dynamically returns the player name.
	string GetName()
	{
		return SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(m_iID);
	}
	
	IEntity GetPlayer()
	{
		return GetGame().GetPlayerManager().GetPlayerControlledEntity(m_iID);
	}
	
	int GetTime()
	{
		if (!m_iStart)
			return 0;
		else if (m_iTime > 0)
			return m_iTime;
		
		ChimeraWorld world = GetGame().GetWorld();
		return world.GetServerTimestamp().DiffMilliseconds(m_iStart);
	}
	
	int GetTotal()
	{
		return GetTime() + m_iPenalty - m_iBonus;
	}

	bool IsEmpty()
	{
		return m_eType == RKN_TimeTrialScoreType.NONE;
	}
	
	RKN_TimeTrialScoreInfo CopyAs(RKN_TimeTrialScoreType type)
	{
		RKN_TimeTrialScoreInfo c = new RKN_TimeTrialScoreInfo();
		c.m_eType = type;
		c.m_iID = m_iID;
		c.m_iStart = m_iStart;
		c.m_iTime = m_iTime;
		c.m_iPenalty = m_iPenalty;
		c.m_iBonus = m_iBonus;
		return c;
	}
};

enum RKN_TimeTrialScoreType
{
	NONE, // When current score is not set
	PREVIOUS, BEST, // For attempt history
	TRAINING, COMPETITIVE // For active attempt
}