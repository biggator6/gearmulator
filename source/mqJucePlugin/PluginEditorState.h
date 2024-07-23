#pragma once

#include <functional>

#include "jucePluginEditorLib/pluginEditorState.h"

namespace juce
{
	class Component;
}

class AudioPluginAudioProcessor;

class PluginEditorState : public jucePluginEditorLib::PluginEditorState
{
public:
	explicit PluginEditorState(AudioPluginAudioProcessor& _processor);
	void initContextMenu(juce::PopupMenu& _menu) override;
	bool initAdvancedContextMenu(juce::PopupMenu& _menu, bool _enabled) override;
private:
	jucePluginEditorLib::Editor* createEditor(const Skin& _skin) override;
};
