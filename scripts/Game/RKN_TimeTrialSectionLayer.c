class RKN_TimeTrialSectionLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialSectionLayer : SCR_ScenarioFrameworkLayerBase
{
	[Attribute()]
	ref RKN_Get m_CourseGetter;
	
	ref array<RKN_TimeTrialObjectiveSlot> m_aObjectives = {};
	ref array<RKN_TimeTrialObjectiveSlot> m_aOptionalObjectives = {};
	RKN_TimeTrialCourseLayer m_Course;
	int m_iCompletedObjectives;
	
	override bool InitOtherThings()
	{
		RegisterSection();
		return super.InitOtherThings();
	}
	
	void RegisterObjective(RKN_TimeTrialObjectiveSlot obj, bool optional)
	{
		if (optional)
			m_aOptionalObjectives.Insert(obj);
		else
			m_aObjectives.Insert(obj);
	}
	
	void RegisterSection()
	{
		IEntity entity;
		if (m_CourseGetter)
		{
			SCR_ScenarioFrameworkParamBase paramBase = m_CourseGetter.Get(GetOwner());
			entity = SCR_ScenarioFrameworkParam<IEntity>.Cast(paramBase).GetValue();
		}
		
		if (!entity)
		{
			Print("Course not found!", LogLevel.ERROR);
			return;
		}
		m_Course = RKN_TimeTrialCourseLayer.Cast(entity.FindComponent(RKN_TimeTrialCourseLayer));
		if (!m_Course)
		{
			Print("Course entity does not have course component!", LogLevel.ERROR);
			return;
		}
		m_Course.RegisterSection(this);
	}
	
	void ActivateSection()
	{
		foreach (RKN_TimeTrialObjectiveSlot obj : m_aObjectives)
			obj.ActivateObjective();
		foreach (RKN_TimeTrialObjectiveSlot obj : m_aOptionalObjectives)
			obj.ActivateObjective();
	}
	
	void ResetSection()
	{
		m_iCompletedObjectives = 0;
		foreach (RKN_TimeTrialObjectiveSlot obj : m_aObjectives)
			obj.ResetObjective();
		foreach (RKN_TimeTrialObjectiveSlot obj : m_aOptionalObjectives)
			obj.ResetObjective();
	}
	
	void FinishObjective(RKN_TimeTrialObjectiveSlot objective)
	{
		if (++m_iCompletedObjectives >= m_aObjectives.Count())
		{
			m_Course.FinishSection(this);
		}
	}
}