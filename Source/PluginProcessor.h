/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oschilloscope.h"
//#include "Visualiser.h"
//==============================================================================
/**
*/
class Dist0322AudioProcessor  : public juce::AudioProcessor, juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    Dist0322AudioProcessor();
    ~Dist0322AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::LinearSmoothedValue<float> inputDB {0.01};
    juce::LinearSmoothedValue<float> driveDB {0.01};
    juce::LinearSmoothedValue<float> outputDB {0.01};
    juce::LinearSmoothedValue<float> mix {0.01};
    // apvts Object
    juce::AudioProcessorValueTreeState apvts;
    
    AudioBufferQueue<float> & getAudioBufferQueue() { return scopeDataQueue; }
    
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    
   
private:
    AudioBufferQueue<float> scopeDataQueue;
    ScopeDataCollector<float> scopeDataCollector;
    // apvts Function
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
  
    //==============================================================================
    const float piDiv = 2.0/ juce::MathConstants<float>::pi;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dist0322AudioProcessor)
};
