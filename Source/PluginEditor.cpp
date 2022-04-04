/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Dist0322AudioProcessorEditor::Dist0322AudioProcessorEditor (Dist0322AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) ,scopeComponent(p.getAudioBufferQueue())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    //Resizeable GUI
    setResizable (true, true);
    const float ratio = 450.0/ 400.0;
    setResizeLimits (450,  juce::roundToInt (450.0 / ratio),
                         900, juce::roundToInt (900.0 / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);
    setSize (450, 400);
    
    //Knobs
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT", inputKnob);
   // inputKnob.addListener(this);
    inputKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(inputKnob);
    
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveKnob);
  //  driveKnob.addListener(this);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(driveKnob);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixKnob);
  //  mixKnob.addListener(this);
    mixKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

    addAndMakeVisible(mixKnob);
    
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTPUT", outputKnob);
   // outputKnob.addListener(this);
    outputKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(outputKnob);
    
    //Labels
    inputLabel.setText ("IN", juce::dontSendNotification);
    inputLabel.setJustificationType(juce::Justification::centred);
    inputLabel.setColour(0x1000281, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    inputLabel.setFont (juce::Font (14.0f));
    inputLabel.setInterceptsMouseClicks(false, false);
    inputLabel.attachToComponent(&inputKnob, false);
    addAndMakeVisible (inputLabel);

    driveLabel.setText ("DRIVE", juce::dontSendNotification);
    driveLabel.setJustificationType(juce::Justification::centred);
    driveLabel.setColour(0x1000281, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    driveLabel.setFont (juce::Font (14.0f));
    driveLabel.setInterceptsMouseClicks(false, false);
    driveLabel.attachToComponent(&driveKnob, false);
    addAndMakeVisible (driveLabel);

    mixLabel.setText ("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.setColour(0x1000281, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    mixLabel.setFont (juce::Font (14.0f));
    mixLabel.setInterceptsMouseClicks(false, false);
    mixLabel.attachToComponent(&mixKnob, false);
    addAndMakeVisible (mixLabel);

    outputLabel.setText ("OUT", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::centred);
    outputLabel.setColour(0x1000281, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    outputLabel.setFont (juce::Font (14.0f));
    outputLabel.setInterceptsMouseClicks(false, false);
    outputLabel.attachToComponent(&outputKnob, false);
    addAndMakeVisible (outputLabel);
    
    title.setText ("F.W CLIPPER v1.0", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::left);
    title.setColour(0x1000281, juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));
    title.setFont (juce::Font (12.0f));
    title.setInterceptsMouseClicks(false, false);
    addAndMakeVisible (title);
    addAndMakeVisible(scopeComponent);
    addAndMakeVisible (line);
}

Dist0322AudioProcessorEditor::~Dist0322AudioProcessorEditor()
{
   
}

//==============================================================================
void Dist0322AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.08f, 1.0f));
   
    scopeComponent.repaint();
}

void Dist0322AudioProcessorEditor::resized()
{
    // child components
    juce::Rectangle<int> area = getLocalBounds();
    area.reduce(area.getWidth()* 0.05f, area.getHeight()* 0.1f);
    juce::Rectangle<int> lineArea = area.removeFromTop(area.getHeight()/4);
    juce::Rectangle<int> titleArea = lineArea.removeFromTop(lineArea.getHeight()/6);
    lineArea.reduce(lineArea.getWidth()* 0.05f, lineArea.getHeight()* 0.1f);
    juce::Rectangle<int> sliderArea = area.removeFromTop(area.getHeight()/2);
    sliderArea.reduce(sliderArea.getWidth()* 0.05f, sliderArea.getHeight()* 0.001f);
    juce::Rectangle<int> inputSliderArea = sliderArea.removeFromLeft(sliderArea.getWidth()/4);
    juce::Rectangle<int> driveSliderArea = sliderArea.removeFromLeft(sliderArea.getWidth()/3);
    juce::Rectangle<int> mixSliderArea = sliderArea.removeFromLeft(sliderArea.getWidth()/2);

    title.setBounds(titleArea);
    line.setBounds(lineArea);
    inputKnob.setBounds(inputSliderArea);
    driveKnob.setBounds(driveSliderArea);
    mixKnob.setBounds(mixSliderArea);
    outputKnob.setBounds(sliderArea);
    scopeComponent.setBounds(area);
    scopeComponent.repaint();

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

