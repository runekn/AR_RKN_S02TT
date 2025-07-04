class RKN_TimeTrialControlPanelEntityClass : GenericEntityClass
{
}

class RKN_TimeTrialControlPanelEntity : GenericEntity
{	
	const static int COUNTDOWN_TIME = 3500;
	const static int RESET_BUTTON_TIME = 500;
	const static float BUTTON_PRESSED_STATE = -0.005;
	const static int BUTTON_DEFAULT_STATE = 0;
	
	private int m_iPushButtonStart;
	private SignalsManagerComponent m_SignalManager;
	
	override void EOnInit(IEntity owner)
	{
		m_SignalManager = SignalsManagerComponent.Cast(FindComponent(SignalsManagerComponent));
	}
	
	void StartCountdown(IEntity pUserEntity, RKN_TimeTrialCourseLayer course)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		RplId courseId = Replication.FindId(course);
		Rpc(RpcAsk_StartCountdown, playerId, courseId);
		AnimateButton();
	}
	
	void CancelCourse(IEntity pUserEntity, RKN_TimeTrialCourseLayer course)
	{
		RplId courseId = Replication.FindId(course);
		Rpc(RpcAsk_CancelCourse, courseId);
		AnimateButton();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_CancelCourse(RplId courseId)
	{
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(Replication.FindItem(courseId));
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (soundManagerEntity)
			soundManagerEntity.CreateAndPlayAudioSource(this, SCR_SoundEvent.SOUND_RANGECP_STARTBUTTON);
		course.CancelCourse();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcAsk_StartCountdown(int playerId, RplId courseId)
	{
		RKN_TimeTrialCourseLayer course = RKN_TimeTrialCourseLayer.Cast(Replication.FindItem(courseId));
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (soundManagerEntity)
		{
			soundManagerEntity.CreateAndPlayAudioSource(this, SCR_SoundEvent.SOUND_RANGECP_STARTBUTTON);
			soundManagerEntity.CreateAndPlayAudioSource(this, SCR_SoundEvent.SOUND_RANGECP_ROUNDSTART);
		}
		course.ScheduleCourse(playerId, COUNTDOWN_TIME);
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
}