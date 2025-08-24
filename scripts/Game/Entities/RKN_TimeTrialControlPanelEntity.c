class RKN_TimeTrialControlPanelEntityClass : GenericEntityClass
{
}

class RKN_TimeTrialControlPanelEntity : GenericEntity
{	
	const static int COUNTDOWN_TIME = 3500;
	const static int RESET_BUTTON_TIME = 500;
	const static float BUTTON_PRESSED_STATE = -0.005;
	const static int BUTTON_DEFAULT_STATE = 0;
	
	ref ScriptInvoker m_OnStart = new ScriptInvoker();
	ref ScriptInvoker m_OnCancel = new ScriptInvoker();
	[RplProp()]
	int m_iCourseId = -1;
	
	private int m_iPushButtonStart;
	private SignalsManagerComponent m_SignalManager;
	
	override void EOnInit(IEntity owner)
	{
		m_SignalManager = SignalsManagerComponent.Cast(FindComponent(SignalsManagerComponent));
	}
	
	void PlaySound(string soundEvent)
	{
		Rpc(RplDo_PlaySound, soundEvent);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RplDo_PlaySound(string soundEvent)
	{
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (soundManagerEntity)
		{
			soundManagerEntity.CreateAndPlayAudioSource(this, soundEvent);
		}
	}
	
	void StartCountdown(IEntity pUserEntity, bool competitive)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		Rpc(RpcAsk_StartCountdown, playerId, competitive);
		AnimateButton();
	}
	
	void CancelCourse(IEntity pUserEntity)
	{
		Rpc(RpcAsk_CancelCourse);
		AnimateButton();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_CancelCourse()
	{
		PlaySound(SCR_SoundEvent.SOUND_RANGECP_STARTBUTTON);
		m_OnCancel.Invoke();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_StartCountdown(int playerId, bool competitive)
	{
		PlaySound(SCR_SoundEvent.SOUND_RANGECP_STARTBUTTON);
		PlaySound(SCR_SoundEvent.SOUND_RANGECP_ROUNDSTART);
		m_OnStart.Invoke(playerId, COUNTDOWN_TIME, competitive);
	}
	
	void AnimateButton()
	{
		m_iPushButtonStart = m_SignalManager.FindSignal("StartRound");
		m_SignalManager.SetSignalValue(m_iPushButtonStart, BUTTON_PRESSED_STATE);
		GetGame().GetCallqueue().CallLater(ResetButton, RESET_BUTTON_TIME, false); 
	}
	
	void ResetButton()
	{
		m_SignalManager.SetSignalValue(m_iPushButtonStart, BUTTON_DEFAULT_STATE);
	}
	
	void SetCourseId(int courseId)
	{
		m_iCourseId = courseId;
		Replication.BumpMe();
	}
}