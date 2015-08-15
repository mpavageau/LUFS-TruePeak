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

#include "TextAndFloatComponent.h"

TextAndFloatComponent::TextAndFloatComponent(const char * name, juce::Colour color, bool invertedWarning)
    : juce::Component( name )
    , m_floatComponent( color )
    , m_color( color )
    , m_thresholdVolume( 0.f )
    , m_showWarningFrame( false )
    , m_invertedWarning( invertedWarning )
{
    addAndMakeVisible( &m_floatComponent );
}

void TextAndFloatComponent::paint( juce::Graphics & g )
{
#ifdef TESTCOLORS
    g.fillAll( juce::Colours::red );
#endif

    if ( m_showWarningFrame )
        paintFrame( g );

    g.setColour( m_color );
    juce::Font font( 18.f );
    font.setBold(true);
    g.setFont( font );
    g.drawFittedText( juce::String( getName() ), 0, 60, getWidth(), 20, juce::Justification::centred, 1, 0.01f );
}

void TextAndFloatComponent::resized() 
{
    jassert( getHeight() >= 50 );

    m_floatComponent.setBounds( 0, 10, getWidth(), 40 );
}

void TextAndFloatComponent::setVolume( const float volume )
{
    m_floatComponent.setVolume( volume );

    if ( m_invertedWarning )
    {
        if ( m_showWarningFrame )
        {
            if ( volume > m_thresholdVolume )
            {
                m_showWarningFrame = false;
                repaint();
            }
        }
        else
        {
            if ( volume < m_thresholdVolume )
            {
                m_showWarningFrame = true;
                repaint();
            }
        }
    }
    else
    {
        if ( m_showWarningFrame )
        {
            if ( volume < m_thresholdVolume )
            {
                m_showWarningFrame = false;
                repaint();
            }
        }
        else
        {
            if ( volume > m_thresholdVolume )
            {
                m_showWarningFrame = true;
                repaint();
            }
        }
    }
}

void TextAndFloatComponent::setThresholdVolume(float thresholdVolume)
{
    m_thresholdVolume = thresholdVolume;

    m_showWarningFrame = false;

    repaint();
}

void TextAndFloatComponent::paintFrame( juce::Graphics& g )
{
    const int frameOffset = 3;
    const int frameWidth = 7;
    g.setColour( juce::Colours::red );
    g.fillRect( frameOffset, frameOffset, getWidth() - 2 * frameOffset, frameWidth );
    g.fillRect( frameOffset, getHeight() - frameOffset - frameWidth, getWidth() - 2 * frameOffset, frameWidth );
    g.fillRect( frameOffset, frameOffset, frameWidth, getHeight() - 2 * frameOffset );
    g.fillRect( getWidth() - frameOffset - frameWidth, frameOffset, frameWidth, getHeight() - 2 * frameOffset );
}

void TextAndFloatComponent::juceValueHasChanged(double value)
{
    setThresholdVolume((float)value); 
}

