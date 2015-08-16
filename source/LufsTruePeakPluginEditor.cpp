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

#include "LufsAudioProcessor.h"
#include "LufsTruePeakPluginEditor.h"
#include "ExportSettingsComponent.h"
#include "OptionsComponent.h"
#include "AboutComponent.h"

void DEBUGPLUGIN_output( const char * _text, ...);

//==============================================================================
LufsTruePeakPluginEditor::LufsTruePeakPluginEditor(LufsAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
    , m_momentaryComponent( "Momentary", COLOR_MOMENTARY, false )
    , m_shortTermComponent( "Short Term", COLOR_SHORTTERM, false )
    , m_integratedComponent( "Integrated", COLOR_INTEGRATED, false )
    , m_rangeComponent( "Range", COLOR_RANGE, false )
    , m_truePeakComponent( -42.f, 6.f ) // -18.f 6.f
    , m_chart( -42.f, 0.f )//-8.f )
    , m_internallyPaused( false )
    , m_momentaryThreshold( getProcessor()->m_settings.getUserSettings(), "MomentaryThreshold", -8.f)
    , m_shortTermThreshold( getProcessor()->m_settings.getUserSettings(), "ShortTermThreshold", -15.f)
    , m_integratedThreshold( getProcessor()->m_settings.getUserSettings(), "IntegratedThreshold", -23.f)
    , m_rangeThreshold( getProcessor()->m_settings.getUserSettings(), "RangeThreshold", 10.f)
    , m_truePeakThreshold( getProcessor()->m_settings.getUserSettings(), "TruePeakThreshold", -1.f)
{
    DEBUGPLUGIN_output("LufsTruePeakPluginEditor::LufsTruePeakPluginEditor ownerFilter 0x%x", ownerFilter);

    m_timeComponent.setTextColor( COLOR_LUFSTIME );
    addAndMakeVisible( &m_timeComponent );

    addAndMakeVisible( &m_momentaryComponent );
    addAndMakeVisible( &m_shortTermComponent );
    addAndMakeVisible( &m_integratedComponent );
    addAndMakeVisible( &m_rangeComponent );

    m_resetButton.setButtonText( juce::String( "Reset" ) );
    m_resetButton.addListener( this );
    m_resetButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_resetButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_resetButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_resetButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    addAndMakeVisible( &m_resetButton );

    m_pauseButton.setButtonText( juce::String( "Pause" ) );
    m_pauseButton.addListener( this );
    m_pauseButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_pauseButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_pauseButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_pauseButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    addAndMakeVisible( &m_pauseButton );

    m_exportButton.setButtonText( juce::String( "Export" ) );
    m_exportButton.addListener( this );
    m_exportButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_exportButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_exportButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_exportButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    addAndMakeVisible( &m_exportButton );

    m_aboutButton.setButtonText( juce::String( "About" ) );
    m_aboutButton.addListener( this );
    m_aboutButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_aboutButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_aboutButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_aboutButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    addAndMakeVisible( &m_aboutButton );

    m_optionsButton.setButtonText( juce::String( "Options" ) );
    m_optionsButton.addListener( this );
    m_optionsButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_optionsButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_optionsButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_optionsButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    addAndMakeVisible( &m_optionsButton );
    
    m_chart.setProcessor( getProcessor() );
    addAndMakeVisible( &m_chart );

    m_truePeakComponent.setProcessor( getProcessor() );
    addAndMakeVisible( &m_truePeakComponent );  

    setSize( LUFS_EDITOR_WIDTH, LUFS_EDITOR_HEIGHT );

    m_momentaryThreshold.addListener(&m_momentaryComponent);
    m_shortTermThreshold.addListener(&m_shortTermComponent);
    m_integratedThreshold.addListener(&m_integratedComponent);
    m_rangeThreshold.addListener(&m_rangeComponent);
    m_truePeakThreshold.addListener(&m_truePeakComponent.m_valueComponent);

    startTimer( 40 );
}

LufsTruePeakPluginEditor::~LufsTruePeakPluginEditor()
{
    DEBUGPLUGIN_output("LufsTruePeakPluginEditor::~LufsTruePeakPluginEditor");
}

//==============================================================================
void LufsTruePeakPluginEditor::paint (juce::Graphics& g)
{
    DEBUGPLUGIN_output("LufsTruePeakPluginEditor::paint");

    g.fillAll( LUFS_COLOR_BACKGROUND );

    juce::Font lufsFont( 36.f );
    lufsFont.setBold(true);
    g.setFont( lufsFont );
    g.setColour( LUFS_COLOR_FONT );

    int x = 10;
    const int width = 120;
    g.setColour( COLOR_LUFSTIME );
    g.drawFittedText( juce::String( "LUFS" ), x, 10, width, 40, juce::Justification::centred, 1, 0.01f );
    x += width;
}

void LufsTruePeakPluginEditor::resized()
{
    DEBUGPLUGIN_output("LufsTruePeakPluginEditor::resized width %d height %d", getWidth(), getHeight());

    int x = 10;
    const int width = 120;
    const int truePeakWidth = 120;
    m_timeComponent.setBounds( x, 48, width, 40 );
    x += width;
    m_momentaryComponent.setBounds( x, 0, width, 100 );
    x += width;
    m_shortTermComponent.setBounds( x, 0, width, 100 );
    x += width;
    m_integratedComponent.setBounds( x, 0, width, 100 );
    x += width;
    m_rangeComponent.setBounds( x, 0, width, 100 );
    x += width;

    const int imageX = 10;
    const int imageY = 100;
    const int imageWidth = getWidth() - 2 * imageX - truePeakWidth;
    const int imageHeight = getHeight() - 10 - imageY;
    m_chart.setBounds( imageX, imageY, imageWidth, imageHeight );

    const int buttonYOffset = 8;
    int buttonHeight = 30;
    int buttonY = 2 * buttonYOffset;
    m_resetButton.setBounds( x, buttonY, width / 2, buttonHeight );
    buttonY += ( buttonHeight + buttonYOffset );
    m_pauseButton.setBounds( x, buttonY, width / 2, buttonHeight );
    buttonY += ( buttonHeight + buttonYOffset );

    x += 10 + width / 2;
    buttonY = 2 * buttonYOffset;
    m_exportButton.setBounds( x, buttonY, width / 2, buttonHeight );
    buttonY += ( buttonHeight + buttonYOffset );
    m_optionsButton.setBounds( x, buttonY, width / 2, buttonHeight );
    
    x += 10 + width / 2;
    buttonY = 2 * buttonYOffset;
    m_aboutButton.setBounds( x, buttonY, width / 2, buttonHeight );

    m_chart.setBounds( imageX, imageY, imageWidth, imageHeight ); 
    m_truePeakComponent.setBounds( imageX + imageWidth, 0, truePeakWidth, imageHeight + imageY);
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void LufsTruePeakPluginEditor::timerCallback()
{
    if ( m_internallyPaused )
        return;

    //DEBUGPLUGIN_output("LufsTruePeakPluginEditor::timerCallback");
    LufsAudioProcessor* processor = getProcessor();

    processor->m_lufsProcessor.update();

    const int validSize = processor->m_lufsProcessor.getValidSize();

    if ( validSize )
    {
        const float momentary = processor->m_lufsProcessor.getMomentaryVolumeArray()[ validSize - 1 ];
        m_momentaryComponent.setVolume( momentary );

        const float shortTerm = processor->m_lufsProcessor.getShortTermVolumeArray()[ validSize - 1 ];
        m_shortTermComponent.setVolume( shortTerm );

        m_integratedComponent.setVolume( processor->m_lufsProcessor.getIntegratedVolume() );

        const float minRangeVolume = processor->m_lufsProcessor.getRangeMinVolume();
        const float maxRangeVolume = processor->m_lufsProcessor.getRangeMaxVolume();
        m_rangeComponent.setVolume( maxRangeVolume - minRangeVolume );
    }
    else
    {
        m_momentaryComponent.setVolume( DEFAULT_MIN_VOLUME );
        m_shortTermComponent.setVolume( DEFAULT_MIN_VOLUME );
        m_integratedComponent.setVolume( DEFAULT_MIN_VOLUME );
        m_rangeComponent.setVolume( 0.f );
        m_truePeakComponent.reset();
    }

    m_timeComponent.setSeconds( processor->m_lufsProcessor.getSeconds() );

    if ( !processor->m_lufsProcessor.isPaused() )
    {
        m_chart.update();
        m_truePeakComponent.update();
    }
}

void LufsTruePeakPluginEditor::buttonClicked (juce::Button* button)
{
    DEBUGPLUGIN_output("LufsTruePeakPluginEditor::buttonClicked");
    LufsAudioProcessor* processor = getProcessor();

    if ( button == &m_resetButton )
    {
        m_momentaryComponent.resetWarning();
        m_shortTermComponent.resetWarning();
        m_integratedComponent.resetWarning();
        m_rangeComponent.resetWarning();

        m_truePeakComponent.reset();
        processor->m_lufsProcessor.reset();

        m_chart.resetScrolling();
        
        repaint();
    }
    else if ( button == &m_pauseButton )
    {
        if ( processor->m_lufsProcessor.isPaused() )
        {
            m_truePeakComponent.resume();
            processor->m_lufsProcessor.resume();
            m_pauseButton.setButtonText( juce::String( "Pause" ) );
        }
        else
        {
            m_truePeakComponent.pause();
            processor->m_lufsProcessor.pause();
            m_pauseButton.setButtonText( juce::String( "Resume" ) );
        }
        
        repaint();
    }
    else if ( button == &m_exportButton )
    {
        const int updateSize = processor->m_lufsProcessor.getValidSize();
        if ( !updateSize )
        {
            juce::AlertWindow::showMessageBox( juce::AlertWindow::NoIcon, "Measurement has not yet started - there is no data to export.", "" );
            return;
        }

        m_internallyPaused = true;

        ExportSettingsComponent component( getProcessor()->m_settings );
        int result = juce::DialogWindow::showModalDialog( "Export volume figures to text file", &component, this, LUFS_COLOR_BACKGROUND, true, false, false );

        if ( result )
            exportToText( component.useCommas(), component.exportTruePeak() );
        
        m_internallyPaused = false;
    }
    else if ( button == &m_aboutButton )
    {
        AboutComponent component;
        juce::String about( "About " );
        about << LufsAudioProcessor::makeAppNameWithVersion();
        juce::DialogWindow::showModalDialog( about, &component, this, LUFS_COLOR_BACKGROUND, true, false, false );
    }
    else if ( button == &m_optionsButton )
    {
        OptionsComponent component( getProcessor()->m_settings, 
            m_momentaryThreshold, m_shortTermThreshold, m_integratedThreshold, 
            m_rangeThreshold, m_truePeakThreshold);

        juce::String about( "Options: threshold volumes for warnings" );
        juce::DialogWindow::showModalDialog( about, &component, this, juce::Colours::grey/*LUFS_COLOR_BACKGROUND*/, true, false, false );
    }
}

void LufsTruePeakPluginEditor::exportToText( bool useCommasForDigitSeparation, bool exportTruePeak )
{
    LufsAudioProcessor* processor = getProcessor();

    const int updateSize = processor->m_lufsProcessor.getValidSize();

    const juce::String saveDirString( "saveDirectory" );
    const juce::String saveDir = getProcessor()->m_settings.getUserSettings()->getValue( saveDirString );

    juce::File directory;
    if ( saveDir.length() )
        directory = juce::File( directory );
    if ( !directory.exists() )
        directory = juce::File::getSpecialLocation( juce::File::userHomeDirectory );

    juce::FileChooser fileChooser( "Select text output file", directory, "*.txt" );
    if ( fileChooser.browseForFileToSave( true ) )
    {
        juce::File file( fileChooser.getResult() );
        if ( ! file.deleteFile() )
        {
            juce::AlertWindow::showMessageBox( juce::AlertWindow::NoIcon, "Unable to delete existing file, not saving.", "" );
            return;
        }

        // save chosen directory
        getProcessor()->m_settings.getUserSettings()->getValue( saveDirString, file.getParentDirectory().getFullPathName() );

        juce::FileOutputStream outputStream( file );

        juce::String text = "Time\tMomentary\tShort Term\tIntegrated";
        
        if (exportTruePeak)
            text += "\tTrue Peak L\tTrue Peak R\tTrue Peak C\tTrue Peak Lfe\tTrue Peak Ls\tTrue Peak Rs";
            // kSpeakerArr51 is "L R C Lfe Ls Rs";
        text += "\n";

        int ten = 10;
        for ( int tens = 0; tens < updateSize - ten ; tens += ten )
        {
            // add time 
            const int seconds = tens / 10;
            const int hours = seconds / 3600;
            const int minutes = ( seconds - hours * 3600 ) / 60;
            int shownSeconds = seconds - hours * 3600 - minutes * 60;
            juce::String line( hours );
            line << ":";
            if ( minutes < 10 )
                line << "0";
            line << minutes;
            line << ":";
            if ( shownSeconds < 10 )
                line << "0";
            line << shownSeconds;
            line << "\t";

            // add momentary
            float momentary = DEFAULT_MIN_VOLUME;
            for ( int i = 0 ; i < ten ; ++i )
            {
                float val = processor->m_lufsProcessor.getMomentaryVolumeArray()[ tens + i ];
                if ( momentary < val  )
                    momentary = val;
            }
            line << juce::String( momentary, 1 );
            line << "\t";

            // add short term
            float shortTerm = DEFAULT_MIN_VOLUME;
            for ( int i = 0 ; i < ten ; ++i )
            {
                float val = processor->m_lufsProcessor.getShortTermVolumeArray()[ tens + i ];
                if ( shortTerm < val  )
                    shortTerm = val;
            }
            line << juce::String( shortTerm, 1 );
            line << "\t";

            // add integrated
            float integrated = DEFAULT_MIN_VOLUME;
            for ( int i = 0 ; i < ten ; ++i )
            {
                float val = processor->m_lufsProcessor.getIntegratedVolumeArray()[ tens + i ];
                if ( integrated < val  )
                    integrated = val;
            }
            line << juce::String( integrated, 1 );
            
            if ( exportTruePeak )
            {
                for ( int truePeakCh = 0 ; truePeakCh < LUFS_TP_MAX_NB_CHANNELS ; ++truePeakCh )
                {
                    line << "\t";
                    
                    float truePeakVolume = DEFAULT_MIN_VOLUME;
                    for ( int i = 0 ; i < ten ; ++i )
                    {
                        float val = processor->m_lufsProcessor.getTruePeakChannelArray(truePeakCh)[ tens + i ];
                        if ( truePeakVolume < val  )
                            truePeakVolume = val;
                    }
                    line << juce::String( truePeakVolume, 1 );
                }
            }
            
            line << "\n";

            if ( useCommasForDigitSeparation )
                line = line.replaceCharacter( '.', ',' );

            text << line;
        }

        outputStream.writeText( text, false, false );
    }
}
