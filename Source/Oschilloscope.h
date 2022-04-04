/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/


#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <array>


//Template class - holds sample data to be displayed in the scope
template <typename SampleType>
class AudioBufferQueue
{
public:
    
    // constants
    static constexpr size_t order = 9;
    static constexpr size_t bufferSize = 1U << order;
    static constexpr size_t numBuffers = 5;

    //Fifo
    void push(const SampleType* dataToPush, size_t numSamples)
    {
       
        jassert(numSamples <= bufferSize);
        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0) {
            juce::FloatVectorOperations::copy(buffers[(size_t)start1].data(), dataToPush, (int)juce::jmin(bufferSize, numSamples));
        }
        
        abstractFifo.finishedWrite(size1);
      
    }

    void pop(SampleType* outputBuffer)
    {
       
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(1, start1, size1, start2, size2);

        jassert(size1 <= 1);
        jassert(size2 == 0);

        if (size1 > 0) {
            juce::FloatVectorOperations::copy(outputBuffer, buffers[(size_t)start1].data(), (int)bufferSize);
        }
        abstractFifo.finishedRead(size1);
        
    }

private:
    //container to hold audio buffer for plotting, passing the num of elements to Fifo
    std::array <std::array<SampleType, bufferSize>, numBuffers> buffers;
    juce::AbstractFifo abstractFifo{ numBuffers };

};

//==============================================================================
// A class that collects sample data from the DSP audio buffer and adds the sample data to the AudioBufferQueue object.
template<typename SampleType>
class ScopeDataCollector
{
public:
    ScopeDataCollector(AudioBufferQueue<SampleType>& queueToUse)
        : audioBufferQueue(queueToUse)
    {}

    void process(const SampleType* data, size_t numSamples)
    {
        size_t index = 0;
            
        if (currentState == State::WaitingForTrigger)
        {
            while (index++ < numSamples)
            {
                auto currentSample = *data++;
              
                if (currentSample >= triggerLevel && prevSample < triggerLevel)
                {
                    //change here to reset the line
                    numCollected = 0;
                    currentState = State::Collecting;
                    break;
                }
             
                prevSample = currentSample;

            }
        }

        if (currentState == State::Collecting)
        {
            while (index++ < numSamples)
            {
               
                buffer[numCollected++] = *data++;

                if (numCollected == buffer.size())
                {
                    audioBufferQueue.push(buffer.data(), buffer.size());
                    currentState = State::WaitingForTrigger;
                    prevSample = SampleType(100);
                    break;
                }
            }
        }
    }

private:
   
    enum class State
    {
        WaitingForTrigger,
        Collecting
    };

    static constexpr auto triggerLevel = SampleType(0.001);

    std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;
    State currentState{ State::WaitingForTrigger };
    AudioBufferQueue<SampleType>& audioBufferQueue;
    size_t numCollected;
    SampleType prevSample = SampleType(100);
};

// A class of GUI components that plots and draws sample data stored in the AudioBufferQueue object.
// Inheriting classes: juce :: Component class, juce :: Timer class

template<typename SampleType>
class ScopeComponent : public juce::Component, private juce::Timer
{
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent (Queue& queueToUse)
        : audioBufferQueue (queueToUse)
    {
        sampleData.fill (SampleType (0));
        setFramesPerSecond (24);
    }

    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
       
    }

    //==============================================================================
    void paint(juce::Graphics& g) override
    {
        int panelNameHeight = 20;
        
        // area for the plotting
        juce::Rectangle<int> drawArea = getLocalBounds();
        drawArea.removeFromTop(panelNameHeight);
        drawArea.reduce(drawArea.getWidth()* 0.05f, drawArea.getHeight()* 0.01f);

        // background colour
        g.setColour(juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.08f, 1.0f));
        g.fillRect(drawArea);

        SampleType drawX = (SampleType)drawArea.getX();
        SampleType drawY = (SampleType)drawArea.getY();
        SampleType drawH = (SampleType)drawArea.getHeight();
        SampleType drawW = (SampleType)drawArea.getWidth();
        juce::Rectangle<SampleType> scopeRect = juce::Rectangle<SampleType>{ drawX, drawY, drawW, drawH };

        // colour of waveform
        g.setColour(juce::Colour::fromFloatRGBA (0.96f, 1.0f, 0.89f, 1.0f));

        // call the plot function
        plot(sampleData.data(), sampleData.size(), g, scopeRect, SampleType(0.4), scopeRect.getHeight() / 2);
    }

    void resized() override {}

private:
    void timerCallback() override
    {
        audioBufferQueue.pop(sampleData.data());
        repaint();
    }

    static void plot(const SampleType* data
        , size_t numSamples
        , juce::Graphics& g
        , juce::Rectangle<SampleType> rect
        , SampleType scaler = SampleType(1)
        , SampleType offset = SampleType(0))
    {
        auto w = rect.getWidth();
        auto h = rect.getHeight();
        auto right = rect.getRight();
        auto alignedCentre = rect.getBottom() - offset;
        auto gain = h * scaler;

        for (size_t i = 1; i < numSamples; ++i)
        {
            
            const float x1 = juce::jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
            const float y1 = alignedCentre - gain * juce::jmax(SampleType(-1.0), juce::jmin(SampleType(1.0), data[i - 1] ));
            const float x2 = juce::jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
            const float y2 = alignedCentre - gain * juce::jmax(SampleType(-1.0), juce::jmin(SampleType(1.0), data[i]));
            const float t = 1.0f;
            g.drawLine(x1, y1, x2, y2, t);
        }
        
    }
   
    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};
