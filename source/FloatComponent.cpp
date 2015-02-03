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

#include "FloatComponent.h"

FloatComponent::FloatComponent()
    : m_volume( DEFAULT_MIN_VOLUME )
{
}

void FloatComponent::paint( juce::Graphics & g )
{
#ifdef TESTCOLORS
    g.fillAll( juce::Colours::red );
#endif

    juce::Font font( 36.f );
    font.setBold(true);
    g.setFont (font);
    g.setColour( m_color );

    juce::String text = juce::String( m_volume, 1 );

    const int xIndent = 2;
    const int yIndent = 2;

    g.drawFittedText( text, xIndent, yIndent, getWidth() - xIndent, getHeight() - yIndent, juce::Justification::centred, 1, 0.01f );
}

void FloatComponent::setVolume( const float volume )
{
    const float newVolume = makeDigitValue( volume );

    if ( m_volume != newVolume )
    {
        m_volume = newVolume;
        repaint();
    }
}

float FloatComponent::makeDigitValue( const float value )
{
    int tenTimes = (int)( value * 10.f );
    float hundredTimes = value * 100.f;
    int intHundred = (int)hundredTimes;

    if ( ( intHundred % 10 ) > 5 )
        ++tenTimes;
    
    float floatTenTimes = (float)tenTimes;

    return floatTenTimes / 10.f;
}
