[EntityEditorProps(category: "GameScripted/FiringRange", description: "Handeles behaviour of target. Evaluates a hit.", color: "0 0 255 255")]
class RKN_TimeTrialStandardTargetClass : RKN_TimeTrialTargetEntityClass
{
}

// Shamelessly stole most from SCR_FiringRangeTarget, just without m_FiringRangeManager
class RKN_TimeTrialStandardTarget : RKN_TimeTrialTargetEntity
{
	const static string KEY_CENTER = "center";
	const static string KEY_CIRCLE_1 = "circle1";
	const static string KEY_CIRCLE_2 = "circle2";
	const static string KEY_CIRCLE_3 = "circle3";
	
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
	
	override bool IsHitValid(vector coordOfHit)
	{
		float HitX = coordOfHit[0];
		float HitY = coordOfHit[1];
		
		if (HitX < AREA_1_W_LEFT && HitX > AREA_1_W_RIGHT && HitY < AREA_1_H_UP && HitY > AREA_BOTTOM_VALID)
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Evaluate the hit coordinates and add a propper score
	override string GetHitKey(vector coordOfHit)
	{				
		float HitX = coordOfHit[0];
		float HitY = coordOfHit[1];
				
		if (HitX < AREA_10_W_LEFT && HitX > AREA_10_W_RIGHT && HitY < AREA_10_H_UP && HitY > AREA_10_H_DOWN)
			return KEY_CENTER;
		
		else if (HitX < AREA_5_W_LEFT && HitX > AREA_5_W_RIGHT && HitY < AREA_5_H_UP && HitY > AREA_5_H_DOWN)
			return KEY_CIRCLE_1;
		
		else if (HitX < AREA_3_W_LEFT && HitX > AREA_3_W_RIGHT && HitY < AREA_3_H_UP && HitY > AREA_3_H_DOWN)
			return KEY_CIRCLE_2;
		
		else if (HitX < AREA_1_W_LEFT && HitX > AREA_1_W_RIGHT && HitY < AREA_1_H_UP && HitY > AREA_1_H_DOWN)
			return KEY_CIRCLE_3;
		
		return "";
	}
}