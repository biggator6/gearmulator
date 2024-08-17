#include "uiObjectStyle.h"

#include "editor.h"
#include "uiObject.h"

namespace genericUI
{
	UiObjectStyle::UiObjectStyle(Editor& _editor) : m_editor(_editor) {}

	void UiObjectStyle::setTileSize(int _w, int _h)
	{
		m_tileSizeX = _w;
		m_tileSizeY = _h;
	}

	void UiObjectStyle::setDrawable(juce::Drawable* _drawable)
	{
		m_drawable = _drawable;
	}

	void UiObjectStyle::setTextHeight(int _height)
	{
		m_textHeight = _height;
	}

	void UiObjectStyle::apply(Editor& _editor, const UiObject& _object)
	{
		const auto x = _object.getPropertyInt("tileSizeX");
		const auto y = _object.getPropertyInt("tileSizeY");

		setTileSize(x,y);

		const auto tex = _object.getProperty("texture");
		if(!tex.empty())
			setDrawable(_editor.getImageDrawable(tex));

		setTextHeight(_object.getPropertyInt("textHeight"));

		m_text = _object.getProperty("text");
		const auto color = _object.getProperty("color");

		auto parseColor = [&_object](juce::Colour& _target, const std::string& _prop)
		{
			const auto color = _object.getProperty(_prop);
			UiObjectStyle::parseColor(_target, color);
		};

		parseColor(m_color, "color");
		parseColor(m_bgColor, "backgroundColor");
		parseColor(m_linesColor, "linesColor");
		parseColor(m_selectedItemBgColor, "selectedItemBackgroundColor");
		parseColor(m_outlineColor, "outlineColor");

		const auto alignH = _object.getProperty("alignH");
		if(!alignH.empty())
		{
			juce::Justification a = 0;
			switch (alignH[0])
			{
			case 'L':	a = juce::Justification::left;					break;
			case 'C':	a = juce::Justification::horizontallyCentred;	break;
			case 'R':	a = juce::Justification::right;					break;
			}
			m_align = a;
		}

		const auto alignV = _object.getProperty("alignV");
		if(!alignV.empty())
		{
			juce::Justification a = 0;
			switch (alignV[0])
			{
			case 'T':	a = juce::Justification::top;					break;
			case 'C':	a = juce::Justification::verticallyCentred;		break;
			case 'B':	a = juce::Justification::bottom;				break;
			}
			m_align = m_align.getFlags() | a.getFlags();
		}

		m_fontFile = _object.getProperty("fontFile");
		m_fontName = _object.getProperty("fontName");

		m_bold = _object.getPropertyInt("bold") != 0;
		m_italic = _object.getPropertyInt("italic") != 0;

		m_url = _object.getProperty("url");

		m_offsetL = _object.getPropertyInt("offsetL", 1);
		m_offsetT = _object.getPropertyInt("offsetT", 1);
		m_offsetR = _object.getPropertyInt("offsetR", -30);
		m_offsetB = _object.getPropertyInt("offsetB", -2);

		m_hitAreaOffset.setX(_object.getPropertyInt("hitOffsetL", 0));
		m_hitAreaOffset.setY(_object.getPropertyInt("hitOffsetT", 0));
		m_hitAreaOffset.setWidth(_object.getPropertyInt("hitOffsetR", 0));
		m_hitAreaOffset.setHeight(_object.getPropertyInt("hitOffsetB", 0));
	}

	std::optional<juce::Font> UiObjectStyle::getFont() const
	{
		if (m_fontFile.empty())
			return {};

		auto font = juce::Font(m_editor.getFont(m_fontFile).getTypefacePtr());
		applyFontProperties(font);
		return font;
	}

	bool UiObjectStyle::parseColor(juce::Colour& _color, const std::string& _colorString)
	{
		uint32_t r,g,b,a;

		if(_colorString.size() == 8)
		{
			sscanf(_colorString.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);
		}
		else if(_colorString.size() == 6)
		{
			sscanf(_colorString.c_str(), "%02x%02x%02x", &r, &g, &b);
			a = 255;
		}
		else
		{
			return false;
		}

		_color = juce::Colour(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a));
		return true;
	}

	juce::Font UiObjectStyle::getComboBoxFont(juce::ComboBox& _comboBox)
	{
		if (const auto f = getFont())
			return *f;

		auto font = LookAndFeel_V4::getComboBoxFont(_comboBox);
		applyFontProperties(font);
		return font;
	}

	juce::Font UiObjectStyle::getLabelFont(juce::Label& _label)
	{
		if (const auto f = getFont())
			return *f;

		auto font = LookAndFeel_V4::getLabelFont(_label);
		applyFontProperties(font);
		return font;
	}

	juce::Font UiObjectStyle::getPopupMenuFont()
	{
		auto f = LookAndFeel_V4::getPopupMenuFont();
		f.setHeight(f.getHeight() * 2.0f);
		return f;
	}

	juce::Font UiObjectStyle::getTextButtonFont(juce::TextButton& _textButton, int buttonHeight)
	{
		if (const auto f = getFont())
			return *f;
		auto font = LookAndFeel_V4::getTextButtonFont(_textButton, buttonHeight);
		applyFontProperties(font);
		return font;
	}

	bool UiObjectStyle::shouldPopupMenuScaleWithTargetComponent(const juce::PopupMenu::Options& _options)
	{
		return true;
	}

	juce::PopupMenu::Options UiObjectStyle::getOptionsForComboBoxPopupMenu(juce::ComboBox& _comboBox, juce::Label& _label)
	{
		return LookAndFeel_V4::getOptionsForComboBoxPopupMenu(_comboBox, _label).withStandardItemHeight(0);
	}

	void UiObjectStyle::applyFontProperties(juce::Font& _font) const
	{
		if(m_textHeight)
			_font.setHeight(static_cast<float>(m_textHeight));
		_font.setBold(m_bold);
		_font.setItalic(m_italic);
	}

	void UiObjectStyle::applyColorIfNotZero(juce::Component& _target, int _id, const juce::Colour& _col)
	{
		if(_col.getARGB())
			_target.setColour(_id, _col);
	}
}
