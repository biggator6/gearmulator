#pragma once

#include "../jucePluginEditorLib/partbutton.h"

#include "../jucePluginLib/event.h"

namespace xtJucePlugin
{
	class Editor;

	class PartName : public jucePluginEditorLib::PartButton<juce::TextButton>
	{
	public:
		explicit PartName(Editor& _editor);

	private:
		void updatePartName();

		Editor& m_editor;
		pluginLib::EventListener<uint8_t> m_onProgramChanged;
		pluginLib::EventListener<bool> m_onPlayModeChanged;
	};
}