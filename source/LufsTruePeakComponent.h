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

#include "LufsAudioProcessor.h"

class LufsTruePeakComponent 
    : public juce::Component
    , public juce::Button::Listener
    , public juce::AudioIODeviceCallback
{
public:

    LufsTruePeakComponent( bool _hostAppContext );
    virtual ~LufsTruePeakComponent();

    // juce::Component
    virtual void paint( juce::Graphics & g );
    virtual void resized();

    // juce::Button::Listener
    virtual void buttonClicked( juce::Button* );

    // juce::AudioIODeviceCallback 
    virtual void audioDeviceIOCallback( const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples);
    virtual void audioDeviceAboutToStart( juce::AudioIODevice* device );
    virtual void audioDeviceStopped();
    virtual void audioDeviceError( const juce::String & errorMessage );

    LufsAudioProcessor * getProcessor() { return &m_processor; }

private:

    void updateAudioDeviceName();
    
    LufsAudioProcessor m_processor;

    // audio manager
    juce::AudioDeviceManager m_audioDevice;
    juce::Label m_audioDeviceSettingsLabel;
    juce::TextButton m_audioDeviceButton;

    juce::StringArray m_inputChannelNames;

    juce::String m_audioConfigString;
    
    bool m_hostAppContext; // should be false when component is used in a plug
};



