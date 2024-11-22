#pragma once

#include <string>

#include "juce_gui_basics/juce_gui_basics.h"

#include "button.h"
#include "uiObject.h"

#include "editorInterface.h"

namespace genericUI
{
	class Slider;

	class Editor : public juce::Component, public juce::DragAndDropContainer
	{
	public:
		explicit Editor(EditorInterface& _interface);
		Editor(const Editor&) = delete;
		Editor(Editor&&) = delete;

		void create(const std::string& _jsonFilename);

		std::string exportToFolder(const std::string& _folder) const;

		juce::Drawable* getImageDrawable(const std::string& _texture);
		std::unique_ptr<juce::Drawable> createImageDrawable(const std::string& _texture);
		const juce::Font& getFont(const std::string& _fontFile);

		void registerComponent(const std::string& _name, juce::Component* _component);
		void registerTabGroup(TabGroup* _group);

		EditorInterface& getInterface() const { return m_interface; }

		static const std::vector<juce::Component*>& findComponents(const std::map<std::string, std::vector<juce::Component*>>& _components, const std::string& _name, uint32_t _expectedCount, const std::string& _typename);
		const std::vector<juce::Component*>& findComponents(const std::string& _name, uint32_t _expectedCount = 0) const;
		const std::vector<juce::Component*>& findComponentsByParam(const std::string& _name, uint32_t _expectedCount = 0) const;

		template<typename T>
		void findComponents(std::vector<T*>& _dst, const std::string& _name, uint32_t _expectedCount = 0) const
		{
			const auto& res = findComponents(_name, _expectedCount);
			for (auto* s : res)
			{
				auto* t = dynamic_cast<T*>(s);
				if(t)
					_dst.push_back(t);
			}
		}

		juce::Component* findComponent(const std::string& _name, bool _mustExist = true) const;
		juce::Component* findComponentByParam(const std::string& _param, bool _mustExist = true) const;

		template<typename T>
		T* findComponentT(const std::string& _name, bool _mustExist = true) const
		{
			juce::Component* c = findComponent(_name, _mustExist);
			return dynamic_cast<T*>(c);
		}

		template<typename T>
		T* findComponentByParamT(const std::string& _name, const bool _mustExist = true) const
		{
			juce::Component* c = findComponentByParam(_name, _mustExist);
			return dynamic_cast<T*>(c);
		}

		float getScale() const { return m_scale; }

		TabGroup* findTabGroup(const std::string& _name, bool _mustExist = true)
		{
			const auto it = m_tabGroupsByName.find(_name);
			if(it == m_tabGroupsByName.end())
			{
				if(_mustExist)
					throw std::runtime_error("tab group with name " + _name + " not found");
				return nullptr;
			}
			return it->second;
		}

		size_t getTabGroupCount() const
		{
			return m_tabGroupsByName.size();
		}

		size_t getConditionCountRecursive() const;
		size_t getControllerLinkCountRecursive() const;
		void registerTemplate(const std::shared_ptr<UiObject>& _value);

		static void setEnabled(juce::Component& _component, bool _enable);

		virtual void setCurrentPart(uint8_t _part);
		void updateKeyValueConditions(const std::string& _key, const std::string& _value) const;

		juce::TooltipWindow& getTooltipWindow() { return m_tooltipWindow; }

		std::shared_ptr<UiObject> getTemplate(const std::string& _name) const;

		virtual void setPerInstanceConfig(const std::vector<uint8_t>& _data) {}
		virtual void getPerInstanceConfig(std::vector<uint8_t>& _data) {}

		virtual Slider* createJuceComponent(Slider*, UiObject& _object) { return nullptr; }
		virtual juce::Component* createJuceComponent(juce::Component*, UiObject& _object) { return nullptr; }
		virtual juce::ComboBox* createJuceComponent(juce::ComboBox*, UiObject& _object) { return nullptr; }
		virtual juce::Label* createJuceComponent(juce::Label*, UiObject& _object) { return nullptr; }
		virtual Button<juce::HyperlinkButton>* createJuceComponent(Button<juce::HyperlinkButton>*, UiObject& _object) { return nullptr; }
		virtual Button<juce::DrawableButton>* createJuceComponent(Button<juce::DrawableButton>*, UiObject& _object, const std::string& _name, juce::DrawableButton::ButtonStyle) { return nullptr; }
		virtual Button<juce::TextButton>* createJuceComponent(Button<juce::TextButton>*, UiObject& _object) { return nullptr; }

		const UiObject& getRootObject() const { return *m_rootObject; }

		static bool resizeDrawableImage(juce::DrawableImage& _drawable, uint32_t _percent);

		void paint(juce::Graphics& g) override;
		
		bool selectTabWithComponent(const juce::Component* _component) const;

	private:
		EditorInterface& m_interface;

		std::string m_jsonFilename;

		std::map<std::string, std::unique_ptr<juce::Drawable>> m_drawables;
		std::map<std::string, juce::Font> m_fonts;

		std::unique_ptr<UiObject> m_rootObject;

		std::map<std::string, std::vector<juce::Component*>> m_componentsByName;
		std::map<std::string, std::vector<juce::Component*>> m_componentsByParameter;
		std::map<std::string, TabGroup*> m_tabGroupsByName;
		std::map<std::string, std::shared_ptr<UiObject>> m_templates;

		juce::TooltipWindow m_tooltipWindow;

		float m_scale = 1.0f;
	};
}
