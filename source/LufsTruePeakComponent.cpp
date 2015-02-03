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

#include "LufsTruePeakPluginEditor.h"

const int lufsYPos = 40;


LufsTruePeakComponent::LufsTruePeakComponent( bool _hostAppContext )
    : m_hostAppContext( _hostAppContext )
    , m_audioConfigString( "AudioConfig" )
{
    const juce::XmlElement * audioConfiguration = m_processor.m_settings.getUserSettings()->getXmlValue( m_audioConfigString );
    m_audioDevice.initialise( 6, 6, audioConfiguration, false, juce::String::empty, 0 );
    delete audioConfiguration;

    m_audioDevice.addAudioCallback( this );

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
    m_audioDeviceButton.setButtonText( "Select the Audio Device");
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

    const char * inputChannelNames[] = { "Left", "Right", "Center", "Low Freq Effects", "Left Surround", "Right Surround" };
    m_inputChannelNames = juce::StringArray( inputChannelNames, 6 );
}

LufsTruePeakComponent::~LufsTruePeakComponent()
{
    juce::AudioProcessorEditor * lufsEditor = m_processor.getActiveEditor();

    if ( lufsEditor )
    {
        if ( m_hostAppContext )
            m_processor.editorBeingDeleted( lufsEditor );

        delete lufsEditor;
    }

    juce::Thread::sleep( 100 );

    const juce::XmlElement * audioConfiguration = m_audioDevice.createStateXml();
    m_processor.m_settings.getUserSettings()->setValue( m_audioConfigString, audioConfiguration );
    delete audioConfiguration;
}

void LufsTruePeakComponent::paint( juce::Graphics & g )
{
#ifdef TESTCOLORS
    g.fillAll( juce::Colours::orange );
#endif

    g.fillAll( LUFS_COLOR_BACKGROUND );

}

void LufsTruePeakComponent::updateAudioDeviceName()
{
    if ( m_audioDevice.getCurrentAudioDevice() )
    {
        juce::String text = m_audioDevice.getCurrentAudioDevice()->getName();
        text += juce::String( " using " );
        text += m_audioDevice.getCurrentAudioDeviceType();
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
    const bool wasPaused = m_processor.m_lufsProcessor.isPaused();
    if ( !wasPaused )
        m_processor.m_lufsProcessor.pause();

    juce::AudioDeviceSelectorComponent component( m_audioDevice, 
        LUFS_TP_MAX_NB_CHANNELS, LUFS_TP_MAX_NB_CHANNELS, 
        LUFS_TP_MAX_NB_CHANNELS, LUFS_TP_MAX_NB_CHANNELS, 
        false, false, false, false );

    component.setSize( 600, 400 );
    juce::String selectAudioDevice( "Select the Audio Device (channels order is L R C Lfe Ls Rs)" );

    juce::DialogWindow::showModalDialog( selectAudioDevice, &component, this, LUFS_COLOR_FONT, true, false, false );

    updateAudioDeviceName();

    if ( !wasPaused )
        m_processor.m_lufsProcessor.resume();
}

void LufsTruePeakComponent::audioDeviceIOCallback( const float** inputChannelData, int numInputChannels, float** /*outputChannelData*/, int /*numOutputChannels*/, int numSamples)
{
    float const * floatArray[ 6 ] = { 0 };
    const int nbChannels = juce::jmin( 6, numInputChannels );
    for ( int i = 0 ; i < nbChannels; ++i )
        floatArray[ i ] = inputChannelData[ i ];
    juce::AudioSampleBuffer buffer( (float* const*) floatArray, nbChannels, numSamples );

    juce::MidiBuffer emptyMidiBuffer;
    m_processor.processBlock( buffer, emptyMidiBuffer );
}

void LufsTruePeakComponent::audioDeviceAboutToStart( juce::AudioIODevice* device )
{
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

