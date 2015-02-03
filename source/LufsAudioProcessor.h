/*
  =================================================================

  This file is part of the LUFS-TruePeak program.
  Copyright (c) 2015 - Mathieu Pavageau - contact@repetito.com

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  Details of these licenses can be found at: www.gnu.org/licenses

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  =================================================================
*/

#pragma once 

#include "LufsProcessor.h"

//==============================================================================
/**
*/
class LufsAudioProcessor  : public juce::AudioProcessor
{
public:

    // Shared static
    static juce::String makeAppNameWithVersion();

    //==============================================================================
    LufsAudioProcessor();
    ~LufsAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages);
    void reset();

    //==============================================================================
    bool hasEditor() const                  { return true; }
    juce::AudioProcessorEditor* createEditor();

    //==============================================================================
    const juce::String getName() const            { return JucePlugin_Name; }

    int getNumParameters();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    const juce::String getParameterName (int index);
    const juce::String getParameterText (int index);

    const juce::String getInputChannelName (int channelIndex) const;
    const juce::String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms()                                                { return 0; }
    int getCurrentProgram()                                             { return 0; }
    void setCurrentProgram (int /*index*/)                              { }
    const juce::String getProgramName (int /*index*/)                         { return juce::String::empty; }
    void changeProgramName (int /*index*/, const juce::String& /*newName*/)   { }

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

    LufsProcessor m_lufsProcessor;
    juce::ApplicationProperties m_settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LufsAudioProcessor)
};

