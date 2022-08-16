/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "Knob.h"

//==============================================================================
/**
*/
class lineComponent   : public juce::Component
{
public:
    lineComponent() {}
    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
        g.drawLine (0.0f, (float) getHeight() / 3, (float) getWidth(), (float) getHeight() / 3, 1.0f);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (lineComponent)
};

class Dist0322AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Dist0322AudioProcessorEditor (Dist0322AudioProcessor&);
    ~Dist0322AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //void sliderValueChanged(juce::Slider* slider) override;
    
    Knob inputKnob{" dB"}, driveKnob{" dB"}, mixKnob{" %"}, outputKnob{" dB"};

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Dist0322AudioProcessor& audioProcessor;
    //send parameter to constructor
    CustomDial customLookAndFeel;
    
    lineComponent line;
   
    
    juce::Label inputLabel, driveLabel, mixLabel, outputLabel, title;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>inputAttachment, driveAttachment, mixAttachment, outputAttachment;
    
    ScopeComponent<float> scopeComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dist0322AudioProcessorEditor)
};
