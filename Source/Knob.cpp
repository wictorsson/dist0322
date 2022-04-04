/*
  ==============================================================================

    Dial.cpp
    Created: 6 Mar 2022 9:44:00am
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#include "Knob.h"

Knob::Knob(std::string suffix)
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
   
    setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                             juce::MathConstants<float>::pi * 2.75f,
                             true);

    setVelocityBasedMode (true);
    setVelocityModeParameters (1.0, 1, 0.1, false);
    setRange (0.0, 100.0, 0.01);
    setValue (50.0);
    setDoubleClickReturnValue (true, 50.0);
    setTextValueSuffix (suffix);
    //lambda function
    onValueChange = [&]()
    {
        if (getValue() < 10)
            setNumDecimalPlacesToDisplay (2);
        else if (10 <= getValue() && getValue() < 100)
            setNumDecimalPlacesToDisplay (1);
        else
            setNumDecimalPlacesToDisplay (0);
    };
    setColour (juce::Slider::textBoxTextColourId, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    setLookAndFeel (&customLookAndFeel);
}

Knob::~Knob()
{
    setLookAndFeel (nullptr);
    
}
    

