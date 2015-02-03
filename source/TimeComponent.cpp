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

#include "TimeComponent.h"

TimeComponent::TimeComponent()
    : m_seconds( 0 )
{
}

void TimeComponent::paint( juce::Graphics & g )
{
#ifdef TESTCOLORS
    g.fillAll( juce::Colours::green );
#endif

    juce::Font font( 22.f );
    font.setBold(true);
    g.setFont (font);
    g.setColour( m_color );

    int hours = m_seconds / 3600;
    int minutes = ( m_seconds - 3600 * hours ) / 60;
    int seconds = ( m_seconds - 3600 * hours - 60 * minutes );

    juce::String text;
    if ( hours < 10 )
        text << "0";
    text << juce::String( hours );
    text << ":";
    if ( minutes < 10 )
        text << "0";
    text << juce::String( minutes );
    text << ":";
    if ( seconds < 10 )
        text << "0";
    text << juce::String( seconds );

    const int xIndent = 2;
    const int yIndent = 3;

    g.drawFittedText( text, xIndent, yIndent, getWidth() - xIndent, getHeight() - yIndent, juce::Justification::centred, 1, 0.01f );
}

void TimeComponent::setSeconds( int _seconds )
{
    if ( m_seconds != _seconds )
    {
        m_seconds = _seconds;
        repaint();
    }
}
