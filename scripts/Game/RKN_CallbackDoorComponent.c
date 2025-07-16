class RKN_CallbackDoorComponentClass : DoorComponentClass
{
}

class RKN_CallbackDoorComponent : DoorComponent
{
	ref ScriptInvoker m_OnAction = new ScriptInvoker();
}