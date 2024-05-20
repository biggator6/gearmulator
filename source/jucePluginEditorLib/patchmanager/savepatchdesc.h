#pragma once

#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

#include "../../jucePluginLib/patchdb/patchdbtypes.h"

namespace jucePluginEditorLib::patchManager
{
	class PatchManager;

	class SavePatchDesc : public juce::ReferenceCountedObject
	{
		static constexpr int InvalidPart = -1;

	public:
		SavePatchDesc(PatchManager& _pm, const int _part) : m_patchManager(_pm), m_part(_part)
		{
		}

		SavePatchDesc(PatchManager& _pm, std::map<uint32_t, pluginLib::patchDB::PatchPtr>&& _patches) : m_patchManager(_pm), m_part(InvalidPart), m_patches(std::move(_patches))
		{
		}

		auto getPart() const { return m_part; }

		std::map<uint32_t, pluginLib::patchDB::PatchPtr>& getPatches() const;

		bool isPartValid() const { return m_part != InvalidPart; }
		bool hasPatches() const { return !getPatches().empty(); }

		bool writePatchesToFile(const juce::File& _file) const;

		static const SavePatchDesc* fromDragSource(const juce::DragAndDropTarget::SourceDetails& _source)
		{
			return dynamic_cast<const SavePatchDesc*>(_source.description.getObject());
		}

		static std::vector<pluginLib::patchDB::PatchPtr> getPatchesFromDragSource(const juce::DragAndDropTarget::SourceDetails& _dragSourceDetails);

	private:
		PatchManager& m_patchManager;
		int m_part;
		mutable std::map<uint32_t, pluginLib::patchDB::PatchPtr> m_patches;
	};
}
