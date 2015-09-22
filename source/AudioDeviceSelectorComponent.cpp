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

#include <atomic>

#include "AppIncsAndDefs.h"

#include "AudioDeviceSelectorComponent.h"

AudioDeviceSelectorComponent::AudioDeviceSelectorComponent(AudioDeviceManager & deviceManager, Patch & inputPatch)
    : m_deviceManager(deviceManager)
    , m_patchView(inputPatch, LUFS_COLOR_BACKGROUND, LUFS_COLOR_FONT, false)
{
    m_patchView.m_patchComponent.redraw();

    // colors for combo
    m_audioTypeCombo.setColour(juce::ComboBox::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_audioTypeCombo.setColour(juce::ComboBox::textColourId, LUFS_COLOR_FONT);
    m_audioDeviceCombo.setColour(juce::ComboBox::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_audioDeviceCombo.setColour(juce::ComboBox::textColourId, LUFS_COLOR_FONT);
    m_samplingRatesCombo.setColour(juce::ComboBox::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_samplingRatesCombo.setColour(juce::ComboBox::textColourId, LUFS_COLOR_FONT);
    m_bufferSizesCombo.setColour(juce::ComboBox::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_bufferSizesCombo.setColour(juce::ComboBox::textColourId, LUFS_COLOR_FONT);

    // colors for labels
    m_audioTypeLabel.setColour(juce::Label::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_audioTypeLabel.setColour(juce::Label::textColourId, LUFS_COLOR_FONT);
    m_audioDeviceLabel.setColour(juce::Label::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_audioDeviceLabel.setColour(juce::Label::textColourId, LUFS_COLOR_FONT);
    m_samplingRatesLabel.setColour(juce::Label::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_samplingRatesLabel.setColour(juce::Label::textColourId, LUFS_COLOR_FONT);
    m_bufferSizesLabel.setColour(juce::Label::backgroundColourId, LUFS_COLOR_BACKGROUND);
    m_bufferSizesLabel.setColour(juce::Label::textColourId, LUFS_COLOR_FONT);

    // audio device type
    const juce::OwnedArray<juce::AudioIODeviceType> & types = deviceManager.getAvailableDeviceTypes();

    for (int i = 0; i < types.size(); ++i)
    {
        m_audioTypeCombo.addItem (types.getUnchecked(i)->getTypeName(), i + 1);
        m_deviceTypeArray.add(types.getUnchecked(i)->getTypeName());
    }

    if (types.size() > 1)
    {
        addAndMakeVisible(&m_audioTypeCombo);
        m_audioTypeCombo.addListener (this);

        addAndMakeVisible(&m_audioTypeLabel);
        m_audioTypeLabel.setText("Audio type", juce::NotificationType::dontSendNotification);
        m_audioTypeLabel.setJustificationType(juce::Justification::centredRight);

        for (int i = 0; i < types.size(); ++i)
        {
            if (m_deviceManager.getCurrentAudioDeviceType() == m_deviceTypeArray[i])
                m_audioTypeCombo.setSelectedId(1 + i, juce::dontSendNotification);
        }
    }

    // audio device name
    addAndMakeVisible(&m_audioDeviceCombo);
    m_audioDeviceCombo.addListener (this);

    addAndMakeVisible(&m_audioDeviceLabel);
    m_audioDeviceLabel.setText("Audio device", juce::NotificationType::dontSendNotification);
    m_audioDeviceLabel.setJustificationType(juce::Justification::centredRight);

    // sampling rates
    addAndMakeVisible(&m_samplingRatesCombo);
    m_samplingRatesCombo.addListener (this);

    addAndMakeVisible(&m_samplingRatesLabel);
    m_samplingRatesLabel.setText("Sampling rate", juce::NotificationType::dontSendNotification);
    m_samplingRatesLabel.setJustificationType(juce::Justification::centredRight);

    // buffer sizes
    addAndMakeVisible(&m_bufferSizesCombo);
    m_bufferSizesCombo.addListener (this);

    addAndMakeVisible(&m_bufferSizesLabel);
    m_bufferSizesLabel.setText("Buffer size", juce::NotificationType::dontSendNotification);
    m_bufferSizesLabel.setJustificationType(juce::Justification::centredRight);


    m_deviceManager.addChangeListener(this);

    addAndMakeVisible(&m_patchView);
    m_patchView.m_patchComponent.addListener(this);

    updateContent();
}

AudioDeviceSelectorComponent::~AudioDeviceSelectorComponent()
{
}

void AudioDeviceSelectorComponent::paint(juce::Graphics & g)
{
    //g.fillAll(juce::Colours::red);

    if (!m_audioTypeCombo.getNumItems())
    {
        g.setColour(juce::Colours::red);
        juce::Font font( 18.f );
        font.setBold(true);
        g.setFont( font );
        g.drawFittedText( "Audio driver error: unable to find any audio device type.", 0, 60, getWidth(), 40, juce::Justification::centred, 1, 0.01f );

        return;
    }
}

void AudioDeviceSelectorComponent::resized()
{
    const int leftOffsetX = 20;
    const int leftWidth = getWidth() / 4 - leftOffsetX;
    
    const int offsetY = 10;
    const int height = 20;

    const int rightOffsetX = 2 * leftOffsetX + leftWidth;
    const int rightWidth = getWidth() - rightOffsetX - leftOffsetX;

    int y = offsetY;

    if (m_audioTypeCombo.getNumItems() > 1)
    {
        m_audioTypeLabel.setBounds(leftOffsetX, y, leftWidth, height);
        m_audioTypeCombo.setBounds(rightOffsetX, y, rightWidth, height);

        y += height + offsetY;
    }

    m_audioDeviceLabel.setBounds(leftOffsetX, y, leftWidth, height);
    m_audioDeviceCombo.setBounds(rightOffsetX, y, rightWidth, height);

    y += height + offsetY;
    
    m_samplingRatesLabel.setBounds(leftOffsetX, y, leftWidth, height);
    m_samplingRatesCombo.setBounds(rightOffsetX, y, rightWidth, height);

    y += height + offsetY;

    m_bufferSizesLabel.setBounds(leftOffsetX, y, leftWidth, height);
    m_bufferSizesCombo.setBounds(rightOffsetX, y, rightWidth, height);

    y += height + offsetY;

    m_patchView.setBounds(leftOffsetX, y, getWidth() - 2 * leftOffsetX, getHeight() - offsetY - y);
}

void AudioDeviceSelectorComponent::comboBoxChanged(juce::ComboBox * comboBoxThatHasChanged) 
{
    juce::AudioDeviceManager::AudioDeviceSetup config;
    m_deviceManager.getAudioDeviceSetup(config);
    config.useDefaultInputChannels = false;
    config.useDefaultOutputChannels = false;

    if (comboBoxThatHasChanged == &m_audioTypeCombo)
    {
        // change type
        m_deviceManager.setCurrentAudioDeviceType(m_deviceTypeArray[m_audioTypeCombo.getSelectedId() - 1], true);

        m_patchView.m_patchComponent.redraw();
    }
    else if (comboBoxThatHasChanged == &m_audioDeviceCombo)
    {
        config.inputDeviceName = m_deviceNameArray[m_audioDeviceCombo.getSelectedId() - 1];
        config.outputDeviceName = m_deviceNameArray[m_audioDeviceCombo.getSelectedId() - 1];

        juce::String error = m_deviceManager.setAudioDeviceSetup(config, true);

        if (error.isNotEmpty())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                              TRANS("Error when trying to open audio device!"),
                                              error);
        }

        m_patchView.m_patchComponent.redraw();
    }
    else if (comboBoxThatHasChanged == &m_samplingRatesCombo)
    {
        if (m_samplingRatesCombo.getSelectedId() > 0)
        {
            config.sampleRate = (double)m_samplingRatesCombo.getSelectedId();

            juce::String error = m_deviceManager.setAudioDeviceSetup(config, true);

            if (error.isNotEmpty())
            {
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                    TRANS("Error when trying to change audio device sampling rate!"),
                    error);
            }
        }
    }
    else if (comboBoxThatHasChanged == &m_bufferSizesCombo)
    {
        if (m_bufferSizesCombo.getSelectedId() > 0)
        {
            juce::String error = m_deviceManager.setAudioDeviceSetup(config, true);

            if (error.isNotEmpty())
            {
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                    TRANS("Error when trying to change audio device buffer size!"),
                    error);
            }
        }

    }

    updateContent();
}

void AudioDeviceSelectorComponent::changeListenerCallback(juce::ChangeBroadcaster *) 
{
    // m_deviceManager change
    updateContent();
}

void AudioDeviceSelectorComponent::updateContent()
{
    if (!m_audioTypeCombo.getNumItems())
        return;
    
    juce::AudioIODeviceType * type = nullptr;
    
    if (m_audioTypeCombo.getNumItems() > 1)
    {
        m_audioTypeCombo.setText(m_deviceManager.getCurrentAudioDeviceType(), juce::dontSendNotification);

        type = m_deviceManager.getAvailableDeviceTypes()[m_audioTypeCombo.getSelectedId() - 1];
    }
    else
    {
        type = m_deviceManager.getAvailableDeviceTypes()[0];
    }

    m_audioDeviceCombo.clear(juce::dontSendNotification);
    m_deviceNameArray.clear();

    juce::AudioIODevice * device = m_deviceManager.getCurrentAudioDevice();

    if (type != nullptr)
    {
        type->scanForDevices();

        const juce::StringArray deviceNames = type->getDeviceNames();
        for (int i = 0 ; i < deviceNames.size() ; ++i)
        {
            m_audioDeviceCombo.addItem(deviceNames[i], 1 + i);
            m_deviceNameArray.add(deviceNames[i]);
            
            if (device && device->getName() == deviceNames[i])
                m_audioDeviceCombo.setSelectedId(1 + i, juce::dontSendNotification);
        }
    }

    // sampling rate 
    m_samplingRatesCombo.clear(juce::dontSendNotification);
    if (device != nullptr)
    {
        const juce::Array<double> samplingRates(device->getAvailableSampleRates());

        for (int i = 0 ; i < samplingRates.size(); ++i)
        {
            double samplingRate = samplingRates.getUnchecked(i);
            m_samplingRatesCombo.addItem(juce::String(samplingRate / 1000.0, 1) + "kHz", juce::roundToInt(samplingRate));
        }

        m_samplingRatesCombo.setSelectedId(juce::roundToInt(device->getCurrentSampleRate()), juce::dontSendNotification);
    }

    // buffer size
    m_bufferSizesCombo.clear(juce::dontSendNotification);
    if (device != nullptr)
    {
        const juce::Array<int> bufferSizes(device->getAvailableBufferSizes());
        double currentSamplingRate = device->getCurrentSampleRate();
        if (currentSamplingRate == 0)
            currentSamplingRate = 48000.0;

        for (int i = 0 ; i < bufferSizes.size(); ++i)
        {
            int bufferSize = bufferSizes.getUnchecked(i);
            double duration = bufferSize * 1000.0 / currentSamplingRate;
            m_bufferSizesCombo.addItem(juce::String(bufferSize) + " samples (" + juce::String(duration, 1) + " ms)", bufferSize);
        }

        m_bufferSizesCombo.setSelectedId(juce::roundToInt(device->getCurrentBufferSizeSamples()), juce::dontSendNotification);
    }
}

class MyTask : public juce::ThreadWithProgressWindow
{
public:
    MyTask(std::atomic<bool> & flag)
        : juce::ThreadWithProgressWindow( "Connecting...", false, false )
        , m_flag( flag )
    {
    }

    void run()
    {
        while (m_flag.load())
            juce::Thread::sleep(5);
    }

    std::atomic<bool> & m_flag;
};

void AudioDeviceSelectorComponent::audioDeviceAboutToStart(juce::AudioIODevice * /*device*/) 
{
    m_patchView.m_patchComponent.redraw();
}

void AudioDeviceSelectorComponent::patchHasChanged(const PatchComponent * /*component*/, const juce::BigInteger & activeLines, const juce::BigInteger & /*activeColumns*/) 
{
    juce::AudioDeviceManager::AudioDeviceSetup config;
    m_deviceManager.getAudioDeviceSetup(config);

    config.useDefaultInputChannels = false;
    config.useDefaultOutputChannels = true;
    config.inputChannels = activeLines;

    // show window if slow
    std::atomic<bool> connecting = true;
    MyTask connectionWindow(connecting);
    connectionWindow.launchThread();

    juce::String error = m_deviceManager.setAudioDeviceSetup(config, true);

    connecting = false;

    if (error.isNotEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                            TRANS("Error when trying to open audio device!"),
                                            error);
    }

    while (connectionWindow.isThreadRunning())
        juce::Thread::sleep( 5 );
}
