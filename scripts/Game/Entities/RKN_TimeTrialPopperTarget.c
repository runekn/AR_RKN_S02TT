[EntityEditorProps(category: "GameScripted/FiringRange", description: "Handeles behaviour of target. Evaluates a hit.", color: "0 0 255 255")]
class RKN_TimeTrialPopperTargetClass : RKN_TimeTrialTargetEntityClass
{
}

// Shamelessly stole most from SCR_FiringRangeTarget, just without m_FiringRangeManager
class RKN_TimeTrialPopperTarget : RKN_TimeTrialTargetEntity
{
	const static string KEY_HEAD = "head";
	const static string KEY_CIRCLE = "circle";
	const static string KEY_LEGS = "legs";
	
	// definition of areas on target for scoring
	// 10 points area
	const float AREA_DIVIDER_UPPER = 0.85;
	const float AREA_DIVIDER_LOWER = 0.6;
	
	const float AREA_BOTTOM_VALID = 0.02;
	
	override bool IsHitValid(vector coordOfHit)
	{
		float HitY = coordOfHit[1];
		
		if (HitY > AREA_BOTTOM_VALID)
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Evaluate the hit coordinates and add a propper score
	override string GetHitKey(vector coordOfHit)
	{
		float HitY = coordOfHit[1];
				
		if (HitY > AREA_DIVIDER_UPPER)
			return KEY_HEAD;
		
		else if (HitY > AREA_DIVIDER_LOWER)
			return KEY_CIRCLE;
		
		else
			return KEY_LEGS;
	}
}