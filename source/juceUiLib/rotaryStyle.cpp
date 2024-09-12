#include "rotaryStyle.h"

#include "uiObject.h"

namespace genericUI
{
	void RotaryStyle::apply(Editor& _editor, const UiObject& _object)
	{
		UiObjectStyle::apply(_editor, _object);

		const auto style = _object.getProperty("style", "Rotary");

		if(style == "LinearHorizontal")
			m_style = Style::LinearHorizontal;
		else if(style == "LinearVertical")
			m_style = Style::LinearVertical;
		else if(style == "Rotary")
			m_style = Style::Rotary;
		else
			throw std::runtime_error("Unknown slider style type " + style);
	}

	void RotaryStyle::drawRotarySlider(juce::Graphics& _graphics, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& _slider)
	{
        if(!m_drawable || !m_tileSizeX || !m_tileSizeY)
            return;

		const auto w = m_drawable->getWidth();
		const auto h = m_drawable->getHeight();

//		const auto stepsX = w / m_tileSizeX;
		const auto stepsY = h / m_tileSizeY;

//      const auto stepX = juce::roundToInt(static_cast<float>(stepsX - 1) * sliderPosProportional);
		const auto stepY = juce::roundToInt(static_cast<float>(stepsY - 1) * sliderPosProportional);

		auto t = juce::AffineTransform();

		t = t.translated(static_cast<float>(x), static_cast<float>(y));
		t = t.translated(0.0f, static_cast<float>(-m_tileSizeY * stepY));

		if(width != m_tileSizeX || height != m_tileSizeY)
		{
			t = t.scaled(static_cast<float>(width) / static_cast<float>(m_tileSizeX), static_cast<float>(height) / static_cast<float>(m_tileSizeY));
		}

		m_drawable->draw(_graphics, 1.0f, t);
	}

	void RotaryStyle::drawLinearSlider(juce::Graphics& _graphics, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle _sliderStyle, juce::Slider& _slider)
	{
		if (!m_drawable || !_slider.isEnabled())
			return;

		if (_sliderStyle == juce::Slider::LinearHorizontal)
		{
			const auto pos = sliderPos - static_cast<float>(m_drawable->getWidth()) * 0.5f;
			m_drawable->drawAt(_graphics, pos, static_cast<float>(y), 1.0f);
		}
		else if (_sliderStyle == juce::Slider::LinearVertical)
		{
			const auto pos = sliderPos - static_cast<float>(m_drawable->getHeight()) * 0.5f;
			m_drawable->drawAt(_graphics, static_cast<float>(x), pos, 1.0f);
		}
	}

	int RotaryStyle::getSliderThumbRadius(juce::Slider& _slider)
	{
		if(!m_drawable)
			return UiObjectStyle::getSliderThumbRadius(_slider);
		if(_slider.getSliderStyle() == juce::Slider::LinearHorizontal)
			return m_drawable->getWidth() >> 1;
		if(_slider.getSliderStyle() == juce::Slider::LinearVertical)
			return m_drawable->getHeight() >> 1;

		return UiObjectStyle::getSliderThumbRadius(_slider);
	}
}
