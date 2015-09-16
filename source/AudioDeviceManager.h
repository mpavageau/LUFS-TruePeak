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

// juce::AudioDeviceManager wrapper so setCurrentAudioDeviceType calls audioDeviceAboutToStart

class AudioDeviceManager
{
public:

    AudioDeviceManager();

    virtual ~AudioDeviceManager();

    juce::String initialise (int numInputChannelsNeeded,
        int numOutputChannelsNeeded,
        const juce::XmlElement* savedState,
        bool selectDefaultDeviceOnFailure,
        const juce::String& preferredDefaultDeviceName = juce::String(),
        const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr);

    void closeAudioDevice();

    void addAudioCallback(juce::AudioIODeviceCallback* newCallback) { m_deviceManager.addAudioCallback(newCallback); }

    void removeAudioCallback(juce::AudioIODeviceCallback* callback) { m_deviceManager.removeAudioCallback(callback); }

    juce::XmlElement * createStateXml() const { return m_deviceManager.createStateXml(); }

    juce::AudioIODevice * getCurrentAudioDevice() const { return m_deviceManager.getCurrentAudioDevice(); }

    juce::String getCurrentAudioDeviceType() const { return m_deviceManager.getCurrentAudioDeviceType(); }

    const juce::OwnedArray<juce::AudioIODeviceType>& getAvailableDeviceTypes() { return m_deviceManager.getAvailableDeviceTypes(); }

    void setCurrentAudioDeviceType(const juce::String& type, bool treatAsChosenDevice) { m_deviceManager.setCurrentAudioDeviceType(type, treatAsChosenDevice); }

    void getAudioDeviceSetup(juce::AudioDeviceManager::AudioDeviceSetup & result) { m_deviceManager.getAudioDeviceSetup(result); }

    juce::String setAudioDeviceSetup(const juce::AudioDeviceManager::AudioDeviceSetup & newSetup, bool treatAsChosenDevice);

    // juce::ChangeBroadcaster
    void addChangeListener(juce::ChangeListener* listener) { m_deviceManager.addChangeListener(listener); }
    void removeChangeListener(juce::ChangeListener* listener) { m_deviceManager.removeChangeListener(listener); }

private:

    juce::AudioDeviceManager m_deviceManager;

    int m_numInputChannelsNeeded;
    int m_numOutputChannelsNeeded;
    bool m_selectDefaultDeviceOnFailure;
    juce::String m_preferredDefaultDeviceName;
 
    bool m_opened;
};

