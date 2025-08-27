class RKN_TimeTrialRadioComponentClass : ScriptComponentClass
{
}

class RKN_TimeTrialRadioComponent : ScriptComponent
{	
	protected AudioHandle m_iAudioHandle = AudioHandle.Invalid;
	protected SCR_CommunicationSoundComponent m_PlayingSoundComponent;
	protected ref array<string> m_aQueue = {};
	
	void QueueSoundEvents(array<string> soundEvents)
	{
		if (!HasRadio())
			return;
		Rpc(RpcDo_QueueSoundEvents, soundEvents);
		//RpcDo_QueueSoundEvents(soundEvents);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_QueueSoundEvents(array<string> soundEvents)
	{
		if (!m_PlayingSoundComponent)
		{
			m_PlayingSoundComponent = SCR_CommunicationSoundComponent.Cast(GetOwner().FindComponent(SCR_CommunicationSoundComponent));
			if (!m_PlayingSoundComponent)
			{
				Print("Could not find sound component!", LogLevel.ERROR);
				return;
			}
		}
		
		if (m_iAudioHandle != AudioHandle.Invalid)
		{
			Stop();
		}
		m_aQueue.Clear();
		
		m_aQueue.InsertAll(soundEvents);
		SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	protected bool HasRadio()
	{
		InventorySearchPredicate predicate = new SCR_CommonItemTypeSearchPredicate(ECommonItemType.RADIO, null);
		SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_InventoryStorageManagerComponent));
		IEntity entity = inv.FindItem(predicate);
		return entity;
	}
	
	protected void Stop()
	{
		if (m_PlayingSoundComponent && m_iAudioHandle != AudioHandle.Invalid)
		{
			m_PlayingSoundComponent.Terminate(m_iAudioHandle);
		}
		
		m_iAudioHandle = AudioHandle.Invalid;
	}
	
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_iAudioHandle == AudioHandle.Invalid)
		{
			if (m_aQueue.IsEmpty())
			{
				ClearEventMask(GetOwner(), EntityEvent.FRAME);
				return;
			}
			else
			{
				string soundEvent = m_aQueue[0];
				m_aQueue.RemoveOrdered(0);
				m_iAudioHandle = m_PlayingSoundComponent.SoundEvent(soundEvent);
				Rpc(RpcAsk_PlaySoundEvent, soundEvent, GetGame().GetPlayerController().GetPlayerId()); // yuck. But unless I can get EOnFrame to work on non-owner clients, then this is the best I got.
			}
		}
		else if (m_PlayingSoundComponent.IsFinishedPlaying(m_iAudioHandle))
		{
			m_iAudioHandle = AudioHandle.Invalid;
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_PlaySoundEvent(string soundEvent, int playerId)
	{
		Rpc(RpcDo_PlaySoundEvent, soundEvent, playerId);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void RpcDo_PlaySoundEvent(string soundEvent, int playerId)
	{
		if (GetGame().GetPlayerController().GetPlayerId() == playerId)
			return;
		
		if (!m_PlayingSoundComponent)
		{
			m_PlayingSoundComponent = SCR_CommunicationSoundComponent.Cast(GetOwner().FindComponent(SCR_CommunicationSoundComponent));
			if (!m_PlayingSoundComponent)
			{
				Print("Could not find sound component!", LogLevel.ERROR);
				return;
			}
		}
		m_PlayingSoundComponent.TerminateAll();
		m_PlayingSoundComponent.SoundEvent(soundEvent);
	}
	
	
}