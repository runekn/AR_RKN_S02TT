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
		return snapshot.Compare(prop.m_iID, 4) 
			&& snapshot.Compare(prop.m_eType, 4)
			&& snapshot.Compare(prop.m_iStart, 8)
			&& snapshot.Compare(prop.m_iTime, 4)
			&& snapshot.Compare(prop.m_iPenalty, 4)
			&& snapshot.Compare(prop.m_iBonus, 4)
			&& snapshot.Compare(prop.m_iSectionTargetsRemaining, 4);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(RKN_TimeTrialScoreInfo prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{		
		snapshot.SerializeBytes(prop.m_eType, 4);
		snapshot.SerializeBytes(prop.m_iID, 4);
		snapshot.SerializeBytes(prop.m_iStart, 8);
		snapshot.SerializeBytes(prop.m_iTime, 4);
		snapshot.SerializeBytes(prop.m_iPenalty, 4);
		snapshot.SerializeBytes(prop.m_iBonus, 4);
		snapshot.SerializeBytes(prop.m_iSectionTargetsRemaining, 4);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, RKN_TimeTrialScoreInfo prop) 
	{
		snapshot.SerializeBytes(prop.m_eType, 4);
		snapshot.SerializeBytes(prop.m_iID, 4);
		snapshot.SerializeBytes(prop.m_iStart, 8);
		snapshot.SerializeBytes(prop.m_iTime, 4);
		snapshot.SerializeBytes(prop.m_iPenalty, 4);
		snapshot.SerializeBytes(prop.m_iBonus, 4);
		snapshot.SerializeBytes(prop.m_iSectionTargetsRemaining, 4);
		
		return true;
	}
	
	//################################################################################################
	//------------------------------------------------------------------------------------------------
	//! Dynamically returns the player name.
	string GetName()
	{
		return SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(m_iID);
	}
	
	int GetTime()
	{
		if (!m_iStart)
			return 0;
		else if (m_iTime > 0)
			return m_iTime;
		else
			return GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_iStart);
	}
	
	int GetTotal()
	{
		return GetTime() + m_iPenalty - m_iBonus;
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
	PREVIOUS, BEST, // For attempt history
	TRAINING, COMPETITIVE // For active attempt
}