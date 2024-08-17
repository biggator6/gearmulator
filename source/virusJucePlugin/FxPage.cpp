#include "FxPage.h"

#include "VirusEditor.h"

#include "ParameterNames.h"
#include "VirusController.h"

namespace genericVirusUI
{
	FxPage::FxPage(const VirusEditor& _editor)
	{
		const auto delayReverbMode = _editor.getController().getParameterIndexByName(virus::g_paramDelayReverbMode);
		const auto p = _editor.getController().getParamValueObject(delayReverbMode, 0);

		if(!p)
			return;

		const auto containerReverb = _editor.findComponent("ContainerReverb");
		const auto containerDelay = _editor.findComponent("ContainerDelay");

		m_conditionReverb.reset(new genericUI::ConditionByParameterValues(*containerReverb, p, 0, {2,3,4}));
		m_conditionDelay.reset(new genericUI::ConditionByParameterValues(*containerDelay, p, 0, {0,1,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26}));
	}

	FxPage::~FxPage()
	{
		m_conditionReverb.reset();
		m_conditionDelay.reset();
	}
}
