/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 6 Mar 2022 9:43:39am
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "CustomColours.h"

class CustomDial : public juce::LookAndFeel_V4
{
public:
    CustomDial();
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
    
    juce::Label* createSliderTextBox (juce::Slider& slider) override;
private:
    
    
    juce::DropShadow shadowProperties;
    juce::DropShadowEffect dialShadow;
};
