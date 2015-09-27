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

#include "AudioDeviceManager.h"
#include "PatchComponent.h"
#include "Patch.h"

class AudioDeviceSelectorComponent 
    : public juce::Component
    , public juce::ComboBox::Listener
    , public juce::ChangeListener
    , public juce::AudioIODeviceCallback
    , public PatchComponent::Listener
{
public:

    AudioDeviceSelectorComponent(AudioDeviceManager & deviceManager, Patch & inputPatch);

    virtual ~AudioDeviceSelectorComponent();

    // juce::Component
    void paint(juce::Graphics & g) override;
    void resized() override;

    // juce::ComboBox::Listener
    void comboBoxChanged(juce::ComboBox * comboBoxThatHasChanged) override;

    // juce::ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster * source) override;

private:

    // juce::AudioIODeviceCallback
    void audioDeviceIOCallback(const float**, int, float**, int, int) override {}
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override {}

    // PatchComponent::Listener
    void patchHasChanged(const PatchComponent * component, const juce::BigInteger & activeLines, const juce::BigInteger & activeColumns) override;

    void updateContent();

    AudioDeviceManager & m_deviceManager;
    
    juce::StringArray m_deviceTypeArray;
    juce::StringArray m_deviceNameArray;

    juce::ComboBox m_audioTypeCombo;
    juce::Label m_audioTypeLabel;

    juce::ComboBox m_audioDeviceCombo;
    juce::Label m_audioDeviceLabel;

    juce::ComboBox m_samplingRatesCombo;
    juce::Label m_samplingRatesLabel;

    juce::ComboBox m_bufferSizesCombo;
    juce::Label m_bufferSizesLabel;

    PatchComponent m_patchComponent;
};

