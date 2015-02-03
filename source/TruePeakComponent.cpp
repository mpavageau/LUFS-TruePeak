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

TruePeakComponent::TruePeakComponent()
    : m_processor( nullptr )
    , m_validSize( 0 )
{
    m_valueComponent.setTextColor( COLOR_LUFSTIME );
    addAndMakeVisible( &m_valueComponent );

    m_valueComponent.setBounds( 10, 10, 100, 40 );
}

void DEBUGoutput( const char * _text, ...)
{
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
}


void TruePeakComponent::paint( juce::Graphics & g )
{
//    g.fillAll( juce::Colours::red );

    juce::Font titleFont( 18.f );
    titleFont.setBold(true);
    g.setFont( titleFont );

    g.setColour( COLOR_LUFSTIME );
    g.drawFittedText( juce::String( "True Peak" ), 10, 60, 120, 20, juce::Justification::centred, 1, 0.01f );

    if ( !m_validSize )
        return;

    const int currentIndex = m_validSize - 1;

    juce::Colour colorCurrent = COLOR_BACKGROUND_GRAPH;
    juce::Colour colorMax = juce::Colours::white;

    const int offsetX = 8;
    const int width = ( getWidth() - 7 * offsetX ) / 6;
    const int offsetY = 100;
    const int height = getHeight() - offsetY - 35;

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
        float currentDecibel = m_processor->m_lufsProcessor.getTruePeakChannelArray(ch)[currentIndex];
        int y = getVolumeY( height, currentDecibel);
        //DEBUGoutput( "currentIndex %d currentDecibel: %.3f -> y %d", currentIndex, currentDecibel, y);
        g.fillRect( x, offsetY + y, width, height - y);

        x += offsetX + width;
    }

    g.setColour( colorMax );

    x = 2 * offsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        float maxDecibel = m_processor->m_lufsProcessor.getTruePeakChannelMax(ch);
        int y = getVolumeY( height, maxDecibel);
        g.fillRect( x, offsetY + y, width, 1);

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
        //int index = m_validSize - 1;
        m_valueComponent.setVolume( m_processor->m_lufsProcessor.getTruePeak()/*getTruePeakArray()[ index ]*/ );
    }

    repaint();
}

void TruePeakComponent::reset()
{
    m_validSize = 0;
    m_valueComponent.setVolume( DEFAULT_MIN_VOLUME );

    repaint();
}

int TruePeakComponent::getVolumeY( const int height, const float decibels )
{
    static const float minVolume = -18.f;
    static const float maxVolume = 6.f;

    if ( decibels > maxVolume )
        return 0;

    if ( decibels < minVolume )
        return height;

    const float offset = 12.f; // offset to avoid being too close to 0 in log function 
    float value = logf( offset - decibels );
    static const float valueMin = logf( offset - minVolume );
    static const float valueMax = logf( offset - maxVolume );

    return int( ( value - valueMax ) * (float) ( height ) / ( valueMin - valueMax ) );
}

