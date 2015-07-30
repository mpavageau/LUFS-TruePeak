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

#include "Chart.h"

#include "LufsAudioProcessor.h"
#include "LufsTruePeakPluginEditor.h"

int Chart::getVolumeY( const int height, const float decibels )
{
    if ( decibels > m_maxChartVolume )
        return 0;

    if ( decibels < m_minChartVolume )
        return height;

    const float offset = 25.f; // offset to avoid being too close to 0 in log function 
    float value = logf( offset - decibels );
    static const float valueMin = logf( offset - m_minChartVolume );
    static const float valueMax = logf( offset - m_maxChartVolume );

    return int( ( value - valueMax ) * (float) ( height ) / ( valueMin - valueMax ) );
}

void Chart::update()
{
    bool cursorIsAtMaxRight = false;
    if ( m_chartView != nullptr )
    {
        const int viewPositionX = m_chartView->getViewPositionX();
        const int viewWidth = m_chartView->getViewWidth();
        cursorIsAtMaxRight = ( viewPositionX + viewWidth ) >= ( m_validSize - 2 );
    }
 
    m_validSize = m_processor->m_lufsProcessor.getValidSize();
    if ( m_validSize > getWidth() )
        setSize( m_validSize, getHeight() );

    if ( cursorIsAtMaxRight )
    {
        m_chartView->setViewPositionProportionately( 1.0, 1.0 );
    }

    repaint();
}

void Chart::paint(juce::Graphics& g)
{
    juce::Rectangle<int> clipBounds = g.getClipBounds();

    const int imageWidth = clipBounds.getWidth();
    const int imageHeight = getHeight();
    const int beginning = clipBounds.getX();

    g.setColour( COLOR_BACKGROUND_GRAPH );
    g.fillRect( beginning, 0, imageWidth, imageHeight );

    if ( m_validSize && clipBounds.getX() < ( m_validSize - 2 ) )
    {
        juce::Font lufsFont( 12.f );
        lufsFont.setBold(true);
        g.setFont( lufsFont );

        int size = imageWidth;
        if ( beginning + size > m_validSize - 2 )
            size = m_validSize - 2 - beginning;

        // range 
        g.setColour( COLOR_RANGE );
        int yRange = getVolumeY( imageHeight, m_processor->m_lufsProcessor.getRangeMaxVolume() );
        int hRange = getVolumeY( imageHeight, m_processor->m_lufsProcessor.getRangeMinVolume() ) - yRange;
        g.fillRect( clipBounds.getX(), yRange, clipBounds.getWidth(), hRange );

        // time lines
        g.setColour( juce::Colours::black );
        for ( int i = beginning - 100 ; i < ( beginning + imageWidth ) ; ++i )
        {
            if ( !( i % 100 ) )
            {
                g.fillRect( i, 0, 1, imageHeight );
            }
        }

        // true peak vertical lines
        paintTruePeakLines( g, m_processor->m_lufsProcessor.getTruePeakArray(), beginning, size );

        // volume lines 
        g.setColour( juce::Colours::black );
        for ( float v = -3.f ; v > -70.f ; v -= 3.f )
        {
            if ( ( v >= m_minChartVolume ) && ( v <= ( m_maxChartVolume ) ) )
            {
                int y = getVolumeY( imageHeight, v );
                g.fillRect( clipBounds.getX(), y, clipBounds.getWidth(), 1 );
            }
        }

        g.setColour( COLOR_INTEGRATED );
        //g.fillRect( 0, getVolumeY( imageHeight, m_processor->m_lufsProcessor.getIntegratedVolume() ), imageWidth, 3 );
        g.fillRect( clipBounds.getX(), getVolumeY( imageHeight, m_processor->m_lufsProcessor.getIntegratedVolume() ), clipBounds.getWidth(), 3 );

        {
            const int factor = 1;
            paintValues( g, COLOR_MOMENTARY, m_processor->m_lufsProcessor.getMomentaryVolumeArray(), factor, beginning, size / factor );
            paintValues( g, COLOR_SHORTTERM, m_processor->m_lufsProcessor.getShortTermVolumeArray(), factor, beginning, size / factor );
        }

        // time text
        g.setColour( juce::Colours::black );
        // don't show time text at far left and far right to make space for "Min vol" and Memory
        for ( int i = beginning + 40 ; i < ( beginning + imageWidth - 120) ; ++i )
        {
            if ( !( i % 100 ) )
            {
                const int seconds = ( i / 10 ) % 60;
                const int minutes = ( i / 600 ) % 60;
                const int hours = ( i / 36000 );
                juce::String text;
                if ( hours )
                {
                    text += juce::String( hours );
                    text += ":";
                }
                if ( minutes < 10 ) text += "0";
                text += juce::String( minutes );
                text += ":";
                if ( seconds < 10 ) text += "0";
                text += juce::String( seconds );

                g.drawFittedText( text, i + 5, imageHeight - 12, 60, 10, juce::Justification::centredLeft, 1, 0.01f );
            }
        }

    }
    else
    {
        g.setColour( juce::Colours::black );
        for ( float v = -5.f ; v > -70.f ; v -= 5.f )
        {
            int y = getVolumeY( imageHeight, v );
            g.fillRect( 0, y, imageWidth, 1 );
        }
    }

    g.setColour( juce::Colours::black );

    // volume text
    for ( float v = -3.f ; v > -70.f ; v -= 3.f )
    {
        if ( ( v >= m_minChartVolume + 3.f ) && ( v <= m_maxChartVolume ) )
        {
            int y = getVolumeY( imageHeight, v );
            g.drawFittedText( juce::String( v, 0 ), clipBounds.getX() + 5, y - 30, 20, 30, juce::Justification::centredBottom, 1, 0.01f );
        }
    }
    g.drawFittedText( "Min vol", clipBounds.getX() + 5, imageHeight - 15, 40, 10, juce::Justification::centredLeft, 1, 0.01f );

    // memory
    float memoryPercent = 100.f * (float)m_processor->m_lufsProcessor.getValidSize() / (float)m_processor->m_lufsProcessor.getMaxSize();

    if ( memoryPercent > 80.f )
        g.setColour( juce::Colours::red );

    juce::String memory( "Memory: ");
    memory << juce::String( memoryPercent, 2 );
    memory << " %";
    g.drawFittedText( memory, clipBounds.getX() + clipBounds.getWidth() - 155, imageHeight - 15, 150, 10, juce::Justification::centredRight, 1, 0.01f );
    
    if ( m_processor->m_lufsProcessor.isPaused() )
    {
        juce::Font pausedFont( 36.f );
        pausedFont.setBold(true);
        g.setFont( pausedFont );
        
        g.setColour( juce::Colours::red );
        
        g.drawFittedText( "Paused",
                         10, //clipBounds.getX() + clipBounds.getWidth() / 3,
                         10, //clipBounds.getY() + clipBounds.getHeight() / 3,
                         clipBounds.getWidth() / 3,
                         clipBounds.getHeight() / 3, juce::Justification::topLeft, 1, 0.01f );
        
        
    }

}

void Chart::paintValues( juce::Graphics& g, const juce::Colour _color, const float * _data, const int _itemsPerPixel, const int _offset, const int _pixels )
{
    g.setColour( _color );

    const int imageHeight = getHeight();
    const int max = _offset + _pixels * _itemsPerPixel;

    jassert( _offset < m_validSize );
    jassert( max < m_validSize );

    _data += _offset;

    if ( _itemsPerPixel == 1 )
    {
        float vol1 = *_data++;
        float vol2 = *_data++;

        for ( int i = _offset ; i < _offset + _pixels - 2 ; ++i )
        {
            g.drawLine( (float)( i ), (float)getVolumeY( imageHeight, vol1 ), (float)( i + 1 ), (float)getVolumeY( imageHeight, vol2 ), 3.f );
            vol1 = vol2;
            vol2 = *_data++;
        }   
    }
    else
    {
        float min1 = *_data++;;
        float max1 = min1;
        for ( int j = 1 ; j < _itemsPerPixel ; ++j )
        {
            float val = *_data++;
            if ( val > max1 ) max1 = val;
            if ( val < min1 ) min1 = val;
        }

        for ( int i = 0 ; i < _pixels - 2; ++i )
        {
            float min2 = *_data++;;
            float max2 = min2;
            for ( int j = 1 ; j < _itemsPerPixel ; ++j )
            {
                float val = *_data++;
                if ( val > max2 ) max2 = val;
                if ( val < min2 ) min2 = val;
            }

            g.drawLine( (float)( i ), (float)getVolumeY( imageHeight, max1 ), (float)( i + 1 ), (float)getVolumeY( imageHeight, max2 ), 3.f );
            max1 = max2;
        }
    }
}

void Chart::paintTruePeakLines( juce::Graphics& g, const float * _data, const int _offset, const int _pixels )
{
    const int imageHeight = getHeight();
    const int max = _offset + _pixels;

    jassert( _offset < m_validSize );
    jassert( max < m_validSize );

    _data += _offset;

    for ( int i = _offset ; i < _offset + _pixels - 2 ; ++i )
    {
        const float decibelTruePeak = *_data++;
        if ( decibelTruePeak >= DEFAULT_ACCEPTABLE_MAX_TRUE_PEAK )
        {
            g.setColour( juce::Colours::red );
            g.fillRect( i, 0, 1, imageHeight ); // drawLine( (float)( i ), 0, (float)( i + 1 ), (float)getVolumeY( imageHeight, vol2 ), 3.f );
        }
    }   
}


ChartView::ChartView( float _minChartVolume, float _maxChartVolume ) 
    : m_chart (_minChartVolume, _maxChartVolume )
{
    resetScrolling();

    m_chart.setChartView( this );

    setViewedComponent( &m_chart, false );
}

void ChartView::resized()
{
    m_chart.setSize( 1 + getWidth(), getHeight() - 35 );
}

void ChartView::resetScrolling()
{
    m_chart.setSize( 1 + getWidth(), getHeight() - 35 );
}

void ChartView::mouseWheelMove( const juce::MouseEvent& /*event*/, const juce::MouseWheelDetails & wheel ) 
{
    juce::String text( wheel.deltaX );
    text += " y: ";
    text += juce::String( wheel.deltaY );
    DBG(text);

    const int viewPositionX = getViewPositionX();
    const int width = m_chart.getWidth() - getWidth();
    const float pos = (float)viewPositionX / (float)width;
    const float page = (float)getWidth() / (float)width;

    if ( wheel.deltaY > 0.f )
    {
        float newPositionX = pos + page / 2.f;
        if ( newPositionX > 1.f )
            newPositionX = 1.f;
        
        setViewPositionProportionately( newPositionX, 1.f );
    }
    else if ( wheel.deltaY < 0.f )
    {
        float newPositionX = pos - page / 2.f;
        if ( newPositionX < 0.f )
            newPositionX = 0.f;
        
        setViewPositionProportionately( newPositionX, 1.f );
    }
}

void ChartView::update()
{
    m_chart.update();
}
