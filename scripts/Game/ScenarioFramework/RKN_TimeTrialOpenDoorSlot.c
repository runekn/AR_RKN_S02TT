class RKN_TimeTrialOpenDoorSlotClass : RKN_TimeTrialObjectiveSlotClass
{
	
}

class RKN_TimeTrialOpenDoorSlot : RKN_TimeTrialObjectiveSlot
{
	RKN_CallbackDoorComponent m_Component;
	
	override void FinishInit()
	{		
		m_Component = FindDoorComponent(m_Entity);
		if (m_Component)
			m_Component.m_OnAction.Insert(OnDoorAction);
		
		super.FinishInit();
	}
	
	override void ResetObjective()
	{
		if (m_Component)
			m_Component.SetControlValue(0);
		super.ResetObjective();
	}
	
	void OnDoorAction()
	{
		FinishObjective();
	}
	
	RKN_CallbackDoorComponent FindDoorComponent(IEntity entity)
	{
		if (entity.FindComponent(RKN_CallbackDoorComponent))
			return RKN_CallbackDoorComponent.Cast(entity.FindComponent(RKN_CallbackDoorComponent));
		
		SCR_ScenarioFrameworkParam<IEntity> param = RKN_Get.findComponentInChildren(entity, RKN_CallbackDoorComponent);
		if (!param)
			return null;
		return RKN_CallbackDoorComponent.Cast(param.GetValue().FindComponent(RKN_CallbackDoorComponent));
	}
}