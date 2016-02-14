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

#include "LufsTruePeakComponent.h"

#include "AudioDeviceSelectorComponent.h"
#include "LufsTruePeakPluginEditor.h"

const int lufsYPos = 40;

LufsTruePeakComponent::LufsTruePeakComponent( bool _hostAppContext )
    : m_audioConfigString( "AudioConfiguration" )
    , m_inputPatchString( "InputPatch" )
    , m_hostAppContext( _hostAppContext )
 {
    const juce::XmlElement * audioConfiguration = m_processor.m_settings.getUserSettings()->getXmlValue( m_audioConfigString );

    const char * names[] = 
    {
        "Left", "Right", "Center", "Low frequency effects", "Left surround (rear left)",
        "Right surround (rear right)" 
    };
    juce::StringArray columnNames(names, 6);
    m_patch.setColumnNames(columnNames);
    
    m_deviceManager.initialise(6, 2/*0 messes up with ASIO in juce*/, audioConfiguration, false, juce::String::empty);

    if (audioConfiguration != nullptr)
    {
        const juce::XmlElement * patch = audioConfiguration->getChildByName(m_inputPatchString);

        if (patch != nullptr && m_deviceManager.getCurrentAudioDevice() != nullptr)
        {
            m_patch.initFromXml(patch);

            // fake audioDeviceAboutToStart so patch gets initialized
            m_patch.audioDeviceAboutToStart(m_deviceManager.getCurrentAudioDevice());

            juce::String deviceType = audioConfiguration->getStringAttribute("deviceType", "");
            juce::String deviceName = audioConfiguration->getStringAttribute("audioOutputDeviceName", "");

            juce::AudioDeviceManager::AudioDeviceSetup config;
            m_deviceManager.getAudioDeviceSetup(config);
            config.inputChannels = m_patch.getActiveLines(deviceType, deviceName);

            m_deviceManager.setAudioDeviceSetup(config, true);
        }

        delete audioConfiguration;
    }

    m_deviceManager.addAudioCallback( this );

    juce::Component * lufsEditor = m_processor.createEditorIfNeeded();
    addAndMakeVisible( lufsEditor );

    const int componentWidth = lufsEditor->getWidth(); 
    const int componentHeight = lufsEditor->getHeight() + lufsYPos + 50;
    setSize( componentWidth, componentHeight );

    const int offsetX = 10;
    const int offsetY = 10;

    const int buttonWidth = 180;
    const int buttonHeight = 30;
    addAndMakeVisible( &m_audioDeviceButton );
    m_audioDeviceButton.setBounds( offsetX, offsetY, buttonWidth, buttonHeight );
    m_audioDeviceButton.setButtonText( "Configure Audio Inputs");
    m_audioDeviceButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_audioDeviceButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_audioDeviceButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_audioDeviceButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    m_audioDeviceButton.addListener( this );

    addAndMakeVisible( &m_audioDeviceSettingsLabel );
    m_audioDeviceSettingsLabel.setBounds( 2 * offsetX + buttonWidth, offsetY, componentWidth - 2 * offsetX + buttonWidth, buttonHeight );
    m_audioDeviceSettingsLabel.setColour( juce::Label::backgroundColourId, LUFS_COLOR_BACKGROUND );
    m_audioDeviceSettingsLabel.setColour( juce::Label::textColourId, LUFS_COLOR_FONT );

    updateAudioDeviceName();
}

LufsTruePeakComponent::~LufsTruePeakComponent()
{
    m_deviceManager.removeAudioCallback( this );

    juce::XmlElement * audioConfiguration = m_deviceManager.createStateXml();
    if ( audioConfiguration != nullptr )
    {
        // add patch
        juce::XmlElement * inputPatch = m_patch.createStateXml(m_inputPatchString);
        if (inputPatch != nullptr)
            audioConfiguration->addChildElement(inputPatch);

        m_processor.m_settings.getUserSettings()->setValue( m_audioConfigString, audioConfiguration );
        delete audioConfiguration;

        audioConfiguration = nullptr;
    }

    juce::AudioProcessorEditor * lufsEditor = m_processor.getActiveEditor();

    if ( lufsEditor )
    {
        if ( m_hostAppContext )
            m_processor.editorBeingDeleted( lufsEditor );

        delete lufsEditor;
    }

    juce::Thread::sleep( 100 );
}

void LufsTruePeakComponent::paint( juce::Graphics & /*g*/ )
{
#ifdef TESTCOLORS
    g.fillAll( juce::Colours::orange );
#endif

//    g.fillAll( LUFS_COLOR_BACKGROUND );
}

void LufsTruePeakComponent::updateAudioDeviceName()
{
    if ( m_deviceManager.getCurrentAudioDevice() )
    {
        juce::String text = m_deviceManager.getCurrentAudioDevice()->getName();
        text += juce::String( " using " );
        text += m_deviceManager.getCurrentAudioDeviceType();
        text += juce::String( " drivers" );
        m_audioDeviceSettingsLabel.setText( text, juce::dontSendNotification );
    }
    else
    {
        m_audioDeviceSettingsLabel.setText( "NOT USING ANY AUDIO DEVICE - operation is disabled", juce::dontSendNotification );
    }
}

void LufsTruePeakComponent::buttonClicked( juce::Button* )
{
    AudioDeviceSelectorComponent component(m_deviceManager, m_patch);

    component.setSize( 700, 600 );
    juce::String selectAudioDevice("Configure Audio Inputs");

    juce::DialogWindow::showModalDialog(selectAudioDevice, &component, this, LUFS_COLOR_BACKGROUND, true, true, true);

    updateAudioDeviceName();
}

void LufsTruePeakComponent::audioDeviceIOCallback( const float** inputChannelData, int /*numInputChannels*/, float** outputChannelData, int numOutputChannels, int numSamples)
{
    juce::AudioSampleBuffer buffer = m_patch.getBuffer( (float**)inputChannelData, numSamples );

    juce::MidiBuffer emptyMidiBuffer;
    m_processor.processBlock( buffer, emptyMidiBuffer );

     // zero outputs
    for (int i = 0 ; i < numOutputChannels ; ++i)
        memset(outputChannelData[i], 0, numSamples * sizeof(float));
}

void LufsTruePeakComponent::audioDeviceAboutToStart( juce::AudioIODevice* device )
{
    m_patch.audioDeviceAboutToStart( device );

    m_processor.prepareToPlay( device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples() );
}

void LufsTruePeakComponent::audioDeviceStopped()
{
}

void LufsTruePeakComponent::audioDeviceError( const juce::String & /*errorMessage*/ )
{
}

void LufsTruePeakComponent::resized()
{
    juce::Component * lufsEditor = m_processor.getActiveEditor();

    if ( lufsEditor )
        lufsEditor->setBounds( 0, lufsYPos, getWidth(), getHeight() - lufsYPos );
}

