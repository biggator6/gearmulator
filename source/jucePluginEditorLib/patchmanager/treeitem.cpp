#include "treeitem.h"

#include "listmodel.h"
#include "patchmanager.h"
#include "savepatchdesc.h"
#include "tree.h"

#include "jucePluginLib/patchdb/patchdbtypes.h"
#include "juceUiLib/treeViewStyle.h"

namespace jucePluginEditorLib::patchManager
{
	TreeItem::TreeItem(PatchManager& _patchManager, const std::string& _title, const uint32_t _count/* = g_invalidCount*/) : m_patchManager(_patchManager), m_count(_count)
	{
		setTitle(_title);
	}

	TreeItem::~TreeItem()
	{
		getPatchManager().removeSelectedItem(getTree(), this);

		if(m_searchHandle != pluginLib::patchDB::g_invalidSearchHandle)
		{
			getPatchManager().cancelSearch(m_searchHandle);
			m_searchHandle = pluginLib::patchDB::g_invalidSearchHandle;
		}
	}

	void TreeItem::setTitle(const std::string& _title)
	{
		if (m_title == _title)
			return;
		m_title = _title;
		updateText();
	}

	void TreeItem::setCount(const uint32_t _count)
	{
		if (m_count == _count)
			return;
		m_count = _count;
		updateText();
	}

	void TreeItem::processDirty(const std::set<pluginLib::patchDB::SearchHandle>& _dirtySearches)
	{
		if (_dirtySearches.find(m_searchHandle) == _dirtySearches.end())
			return;

		const auto search = getPatchManager().getSearch(m_searchHandle);
		if (!search)
			return;

		processSearchUpdated(*search);
	}

	bool TreeItem::beginEdit(const std::string& _initialText, FinishedEditingCallback&& _callback)
	{
		auto pos = getItemPosition(true);
		pos.setHeight(getItemHeight());

		return Editable::beginEdit(getOwnerView(), pos, _initialText, std::move(_callback));
	}

	bool TreeItem::hasSearch() const
	{
		return m_searchHandle != pluginLib::patchDB::g_invalidSearchHandle;
	}

	Tree* TreeItem::getTree() const
	{
		return dynamic_cast<Tree*>(getOwnerView());
	}

	void TreeItem::removeFromParent(const bool _destroy) const
	{
		auto* parent = getParentItem();
		if (!parent)
		{
			if (_destroy)
				delete this;
			return;
		}
		const auto idx = getIndexInParent();
		parent->removeSubItem(idx, _destroy);
	}

	void TreeItem::setParent(TreeViewItem* _parent, const bool _sorted/* = false*/)
	{
		const auto* parentExisting = getParentItem();

		if (_parent == parentExisting)
			return;

		removeFromParent(false);

		if (_parent)
		{
			if(_sorted)
				_parent->addSubItemSorted(*this, this);
			else
				_parent->addSubItem(this);
		}
	}

	void TreeItem::itemSelectionChanged(const bool _isNowSelected)
	{
		if(_isNowSelected && m_forceDeselect)
		{
			m_forceDeselect = false;

			juce::MessageManager::callAsync([this]()
			{
				setSelected(false, false);
			});
			return;
		}

		m_selectedWasChanged = true;

		TreeViewItem::itemSelectionChanged(_isNowSelected);

		if(getTree()->isMultiSelectEnabled())
		{
			if (_isNowSelected)
				getPatchManager().addSelectedItem(getTree(), this);
			else
				getPatchManager().removeSelectedItem(getTree(), this);
		}
		else
		{
			if (_isNowSelected)
				getPatchManager().setSelectedItem(getTree(), this);
		}
	}

	void TreeItem::itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails, int insertIndex)
	{
		const auto patches = SavePatchDesc::getPatchesFromDragSource(dragSourceDetails);

		if(!patches.empty())
			patchesDropped(patches);
	}

	bool TreeItem::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& _dragSourceDetails)
	{
		const auto* list = dynamic_cast<ListModel*>(_dragSourceDetails.sourceComponent.get());

		const auto& patches = SavePatchDesc::getPatchesFromDragSource(_dragSourceDetails);

		return isInterestedInPatchList(list, patches);
	}

	void TreeItem::search(pluginLib::patchDB::SearchRequest&& _request)
	{
		cancelSearch();
		setCount(g_unknownCount);
		m_searchRequest = _request;
		m_searchHandle = getPatchManager().search(std::move(_request));
	}

	void TreeItem::processSearchUpdated(const pluginLib::patchDB::Search& _search)
	{
		setCount(static_cast<uint32_t>(_search.getResultSize()));
	}

	const pluginLib::patchDB::SearchRequest& TreeItem::getParentSearchRequest() const
	{
		return m_parentSearchRequest;
	}

	void TreeItem::setText(const std::string& _text)
	{
		if (m_text == _text)
			return;
		m_text = _text;
		repaintItem();
	}

	void TreeItem::updateText()
	{
		if (m_count == g_invalidCount)
			setText(m_title);
		else if (m_count == g_unknownCount)
			setText(m_title + " (?)");
		else
			setText(m_title + " (" + std::to_string(m_count) + ')');
	}

	void TreeItem::paintItem(juce::Graphics& _g, const int _width, const int _height)
	{
		getTree()->setColour(juce::TreeView::dragAndDropIndicatorColourId, juce::Colour(juce::ModifierKeys::currentModifiers.isShiftDown() ? 0xffff0000 : 0xff00ff00));

		const auto* style = dynamic_cast<const genericUI::TreeViewStyle*>(&getOwnerView()->getLookAndFeel());

		const auto color = getColor();

		_g.setColour(color != pluginLib::patchDB::g_invalidColor ? juce::Colour(color) : style ? style->getColor() : juce::Colour(0xffffffff));

		bool haveFont = false;
		bool antialias = true;

		if(style)
		{
			if (auto f = style->getFont())
			{
				if (style->boldRootItems() && getParentItem() == getTree()->getRootItem())
					f = f->boldened();
				_g.setFont(*f);
				haveFont = true;
			}
			antialias = style->getAntialiasing();
		}

		if(!haveFont)
		{
			auto fnt = _g.getCurrentFont();
			if (getParentItem() == getTree()->getRootItem())
				fnt = fnt.boldened();
			_g.setFont(fnt);
		}

		_g.setImageResamplingQuality(antialias ? juce::Graphics::highResamplingQuality : juce::Graphics::lowResamplingQuality);

		const juce::String t = juce::String::fromUTF8(m_text.c_str());
		_g.drawText(t, 0, 0, _width, _height, style ? style->getAlign() : juce::Justification(juce::Justification::centredLeft));
		TreeViewItem::paintItem(_g, _width, _height);
	}

	bool TreeItem::isInterestedInFileDrag(const juce::StringArray& _files)
	{
		return TreeViewItem::isInterestedInFileDrag(_files);
	}

	void TreeItem::filesDropped(const juce::StringArray& _files, const int _insertIndex)
	{
		const auto patches = m_patchManager.loadPatchesFromFiles(_files);

		if(!patches.empty())
			patchesDropped(patches);
		else
			TreeViewItem::filesDropped(_files, _insertIndex);
	}

	int TreeItem::compareElements(const TreeViewItem* _a, const TreeViewItem* _b)
	{
		const auto* a = dynamic_cast<const TreeItem*>(_a);
		const auto* b = dynamic_cast<const TreeItem*>(_b);

		if(a && b)
			return a->getText().compare(b->getText());

		if (_a < _b)
			return -1;
		if (_a > _b)
			return 1;
		return 0;
	}

	void TreeItem::setParentSearchRequest(const pluginLib::patchDB::SearchRequest& _parentSearch)
	{
		if(_parentSearch == m_parentSearchRequest)
			return;
		m_parentSearchRequest = _parentSearch;
		onParentSearchChanged(m_parentSearchRequest);
	}

	void TreeItem::itemClicked(const juce::MouseEvent& _mouseEvent)
	{
		if(_mouseEvent.mods.isPopupMenu())
		{
			TreeViewItem::itemClicked(_mouseEvent);
			return;
		}

		if(!m_deselectOnSecondClick)
		{
			TreeViewItem::itemClicked(_mouseEvent);
			return;
		}

		// we have the (for Juce) overly complex task to deselect a tree item on left click
		// Juce does not let us though, this click is sent on mouse down and it reselects the item
		// again on mouse up.
		const auto selectedWasChanged = m_selectedWasChanged;
		m_selectedWasChanged = false;

		if(!selectedWasChanged && isSelected() && getOwnerView()->isMultiSelectEnabled())
		{
			m_forceDeselect = true;
			setSelected(false, false);
		}
	}

	void TreeItem::cancelSearch()
	{
		if(m_searchHandle == pluginLib::patchDB::g_invalidSearchHandle)
			return;

		getPatchManager().cancelSearch(m_searchHandle);
		m_searchHandle = pluginLib::patchDB::g_invalidSearchHandle;
	}

	void TreeItem::patchesDropped(const std::vector<pluginLib::patchDB::PatchPtr>& _patches, const SavePatchDesc* _savePatchDesc/* = nullptr*/)
	{
		for (const auto& patch : _patches)
			patchDropped(patch);
	}
}
