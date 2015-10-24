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

#include "Chart.h"
#include "LufsAudioProcessor.h"

TruePeakComponent::TruePeakComponent( float minChartVolume, float maxChartVolume )
    : m_valueComponent( "True Peak", COLOR_LUFSTIME, false )
    , m_processor( nullptr )
    , m_validSize( 0 )
    , m_minChartVolume( minChartVolume )
    , m_maxChartVolume( maxChartVolume )
    , m_chart( nullptr )
{
    addAndMakeVisible( &m_valueComponent );

    resetVolumeInertia();
    
    // kSpeakerArr51 is "L R C Lfe Ls Rs";
    m_channelNames.add( "L" );
    m_channelNames.add( "R" );
    m_channelNames.add( "C" );
    m_channelNames.add( "Lfe" );
    m_channelNames.add( "Ls" );
    m_channelNames.add( "Rs" );

    m_valueComponent.setValueChangedUpdateObject( this );
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
    //g.drawImage(m_vumeterImage, 0, 0, m_vumeterImage.getWidth(), m_vumeterImage.getHeight(), 0, 0, m_vumeterImage.getWidth(), m_vumeterImage.getHeight());

    juce::Font titleFont( 18.f );
    titleFont.setBold(true);
    g.setFont( titleFont );

    const juce::Colour colorCurrent = COLOR_BACKGROUND_GRAPH;
    const juce::Colour colorMax = juce::Colours::white;

    int x;
    int valueArrayForY[LUFS_TP_MAX_NB_CHANNELS];
    
    if ( m_validSize )
    {
        const int currentIndex = m_validSize - 1;

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
        
            valueArrayForY[ch] = getVolumeY( uiVolumeDecibels);
        }
    }
    else
    {
        for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
            valueArrayForY[ch] = m_vumeterHeight;
    }

    x = 2 * m_vumeterOffsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        // use empty image for top
        g.drawImage(m_vumeterImage, x, m_vumeterOffsetY, (int)(m_vumeterImage.getWidth() / 3.f), valueArrayForY[ch], (int)(0.f * m_vumeterImage.getWidth() / 3.f), 0, (int)(m_vumeterImage.getWidth() / 3.f), valueArrayForY[ch]);

        g.drawImage(m_vumeterImage, x, m_vumeterOffsetY + valueArrayForY[ch], (int)(m_vumeterImage.getWidth() / 3.f), m_vumeterHeight - valueArrayForY[ch], (int)(1.f * m_vumeterImage.getWidth() / 3.f), valueArrayForY[ch], (int)(m_vumeterImage.getWidth() / 3.f), m_vumeterHeight - valueArrayForY[ch]);

        x += m_vumeterOffsetX + m_vumeterWidth;
    }

    g.drawImage(m_vumeterImage, x, m_vumeterOffsetY, (int)(m_vumeterImage.getWidth() / 3.f), m_vumeterImage.getHeight(), (int)(2.f * m_vumeterImage.getWidth() / 3.f), 0, (int)(m_vumeterImage.getWidth() / 3.f), m_vumeterImage.getHeight());

    g.setColour( colorMax );

    juce::Font figureFont( 12.f );
    figureFont.setBold(true);
    g.setFont( figureFont );
    
    x = 2 * m_vumeterOffsetX;
    for (int ch = 0 ; ch < LUFS_TP_MAX_NB_CHANNELS ; ++ch )
    {
        float maxDecibel = m_processor->m_lufsProcessor.getTruePeakChannelMax(ch);
        int y = getVolumeY( maxDecibel);
        g.fillRect( x, m_vumeterOffsetY + y, m_vumeterWidth, 1);
        
        const juce::String text = maxDecibel > -100.f ? juce::String(maxDecibel, 1) : juce::String((int)maxDecibel);
        g.drawFittedText( text, x, m_vumeterOffsetY + y - 21, m_vumeterWidth, 20, juce::Justification::centred, 1, 0.01f );
        
        g.drawFittedText( m_channelNames[ch], x, m_vumeterOffsetY + m_vumeterHeight + 3, m_vumeterWidth, 20, juce::Justification::centred, 1, 0.01f );
        

        x += m_vumeterOffsetX + m_vumeterWidth;
    }
}

void TruePeakComponent::resized()
{
    m_valueComponent.setBounds( 0, 0, getWidth(), 100 );

    // create image with vumeter colors and vumeter volume indications 
    
    m_vumeterOffsetX = 3;
    m_vumeterWidth = ( getWidth() - 8 * m_vumeterOffsetX ) / 7;
    m_vumeterOffsetY = 100;
    m_vumeterHeight = getHeight() - m_vumeterOffsetY - 35;
    m_offsetTextForVolumeY = 21;
    m_heightForVolumeY = m_vumeterHeight; 

    // image has 3 inner images
    m_vumeterImage = juce::Image(juce::Image::RGB, 3 * m_vumeterWidth, m_vumeterHeight, false);

    juce::Graphics g(m_vumeterImage);

    const int changeColorY = getVolumeY( m_valueComponent.getThresholdVolume());

    // not active
    float backgroundInterpolator = 0.7f;
    g.setColour(juce::Colours::red.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator));
    g.fillRect(0.f, 0.f, (float)m_vumeterWidth, (float)changeColorY);
    g.setGradientFill(juce::ColourGradient(juce::Colours::yellow.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)changeColorY, juce::Colours::green.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)m_vumeterHeight, false));
    g.fillRect(0.f, (float)changeColorY, (float)m_vumeterWidth, (float)(m_vumeterHeight-changeColorY));

    // active
    g.setColour(juce::Colours::red);
    g.fillRect((float)m_vumeterWidth, 0.f, (float)m_vumeterWidth, (float)changeColorY);
    g.setGradientFill(juce::ColourGradient(juce::Colours::yellow, 0.f, (float)changeColorY, juce::Colours::green, 0.f, (float)m_vumeterHeight, false));
    g.fillRect((float)m_vumeterWidth, (float)changeColorY, (float)m_vumeterWidth, (float)(m_vumeterHeight-changeColorY));

    // figures
    juce::Font figureFont( 12.f );
    figureFont.setBold(true);
    g.setFont( figureFont );
    g.setColour(LUFS_COLOR_BACKGROUND);
    g.fillRect(2.f * (float)m_vumeterWidth, 0.f, (float)m_vumeterWidth, (float)m_vumeterHeight);
    g.setColour(LUFS_COLOR_FONT);
    // white lines for scale
    int lastY = -100;
    for (float decibel = 3.f ; decibel > -80.f ; decibel -= 3.f)
    {
        int y = getVolumeY( decibel);

        if (y - lastY > 20)
        {
            g.fillRect( 2.f * m_vumeterWidth, (float)y, (float)m_vumeterWidth, (float)1);

            const juce::String text = decibel > 0.f ? juce::String("+") + juce::String((int)decibel) : juce::String((int)decibel);
            g.drawFittedText( text, (int)(2.f * m_vumeterWidth), y - 18, m_vumeterWidth, 20, juce::Justification::centred, 1, 0.01f );

            lastY = y;
        }
    }

    // darker lines for scale on vumeter image

    // not active
    lastY = -100;
    backgroundInterpolator = 0.75f;
    g.setColour(juce::Colours::red.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator));
    bool changedColor = false;
    for (float decibel = 3.f ; decibel > -80.f ; decibel -= 3.f)
    {
        int y = getVolumeY( decibel);

        if (y >= changeColorY && !changedColor)
        {
            g.setGradientFill(juce::ColourGradient(juce::Colours::yellow.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)changeColorY, juce::Colours::green.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)m_vumeterHeight, false));

            changedColor = true;
        }

        if (y - lastY > 20)
        {
            g.fillRect( 0.f, (float)y, (float)m_vumeterWidth, (float)1);

            lastY = y;
        }
    }
    // active
    lastY = -100;
    backgroundInterpolator = 0.1f;
    g.setColour(juce::Colours::red.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator));
    changedColor = false;
    for (float decibel = 3.f ; decibel > -80.f ; decibel -= 3.f)
    {
        int y = getVolumeY( decibel);

        if (y >= changeColorY && !changedColor)
        {
            g.setGradientFill(juce::ColourGradient(juce::Colours::yellow.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)changeColorY, juce::Colours::green.interpolatedWith(LUFS_COLOR_BACKGROUND, backgroundInterpolator), 0.f, (float)m_vumeterHeight, false));

            changedColor = true;
        }

        if (y - lastY > 20)
        {
            g.fillRect( (float)m_vumeterWidth, (float)y, (float)m_vumeterWidth, (float)1);

            lastY = y;
        }
    }
}

void TruePeakComponent::update()
{
    m_validSize = m_processor->m_lufsProcessor.getValidSize();

    if ( m_validSize )
    {
        float truePeak = m_processor->m_lufsProcessor.getTruePeak();
        
        m_valueComponent.setVolume( truePeak );
    }

    repaint();
}

void TruePeakComponent::reset()
{
    m_validSize = 0;
    m_valueComponent.setVolume( DEFAULT_MIN_VOLUME );
    m_valueComponent.resetWarning();

    repaint();
    
    resetVolumeInertia();
}

int TruePeakComponent::getVolumeY( const float decibels )
{
    const float minVolume = m_minChartVolume; // -18.f;
    const float maxVolume = m_maxChartVolume; // 6.f;

    jassert(m_offsetTextForVolumeY < m_heightForVolumeY);
    
    if ( decibels > maxVolume )
        return m_offsetTextForVolumeY;

    if ( decibels < minVolume )
        return m_heightForVolumeY;
    
    const float offset = 12.f; // offset to avoid being too close to 0 in log function
    float value = logf( offset - decibels );
    static const float valueMin = logf( offset - minVolume );
    static const float valueMax = logf( offset - maxVolume );

    return m_offsetTextForVolumeY + int( ( value - valueMax ) * (float) ( m_heightForVolumeY - m_offsetTextForVolumeY ) / ( valueMin - valueMax ) );
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
    float decibels = m_decibelVolume - 3.f * (float)(index - m_index);
    return decibels > DEFAULT_MIN_VOLUME ? decibels : DEFAULT_MIN_VOLUME;
}

void TruePeakComponent::valueChangeUpdate()
{
    // force update of m_vumeterImage
    resized();
    repaint();

    if (m_chart != nullptr)
        m_chart->setTruePeakThreshold( m_valueComponent.getThresholdVolume() );
}
