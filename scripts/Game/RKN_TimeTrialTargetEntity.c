class RKN_TimeTrialTargetEntityClass : BaseBuildingClass
{
}

// Shamelessly stole most from SCR_FiringRangeTarget, just without m_FiringRangeManager
class RKN_TimeTrialTargetEntity : BaseBuilding
{
	
	//! Current state at any given time
	protected int m_iDesiredState;
	
	// Signal manager to pass signals into proc anim
	private SignalsManagerComponent m_SignalManager;
	private int m_iTargetHitSignal;
	
	// Proc anim component
	private ProcAnimComponent m_ProcAnimComponent;
		
	// RPL component
	private RplComponent m_RplComponent;
	
	// Variable preventing multiple hit evaluation
	protected bool m_bTargetHit = false;
	
	// value of hit (score)
	private int m_iHitValue; 
	
	// state of the target for JIP 
	private int m_iTargetState;
	
	ref ScriptInvoker Event_TargetChangeState = new ScriptInvoker();
	
	ref ScriptInvoker Event_TargetHit = new ScriptInvoker();
	
	//------------------------------------------------------------------------------------------------
	void RKN_TimeTrialStandardTarget(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);	
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_SignalManager = SignalsManagerComponent.Cast(FindComponent(SignalsManagerComponent));
		m_ProcAnimComponent = ProcAnimComponent.Cast(FindComponent(ProcAnimComponent));
				
		if (!m_SignalManager)
			return;
		
		// Get the signal index
		m_iTargetHitSignal = m_SignalManager.FindSignal("target_hit");
		
		m_RplComponent = RplComponent.Cast(FindComponent(RplComponent));
	}
	
	override void OnDamage(float damage,
				  EDamageType type,
				  IEntity pHitEntity, 
				  inout vector outMat[3], // Hit position/direction/normal
				  IEntity damageSource, 
				  notnull Instigator instigator, 
				  int colliderID, 
				  float speed)
	{

		if (IsProxy())
			return;
		
		// don't continue if projectile hit the stand of the target. 
		if (!IsHitValid(CoordToLocal(outMat[0])))
			return;
		
		super.OnDamage(damage, type, pHitEntity, outMat, damageSource, instigator, colliderID, speed);
		
		// check if target is in erected state
		if (GetState() != ETargetState.TARGET_UP)
 			return;

		// target was already hit. Multiple execution check
		if (m_bTargetHit)
			return;
		m_bTargetHit = true;

		SetState(ETargetState.TARGET_DOWN);
		
		// activate entity EOnFrame
		//SetEventMask(EntityEvent.FRAME);
		
		// get lobby
		// get ID of the player who hits the target	
		int playerID = instigator.GetInstigatorPlayerID();
		
		// only if owner of the target is the player, count score
		string hitKey = GetHitKey(CoordToLocal(outMat[0]));
		Event_TargetHit.Invoke(hitKey, playerID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return if is proxy or not
	bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check what part of the target projectile hit.
	bool IsHitValid(vector coordOfHit);
	
	//------------------------------------------------------------------------------------------------
	//! Return states of target <0, 1> standing or lies
	int GetState()
	{
		if (!m_SignalManager)
			return 0;
		return m_SignalManager.GetSignalValue(m_SignalManager.FindSignal("target_hit"));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets desired animation state
	void SetState(int state)
	{
		Rpc(SetStateMP, state);
		SetStateMP(state);
		//Replication.BumpMe();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void SetStateMP(int state)
	{
		m_iDesiredState = state;
		Event_TargetChangeState.Invoke(state, this);
		if (m_iDesiredState == ETargetState.TARGET_DOWN)
		{
			// Animate target
			if (m_SignalManager)
				m_SignalManager.SetSignalValue(m_iTargetHitSignal, ETargetState.TARGET_DOWN);
			// Play sound
			SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
			if (soundManagerEntity)
			{
				soundManagerEntity.CreateAndPlayAudioSource(this, SCR_SoundEvent.SOUND_TARGET_DOWN); 
			}
			//Replication.BumpMe();
		}
		else
		{
			m_bTargetHit = false;
			// Animate target
			if (m_SignalManager)
				m_SignalManager.SetSignalValue(m_iTargetHitSignal, ETargetState.TARGET_UP);
			// Play sound
			SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
			if (soundManagerEntity)
			{
				soundManagerEntity.CreateAndPlayAudioSource(this, SCR_SoundEvent.SOUND_TARGET_UP); 
			}			
			//Replication.BumpMe();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Evaluate the hit coordinates to a key
	string GetHitKey(vector coordOfHit);
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
    {
        // Save targets state.
		m_iTargetState = GetState();
        writer.Write(m_iTargetState, 1);
        return true;
    }
     
	//------------------------------------------------------------------------------------------------
    override bool RplLoad(ScriptBitReader reader)
    {
		// Load targets state.
        if (!reader.Read(m_iTargetState, 1)) return false;
		SetState(m_iTargetState);
        return true;
    }
}