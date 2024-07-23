#pragma once

#include "editable.h"
#include "savepatchdesc.h"

#include "juce_gui_basics/juce_gui_basics.h"

#include "jucePluginLib/patchdb/patchdbtypes.h"
#include "jucePluginLib/patchdb/search.h"

namespace pluginLib::patchDB
{
	struct Search;
	struct SearchRequest;
}

namespace jucePluginEditorLib::patchManager
{
	class ListModel;
	class Tree;

	static constexpr uint32_t g_invalidCount = ~0;
	static constexpr uint32_t g_unknownCount = g_invalidCount - 1;

	class PatchManager;

	class TreeItem : public juce::TreeViewItem, protected Editable
	{
	public:
		TreeItem(PatchManager& _patchManager, const std::string& _title, uint32_t _count = g_invalidCount);
		~TreeItem() override;

		PatchManager& getPatchManager() const { return m_patchManager; }

		void setTitle(const std::string& _title);
		virtual void setCount(uint32_t _count);

		auto getSearchHandle() const { return m_searchHandle; }
		const auto& getSearchRequest() const { return m_searchRequest; }

		virtual void processDirty(const std::set<pluginLib::patchDB::SearchHandle>& _dirtySearches);

		virtual bool beginEdit() { return false; }
		bool beginEdit(const std::string& _initialText, FinishedEditingCallback&& _callback);

		virtual void patchDropped(const pluginLib::patchDB::PatchPtr& _patch) {}
		virtual void patchesDropped(const std::vector<pluginLib::patchDB::PatchPtr>& _patches, const SavePatchDesc* _savePatchDesc = nullptr);

		bool hasSearch() const;

		Tree* getTree() const;

		void removeFromParent(bool _destroy) const;
		void setParent(TreeViewItem* _parent, bool _sorted = false);

		const std::string& getText() const { return m_text; }

		// TreeViewItem
		void itemSelectionChanged(bool _isNowSelected) override;
		void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails, int insertIndex) override;

		bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& _dragSourceDetails) override;

		virtual bool isInterestedInPatchList(const ListModel* _sourceList, const std::vector<pluginLib::patchDB::PatchPtr>& _patches)		{ return false; }

		bool isInterestedInFileDrag(const juce::StringArray& _files) override;
		void filesDropped(const juce::StringArray& _files, int _insertIndex) override;

		virtual int compareElements(const TreeViewItem* _a, const TreeViewItem* _b);

		virtual void setParentSearchRequest(const pluginLib::patchDB::SearchRequest& _parentSearch);

		virtual pluginLib::patchDB::Color getColor() const { return pluginLib::patchDB::g_invalidColor; }

		void itemClicked(const juce::MouseEvent&) override;

		void setDeselectonSecondClick(const bool _deselect) { m_deselectOnSecondClick = _deselect; }

	protected:
		void cancelSearch();
		void search(pluginLib::patchDB::SearchRequest&& _request);
		virtual void processSearchUpdated(const pluginLib::patchDB::Search& _search);
		virtual void onParentSearchChanged(const pluginLib::patchDB::SearchRequest& _parentSearchRequest) {}
		const pluginLib::patchDB::SearchRequest& getParentSearchRequest() const;

	private:
		bool mightContainSubItems() override { return true; }

		void setText(const std::string& _text);
		void updateText();
		void paintItem(juce::Graphics& _g, int _width, int _height) override;

		PatchManager& m_patchManager;

		std::string m_title;
		uint32_t m_count = g_invalidCount;

		std::string m_text;

		pluginLib::patchDB::SearchRequest m_parentSearchRequest;

		pluginLib::patchDB::SearchRequest m_searchRequest;
		uint32_t m_searchHandle = pluginLib::patchDB::g_invalidSearchHandle;

		bool m_deselectOnSecondClick = false;
		bool m_selectedWasChanged = false;
		bool m_forceDeselect = false;
	};
}
