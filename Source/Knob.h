/*
  ==============================================================================

    Dial.h
    Created: 6 Mar 2022 9:44:00am
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#pragma once
#include "CustomLookAndFeel.h"

class Knob : public juce::Slider
{
public:
    Knob(std::string suffix);
    ~Knob();
    
private:
    CustomLookAndFeel customLookAndFeel;
};

