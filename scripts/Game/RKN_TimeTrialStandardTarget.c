[EntityEditorProps(category: "GameScripted/FiringRange", description: "Handeles behaviour of target. Evaluates a hit.", color: "0 0 255 255")]
class RKN_TimeTrialStandardTargetClass : BaseBuildingClass
{
}

// Shamelessly stole most from SCR_FiringRangeTarget, just without m_FiringRangeManager
class RKN_TimeTrialStandardTarget : BaseBuilding
{
	// target scoring points 
	// TODO: move to slot. entity should just report location
	const float SCORE_CENTER   = 1500;
	const float SCORE_CIRCLE_1 =  1000;
	const float SCORE_CIRCLE_2 =  500;
	const float SCORE_CIRCLE_3 =  0;
	
	// definition of areas on target for scoring
	// 10 points area
	const float AREA_10_W_LEFT = 	0.03;	
	const float AREA_10_W_RIGHT =  -0.03;
	const float AREA_10_H_UP = 		1.15;
	const float AREA_10_H_DOWN = 	1.03;
		
	// 5 points area
	const float AREA_5_W_LEFT = 	0.08;	
	const float AREA_5_W_RIGHT =   -0.08;
	const float AREA_5_H_UP = 		1.2;
	const float AREA_5_H_DOWN = 	0.98;
	
	// 3 points area
	const float AREA_3_W_LEFT = 	0.13;	
	const float AREA_3_W_RIGHT =   -0.13;
	const float AREA_3_H_UP = 		1.32;
	const float AREA_3_H_DOWN = 	0.84;
	
	// 1 point area
	const float AREA_1_W_LEFT = 	0.28;	
	const float AREA_1_W_RIGHT =   -0.28;
	const float AREA_1_H_UP = 		1.77;
	const float AREA_1_H_DOWN = 	0.5;
	
	const float AREA_BOTTOM_VALID = 0.40;
	
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
		float score = GetHitValue(CoordToLocal(outMat[0]));
		Event_TargetHit.Invoke(score, playerID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return if is proxy or not
	bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check what part of the target projectile hit.
	bool IsHitValid(vector coordOfHit)
	{
		float HitX = coordOfHit[0];
		float HitY = coordOfHit[1];
		
		if (HitX < AREA_1_W_LEFT && HitX > AREA_1_W_RIGHT && HitY < AREA_1_H_UP && HitY > AREA_BOTTOM_VALID)
			return true;
		
		return false;
	}
	
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
	//! Evaluate the hit coordinates and add a propper score
	float GetHitValue(vector coordOfHit)
	{				
		float HitX = coordOfHit[0];
		float HitY = coordOfHit[1];
				
		if (HitX < AREA_10_W_LEFT && HitX > AREA_10_W_RIGHT && HitY < AREA_10_H_UP && HitY > AREA_10_H_DOWN)
			return SCORE_CENTER;
		
		else if (HitX < AREA_5_W_LEFT && HitX > AREA_5_W_RIGHT && HitY < AREA_5_H_UP && HitY > AREA_5_H_DOWN)
			return SCORE_CIRCLE_1;
		
		else if (HitX < AREA_3_W_LEFT && HitX > AREA_3_W_RIGHT && HitY < AREA_3_H_UP && HitY > AREA_3_H_DOWN)
			return SCORE_CIRCLE_2;
		
		else if (HitX < AREA_1_W_LEFT && HitX > AREA_1_W_RIGHT && HitY < AREA_1_H_UP && HitY > AREA_1_H_DOWN)
			return SCORE_CIRCLE_3;
		
		return 0;
	}
	
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