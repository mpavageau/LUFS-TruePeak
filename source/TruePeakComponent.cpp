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

#include "TruePeakComponent.h"
#include "LufsAudioProcessor.h"

TruePeakComponent::TruePeakComponent( float minChartVolume, float maxChartVolume )
    : m_processor( nullptr )
    , m_validSize( 0 )
    , m_minChartVolume( minChartVolume )
    , m_maxChartVolume( maxChartVolume )
    , m_truePeakPassedLimit( false )
{
    m_valueComponent.setTextColor( COLOR_LUFSTIME );
    addAndMakeVisible( &m_valueComponent );

    m_valueComponent.setBounds( 10, 10, 100, 40 );
    
    resetVolumeInertia();
    
    // kSpeakerArr51 is "L R C Lfe Ls Rs";
    m_channelNames.add( "L" );
    m_channelNames.add( "R" );
    m_channelNames.add( "C" );
    m_channelNames.add( "Lfe" );
    m_channelNames.add( "Ls" );
    m_channelNames.add( "Rs" );
}

void DEBUGoutput( const char * _text, ...)
{
#if defined (LUFS_TRUEPEAK_WINDOWS)
    char formattedText[0x200];
    va_list args ;
    va_start( args, _text );

    vsprintf_s( formattedText, _text, args );
    va_end (args) ;

    // show time 
    char formattedTextWithTime[0x200];
    static juce::Time timeBeginning = juce::Time::getCurrentTime();
    juce::RelativeTime duration = juce::Time::getCurrentTime() - timeBeginning; 

    juce::Thread::ThreadID threadID = juce::Thread::getCurrentThreadId();
    sprintf_s( formattedTextWithTime, 0x200, "(%.3f) (thread 0x%x) %s", 0.001f * (float)duration.inMilliseconds(), (int)threadID, formattedText);
    DBG( formattedTextWithTime );
#else
    _text; // remove warning
#endif
}


void TruePeakComponent::paint( juce::Graphics & g )
{
    //g.fillAll( juce::Colours::red );

    juce::Font titleFont( 18.f );
    titleFont.setBold(true);
    g.setFont( titleFont );

    const int offsetX = 3;
    const int width = ( getWidth() - 7 * offsetX ) / 6;
    const int offsetY = 100;
    const int height = getHeight() - offsetY - 35;
    const int offsetText = 21;
    
    if ( m_truePeakPassedLimit )
    {
        const int frameOffset = 3;
        const int frameWidth = 5;
        g.setColour( juce::Colours::red );
        g.fillRect( frameOffset, frameOffset, getWidth() - 2 * frameOffset, frameWidth );
        g.fillRect( frameOffset, offsetY - frameOffset - frameWidth, getWidth() - 2 * frameOffset, frameWidth );
        g.fillRect( frameOffset, frameOffset, frameWidth, offsetY - 2 * frameOffset );
        g.fillRect( getWidth() - frameOffset - frameWidth, frameOffset, frameWidth, offsetY - 2 * frameOffset );
    }
    
    g.setColour( COLOR_LUFSTIME );
    g.drawFittedText( juce::String( "True Peak" ), 0, 60, getWidth(), 20, juce::Justification::centred, 1, 0.01f );

    g.setColour( COLOR_LUFSTIME );
    if ( !m_validSize )
        return;

    const int currentIndex = m_validSize - 1;

    const juce::Colour colorCurrent = COLOR_BACKGROUND_GRAPH;
    const juce::Colour colorMax = juce::Colours::white;

    g.setColour( LUFS_COLOR_BACKGROUND );

    int x = 2 * offsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        g.fillRect( x, offsetY, width, height );

        x += offsetX + width;
    }

    g.setColour( colorCurrent );
    
    x = 2 * offsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        float currentDecibels = m_processor->m_lufsProcessor.getTruePeakChannelArray(ch)[currentIndex];
        float inertiaVolumeDecibels = m_channelInertiaStruct[ch].getCurrentVolume(currentIndex);
        
        float uiVolumeDecibels = inertiaVolumeDecibels;
        
        if (currentDecibels > inertiaVolumeDecibels)
        {
            uiVolumeDecibels = currentDecibels;
            
            //store
            m_channelInertiaStruct[ch].m_index = currentIndex;
            m_channelInertiaStruct[ch].m_decibelVolume = currentDecibels;
        }
        
        int y = getVolumeY( offsetText, height, uiVolumeDecibels);
        //DEBUGoutput( "currentIndex %d currentDecibel: %.3f -> y %d", currentIndex, currentDecibel, y);
        g.fillRect( x, offsetY + y, width, height - y);

        x += offsetX + width;
    }

    g.setColour( colorMax );

    juce::Font figureFont( 12.f );
    figureFont.setBold(true);
    g.setFont( figureFont );
    
    x = 2 * offsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        float maxDecibel = m_processor->m_lufsProcessor.getTruePeakChannelMax(ch);
        int y = getVolumeY( offsetText, height, maxDecibel);
        g.fillRect( x, offsetY + y, width, 1);
        
        g.fillRect( x, offsetY + height, width, 1);
        
        const juce::String text = maxDecibel > -100.f ? juce::String(maxDecibel, 1) : juce::String((int)maxDecibel);
        g.drawFittedText( text, x, offsetY + y - 21, width, 20, juce::Justification::centred, 1, 0.01f );
        
        g.drawFittedText( m_channelNames[ch], x, offsetY + height + 3, width, 20, juce::Justification::centred, 1, 0.01f );
        

        x += offsetX + width;
    }

    /*
    int index = m_validSize - 1;
    juce::Font font( 14.f );
    font.setBold(true);
    g.setFont( font );

    juce::String value( m_processor->m_lufsProcessor.getTruePeakArray()[ index ] );
    g.drawFittedText( value, 10, 60, 120, 20, juce::Justification::centred, 1, 0.01f );*/
}

void TruePeakComponent::update()
{
    m_validSize = m_processor->m_lufsProcessor.getValidSize();

    if ( m_validSize )
    {
        float truePeak = m_processor->m_lufsProcessor.getTruePeak();
        
        m_valueComponent.setVolume( truePeak );
        
        m_truePeakPassedLimit = truePeak >= -1.f;
    }

    repaint();
}

void TruePeakComponent::reset()
{
    m_validSize = 0;
    m_valueComponent.setVolume( DEFAULT_MIN_VOLUME );

    repaint();
    
    resetVolumeInertia();
}

int TruePeakComponent::getVolumeY( const int offsetText, const int height, const float decibels )
{
    const float minVolume = m_minChartVolume; // -18.f;
    const float maxVolume = m_maxChartVolume; // 6.f;

    jassert(offsetText < height);
    
    if ( decibels > maxVolume )
        return offsetText;

    if ( decibels < minVolume )
        return height;
    
    const float offset = 12.f; // offset to avoid being too close to 0 in log function
    float value = logf( offset - decibels );
    static const float valueMin = logf( offset - minVolume );
    static const float valueMax = logf( offset - maxVolume );

    return offsetText + int( ( value - valueMax ) * (float) ( height - offsetText ) / ( valueMin - valueMax ) );
}

void TruePeakComponent::resetVolumeInertia()
{
    for ( int i = 0 ; i < LUFS_TP_MAX_NB_CHANNELS ; ++i )
    {
        m_channelInertiaStruct[i].m_decibelVolume = DEFAULT_MIN_VOLUME;
        m_channelInertiaStruct[i].m_index = 0;
    }
}

float TruePeakComponent::InertiaStruct::getCurrentVolume(int index)
{
    float decibels = m_decibelVolume - 5.f * (float)(index - m_index);
    return decibels > DEFAULT_MIN_VOLUME ? decibels : DEFAULT_MIN_VOLUME;
}
