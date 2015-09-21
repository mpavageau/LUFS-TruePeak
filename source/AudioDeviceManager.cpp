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

#include "AppIncsAndDefs.h"

#include "AudioDeviceManager.h"

AudioDeviceManager::AudioDeviceManager()
     : m_numInputChannelsNeeded(0)
     , m_numOutputChannelsNeeded(0)
     , m_selectDefaultDeviceOnFailure(false)
     , m_opened(false)
{
}

AudioDeviceManager::~AudioDeviceManager()
{
}

juce::String AudioDeviceManager::initialise(int numInputChannelsNeeded,
                         int numOutputChannelsNeeded,
                         const juce::XmlElement* savedState,
                         bool selectDefaultDeviceOnFailure,
                         const juce::String& preferredDefaultDeviceName,
                         const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions)
{
    m_numInputChannelsNeeded = numInputChannelsNeeded;
    m_numOutputChannelsNeeded = numOutputChannelsNeeded;
    m_selectDefaultDeviceOnFailure = selectDefaultDeviceOnFailure;
    m_preferredDefaultDeviceName = preferredDefaultDeviceName;

    juce::String ret = m_deviceManager.initialise(numInputChannelsNeeded, numOutputChannelsNeeded, savedState,
        selectDefaultDeviceOnFailure, preferredDefaultDeviceName, preferredSetupOptions);

    if (ret == juce::String::empty)
        m_opened = true;

    return ret;
}

void AudioDeviceManager::closeAudioDevice()
{
    m_deviceManager.closeAudioDevice();

    m_opened = false;
}

juce::String AudioDeviceManager::setAudioDeviceSetup(const juce::AudioDeviceManager::AudioDeviceSetup & newSetup, bool /*treatAsChosenDevice*/) 
{ 
    if (!m_opened)
        return "AudioDeviceManager not yet opened";

    m_deviceManager.closeAudioDevice();

    juce::String ret = m_deviceManager.initialise(m_numInputChannelsNeeded, m_numOutputChannelsNeeded, nullptr/*xml*/,
        m_selectDefaultDeviceOnFailure, m_preferredDefaultDeviceName, &newSetup);

    m_deviceManager.setAudioDeviceSetup(newSetup, true);

    return ret;
}
