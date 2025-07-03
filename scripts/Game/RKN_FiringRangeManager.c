modded class SCR_FiringRangeManager
{
	override void EOnInit(IEntity owner)
	{	
		super.EOnInit(owner);
		
		// Fix bug with SCR_FiringRangeManager where it looks for m_ScoringSystem on the owner rather than gamemode
		if (!m_ScoringSystem) {
			ArmaReforgerScripted game = GetGame();
			if (!game)
				return;
			
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (!gameMode)
				return;
					
			m_ScoringSystem = SCR_FiringRangeScoringComponent.Cast(gameMode.FindComponent(SCR_FiringRangeScoringComponent));
		}
	}
}