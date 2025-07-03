//------------------------------------------------------------------------------------------------
//! Holds scoring data of players.
class RKN_TimeTrialScoreInfo
{
	int m_iID = -1;
	int m_iPrevTime = 0;
	int m_iPrevTotal = 0;
	int m_iBestTime = 0;
	int m_iBestTotal = 0;
	
	//################################################################################################
	//! Codec methods
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, 20);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, 20);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) 
	{	
		return lhs.CompareSnapshots(rhs, 20);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(RKN_TimeTrialScoreInfo prop, SSnapSerializerBase snapshot, ScriptCtx ctx) 
	{
		return snapshot.Compare(prop.m_iID, 4) 
			&& snapshot.Compare(prop.m_iPrevTime, 4)
			&& snapshot.Compare(prop.m_iPrevTotal, 4)
			&& snapshot.Compare(prop.m_iBestTime, 4)
			&& snapshot.Compare(prop.m_iBestTotal, 4);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(RKN_TimeTrialScoreInfo prop, ScriptCtx ctx, SSnapSerializerBase snapshot) 
	{		
		snapshot.SerializeBytes(prop.m_iID, 4);
		snapshot.SerializeBytes(prop.m_iPrevTime, 4);
		snapshot.SerializeBytes(prop.m_iPrevTotal, 4);
		snapshot.SerializeBytes(prop.m_iBestTime, 4);
		snapshot.SerializeBytes(prop.m_iBestTotal, 4);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, RKN_TimeTrialScoreInfo prop) 
	{
		snapshot.SerializeBytes(prop.m_iID, 4);
		snapshot.SerializeBytes(prop.m_iPrevTime, 4);
		snapshot.SerializeBytes(prop.m_iPrevTotal, 4);
		snapshot.SerializeBytes(prop.m_iBestTime, 4);
		snapshot.SerializeBytes(prop.m_iBestTotal, 4);
		
		return true;
	}
	
	//################################################################################################
	//------------------------------------------------------------------------------------------------
	//! Dynamically returns the player name.
	string GetName()
	{
		return SCR_PlayerNamesFilterCache.GetInstance().GetPlayerDisplayName(m_iID);
	}
};
