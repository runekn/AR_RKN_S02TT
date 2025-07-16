class RKN_TimeTrialSectionLayerClass : SCR_ScenarioFrameworkLayerBaseClass
{
}

class RKN_TimeTrialSectionLayer : SCR_ScenarioFrameworkLayerBase
{
	[Attribute(category: "Time trial")]
	ref RKN_Get m_CourseGetter;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnResetActions;
	
	[Attribute(category: "Time trial")]
	ref array<ref SCR_ScenarioFrameworkActionBase> m_aOnFinishActions;
	
	ref ScriptInvoker m_OnActive = new ScriptInvoker();
	ref ScriptInvoker m_OnFinish = new ScriptInvoker();
	
	ref array<RKN_TimeTrialObjectiveSlot> m_aObjectives = {};
	ref array<RKN_TimeTrialObjectiveSlot> m_aOptionalObjectives = {}; // TODO: Not using for anything
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
		m_Course.m_OnCancel.Insert(ResetSection);
	}
	
	void ActivateSection()
	{
		m_bActive = true;
		int i = 0;
		foreach (RKN_TimeTrialObjectiveSlot obj : m_aObjectives)
			if (obj.m_bCountInUI)
				i++;
		m_Course.GetCourseManager().TargetsRemaining(m_Course.m_iCourseIndex, i);
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
		
		m_iCompletedObjectives++;
		if (objective.m_bCountInUI)
		{
			int prev = m_Course.GetCourseManager().GetData(m_Course.m_iCourseIndex).m_CurrentScoreInfo.m_iSectionTargetsRemaining;
			m_Course.GetCourseManager().TargetsRemaining(m_Course.m_iCourseIndex, prev - 1);
		}
		if (m_iCompletedObjectives >= m_aObjectives.Count())
		{
			m_OnFinish.Invoke();
			foreach (SCR_ScenarioFrameworkActionBase action : m_aOnFinishActions)
				action.Init(GetOwner());
			m_Course.FinishSection(this);
		}
	}
}