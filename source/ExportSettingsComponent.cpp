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

#include "ExportSettingsComponent.h"

ExportSettingsComponent::ExportSettingsComponent( juce::ApplicationProperties & _settings )
    : m_settings( _settings )
    , m_useCommasString( "useCommasForDecimalMarkForDataExport" )
    , m_exportTruePeakString( "exportTruePeakValuesInDataExport" )
    , m_useCommas( true )
    , m_exportTruePeak( false )
{
    setSize( 500, 200 );

    m_okButton.setButtonText( juce::String( "OK" ) );
    m_okButton.addListener( this );
    m_okButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_okButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_okButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_okButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    m_okButton.setBounds( 320, 60, 100, 80 );
    addAndMakeVisible( &m_okButton );
        
    m_commaButton.setBounds( 240, 21, 20, 20 );
    m_commaButton.setRadioGroupId( 1 );
    m_commaButton.addListener( this );
    m_commaButton.setClickingTogglesState( true );
    addAndMakeVisible( &m_commaButton );

    m_pointButton.setBounds( 240, 60, 20, 20 );
    m_pointButton.setRadioGroupId( 1 );
    m_pointButton.addListener( this );
    m_pointButton.setClickingTogglesState( true );
    addAndMakeVisible( &m_pointButton );

    m_exportTruePeakButton.setBounds( 240, 121, 20, 20 );
    m_exportTruePeakButton.setRadioGroupId( 2 );
    m_exportTruePeakButton.addListener( this );
    m_exportTruePeakButton.setClickingTogglesState( true );
    addAndMakeVisible( &m_exportTruePeakButton );
    
    m_dontExportTruePeakButton.setBounds( 240, 160, 20, 20 );
    m_dontExportTruePeakButton.setRadioGroupId( 2 );
    m_dontExportTruePeakButton.addListener( this );
    m_dontExportTruePeakButton.setClickingTogglesState( true );
    addAndMakeVisible( &m_dontExportTruePeakButton );
    
    m_useCommas = m_settings.getUserSettings()->getBoolValue( m_useCommasString, "true" );

    m_exportTruePeak = m_settings.getUserSettings()->getBoolValue( m_exportTruePeakString, "false" );
    
    if ( m_useCommas )
    {
        m_commaButton.setToggleState( true, juce::dontSendNotification );
    }
    else
    {
        m_pointButton.setToggleState( true, juce::dontSendNotification );
    }
    
    if ( m_exportTruePeak )
    {
        m_exportTruePeakButton.setToggleState( true, juce::dontSendNotification );
    }
    else
    {
        m_dontExportTruePeakButton.setToggleState( true, juce::dontSendNotification );
    }
}

ExportSettingsComponent::~ExportSettingsComponent()
{
}

void ExportSettingsComponent::paint( juce::Graphics & g )
{
    g.fillAll( LUFS_COLOR_BACKGROUND );

    juce::Font font( 16.f );
    font.setBold(true);
    g.setFont (font);
    g.setColour( LUFS_COLOR_FONT );

    g.drawFittedText( "Use decimal comma", 20, 20, 210, 20, juce::Justification::centredRight, 1, 0.01f );
    g.drawFittedText( "Use decimal point", 20, 60, 210, 20, juce::Justification::centredRight, 1, 0.01f );
    
    g.drawFittedText( "Export True Peak values", 20, 120, 210, 20, juce::Justification::centredRight, 1, 0.01f );
    g.drawFittedText( "Dont export True Peak values", 20, 160, 210, 20, juce::Justification::centredRight, 1, 0.01f );
}

void ExportSettingsComponent::buttonClicked( juce::Button * _button ) 
{
    if ( _button == &m_okButton )
    {
        juce::DialogWindow * window = findParentComponentOfClass<juce::DialogWindow>();

        if ( window != nullptr )
            window->exitModalState( 51 );
    }
    else if ( _button == &m_commaButton || _button == &m_pointButton )
    {
        m_useCommas = m_commaButton.getToggleState();
        m_settings.getUserSettings()->setValue( m_useCommasString, m_useCommas ? 1 : 0 );
    }
    else if ( _button == &m_exportTruePeakButton || _button == &m_dontExportTruePeakButton )
    {
        m_exportTruePeak = m_exportTruePeakButton.getToggleState();
        m_settings.getUserSettings()->setValue( m_exportTruePeakString, m_exportTruePeak ? 1 : 0 );
    }
}
