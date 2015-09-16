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

#include "PatchButton.h"

PatchButton::PatchButton(juce::Colour color)
    : juce::ToggleButton(juce::String::empty)
    , m_color( color )
{
}

void PatchButton::paintButton(juce::Graphics & g, bool isMouseOverButton, bool isButtonDown) 
{
    float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.55f;

    if ( !isEnabled() )
        alpha *= 0.5f;

    float x = 0, y = 0, diam;

    if (getWidth() < getHeight())
    {
        diam = (float) getWidth();
        y = (getHeight() - getWidth()) * 0.5f;
    }
    else
    {
        diam = (float) getHeight();
        y = (getWidth() - getHeight()) * 0.5f;
    }

    x += diam * 0.05f;
    y += diam * 0.05f;
    diam *= 0.9f;

    g.setGradientFill( juce::ColourGradient( juce::Colour::greyLevel( 0.9f ).withAlpha( alpha ), 0, y + diam,
        juce::Colour::greyLevel( 0.6f ).withAlpha( alpha ), 0, y, false ) );
    g.fillEllipse (x, y, diam, diam);

    x += 2.0f;
    y += 2.0f;
    diam -= 4.0f;

    if ( getToggleState() )
    {
        juce::LookAndFeel_V2::drawGlassSphere(g, x, y, diam, m_color.withAlpha (alpha), 1.0f);
    }
}

