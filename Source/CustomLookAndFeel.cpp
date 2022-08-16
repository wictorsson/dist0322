/*
  ==============================================================================

    CustomLookAndFeel.cpp
    Created: 6 Mar 2022 9:43:39am
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#include "CustomLookAndFeel.h"

CustomDial::CustomDial()
{
    
}

void CustomDial::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                  const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    float diameter = fmin(width, height) * .7;
    float radius = diameter * 0.5;
    float centerX = x + width * 0.5;
    float centerY = y + height * 0.5;
    float rx = centerX - radius;
    float ry = centerY -radius;
    float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));
    
    juce::Rectangle<float>dialArea(rx, ry, diameter, diameter);
    g.setColour(juce::Colour::fromFloatRGBA(0.2941f, 0.4784f, 0.2784f, 1.0f).darker(0.2f));
    g.fillEllipse(dialArea);
    
    g.setColour(juce::Colours::black.brighter(0.2f).withAlpha(0.1f));
    g.drawEllipse(rx, ry, diameter, diameter, 3.0f);
    
    juce::Path dialTick;
    g.setColour(juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 1).darker(0.2f));
    dialTick.addRectangle(0, -radius + 6, 2.0f, radius * 0.3);
    g.fillPath(dialTick, juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    
    shadowProperties.radius = 24;
    shadowProperties.offset = juce::Point<int>(-1,4);
    shadowProperties.colour = juce::Colours::black.withAlpha(0.8f);
    dialShadow.setShadowProperties(shadowProperties);
    slider.setComponentEffect(&dialShadow);
}

juce::Label* CustomDial::createSliderTextBox (juce::Slider& slider)
{
    auto* l = new juce::Label();

    l->setJustificationType (juce::Justification::centred);
    l->setColour (juce::Label::textColourId, slider.findColour (juce::Slider::textBoxTextColourId));
    l->setColour (juce::Label::textWhenEditingColourId, slider.findColour (juce::Slider::textBoxTextColourId));
    l->setColour (juce::Label::outlineWhenEditingColourId, juce::Colours::transparentWhite);
    l->setInterceptsMouseClicks (false, false);
    l->setFont (14.0f);

    return l;
}
