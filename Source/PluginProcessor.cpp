/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Dist0322AudioProcessor::Dist0322AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()),
scopeDataCollector(scopeDataQueue)
#endif
{
    apvts.addParameterListener("INPUT", this);
    apvts.addParameterListener("OUTPUT", this);
    apvts.addParameterListener("MIX", this);
    apvts.addParameterListener("DRIVE", this);
    
}

Dist0322AudioProcessor::~Dist0322AudioProcessor()
{
    apvts.removeParameterListener("INPUT", this);
    apvts.removeParameterListener("OUTPUT", this);
    apvts.removeParameterListener("MIX", this);
    apvts.removeParameterListener("DRIVE", this);
}

//==============================================================================
const juce::String Dist0322AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Dist0322AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Dist0322AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Dist0322AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Dist0322AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Dist0322AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Dist0322AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Dist0322AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Dist0322AudioProcessor::getProgramName (int index)
{
    return {};
}

void Dist0322AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Dist0322AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
   

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
   
    inputDB.reset(sampleRate, 0.02f);
    driveDB.reset(sampleRate, 0.02f);
    mix.reset(sampleRate, 0.02f);
    outputDB.reset(sampleRate, 0.02f);
}

void Dist0322AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
 
}
#ifndef JucePlugin_PreferredChannelConfigurations
bool Dist0322AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Dist0322AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

   
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        
        for (int sample{0}; sample < buffer.getNumSamples(); ++sample)
        {
           
            auto* channelData = buffer.getWritePointer (channel);
            const auto input = channelData[sample] * juce::Decibels::decibelsToGain(inputDB.getNextValue());
           // const auto inputDry = channelData[sample];
            
            // arctan distortion = soft clipping
            const auto softClip = piDiv * std::atanf(input * juce::Decibels::decibelsToGain(driveDB.getNextValue()));
            
            auto blend = input * (1.0 - mix.getNextValue()) + softClip * mix.getNextValue();
            
            blend *= juce::Decibels::decibelsToGain(outputDB.getNextValue());
            
            channelData[sample] = blend;
       
        }
    }
    //Oscilloscope
    scopeDataCollector.process(buffer.getReadPointer(0), (size_t)buffer.getNumSamples());
  
   // std::cout << ((size_t)buffer.getNumSamples());
}


//==============================================================================
bool Dist0322AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Dist0322AudioProcessor::createEditor()
{
    return new Dist0322AudioProcessorEditor (*this);
   // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Dist0322AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = apvts.copyState();
           std::unique_ptr<juce::XmlElement> xml (state.createXml());
           copyXmlToBinary (*xml, destData);
}

void Dist0322AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
           if (xmlState.get() != nullptr)
               if (xmlState->hasTagName (apvts.state.getType()))
                   apvts.replaceState (juce::ValueTree::fromXml (*xmlState));

}

//==============================================================================
// This creates new instances of the plugin..


juce::AudioProcessorValueTreeState::ParameterLayout Dist0322AudioProcessor::createParameters()

{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterInt>("INPUT", "Input", -30, 12, 0));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>("DRIVE", "Drive", 0, 15, 0));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>("MIX", "Mix", 0, 100,0));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>("OUTPUT", "Output", -30, 12, 0));
    
    return {params.begin(), params.end()};
}

void Dist0322AudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "INPUT")
    {
        inputDB.setTargetValue(newValue);
    }
    if (parameterID == "OUTPUT")
    {
        outputDB.setTargetValue(newValue);
    }
    if (parameterID == "DRIVE")
    {
        driveDB.setTargetValue(newValue);
    }
    if (parameterID == "MIX")
    {
        mix.setTargetValue(newValue/100);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dist0322AudioProcessor();
}
