class RKN_TimeTrialSectionLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialSectionLayer : SCR_ScenarioFrameworkLayerBase
{
	[Attribute(category: "Time trial")]
	ref RKN_Get m_CourseGetter;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnResetActions;
	
	ref ScriptInvoker m_OnActive = new ScriptInvoker();
	ref ScriptInvoker m_OnFinish = new ScriptInvoker();
	
	ref array<RKN_TimeTrialObjectiveSlot> m_aObjectives = {};
	ref array<RKN_TimeTrialObjectiveSlot> m_aOptionalObjectives = {};
	RKN_TimeTrialCourseLayer m_Course;
	int m_iCompletedObjectives;
	bool m_bActive;
	
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
		m_Course = RKN_TimeTrialUtils.FindCourse(m_CourseGetter, GetOwner());
		if (!m_Course)
			return;
		m_Course.RegisterSection(this);
		m_Course.m_OnReset.Insert(ResetSection);
	}
	
	void ActivateSection()
	{
		m_bActive = true;
		m_OnActive.Invoke();
	}
	
	void ResetSection()
	{
		m_bActive = false;
		m_iCompletedObjectives = 0;
		foreach (SCR_ScenarioFrameworkActionBase action : m_aOnResetActions)
			action.Init(GetOwner());
	}
	
	void FinishObjective(RKN_TimeTrialObjectiveSlot objective)
	{
		if (!m_bActive)
			return;
		if (++m_iCompletedObjectives >= m_aObjectives.Count())
		{
			m_Course.FinishSection(this);
		}
	}
}