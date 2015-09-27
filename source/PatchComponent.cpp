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

#include "PatchComponent.h"

PatchComponent::PatchComponent(Patch & patch, juce::Colour backgroundColor, juce::Colour fontColor, bool enableMultiSelection)
    : juce::Component(juce::String::empty)
    , m_patch(patch)
    , m_patchView(patch, m_columnXInc, m_lineYInc, enableMultiSelection)
    , m_backgroundColor(backgroundColor)
    , m_fontColor(fontColor)
    , m_arrayX(0)
    , m_arrayY(0)
    , m_columnNamesAreLong(false)
{
    m_arrayX = 220;
    m_arrayY = 800;

    addAndMakeVisible(&m_patchView);

    m_patchView.addListener( this );
}

PatchComponent::~PatchComponent()
{
}

void PatchComponent::paint(juce::Graphics & g)
{
    const juce::Rectangle<int> r1 = g.getClipBounds();  
    
    const float angle = juce::float_Pi / 4.f;

    int width = 400;
    int height = 30;

    for (int i = 0 ; i < m_patch.getColumnNames().size() ; ++i)
    {
        juce::String text = m_patch.getColumnNames()[ i ];

        int x = m_arrayX + 10 + m_columnXInc * i;
        
        if ( m_columnNamesAreLong )
        {
            drawTextWithAngle( g, angle, x, m_arrayY, width, height, m_fontColor, text );
        }
        else 
        {
            juce::Font font(height / 2.f);
            g.setFont(font);
            g.setColour(m_fontColor);
            g.drawText(text, x, m_arrayY - height, 20, height, juce::Justification::centred, false);
        }
    }
}

void PatchComponent::redraw()
{
    m_columnNamesAreLong = false;
    for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
    {
        if (m_patch.getColumnNames()[column].length() > 3 )
            m_columnNamesAreLong = true;
    }

    m_arrayY = m_columnNamesAreLong ? 170 : 40;

    setSize(getWidth(), m_arrayY + m_lineYInc * m_patch.getLineNames().size());

    m_patchView.redraw();

    // force repaint since column and line names must be written too
    repaint();
}

void PatchComponent::drawTextWithAngle( juce::Graphics & g, float angle, int x, int y, int width, int height, juce::Colour fontColor, const juce::String & text )
{
    juce::Image image( juce::Image::RGB, width, height, true );

    juce::Graphics textG( image );
    textG.fillAll( m_backgroundColor );
    juce::Font font( height / 2.f );
    textG.setFont( font );
    textG.setColour( fontColor );
    textG.drawText( text, 0, 0, width, height, juce::Justification::centredLeft, false );

    float offset = cosf( angle ) * (float)height;
    juce::AffineTransform transform = juce::AffineTransform::identity.rotated( -angle, 0.f, 0.f ).translated( (float)x, y - offset );

    g.drawImageTransformed( image, transform );
}

void PatchComponent::resized()
{
    m_patchView.setBounds(10, m_arrayY, getWidth() - 20, getHeight() - m_arrayY);
}

void PatchComponent::patchHasChanged(const juce::BigInteger & activeLines, const juce::BigInteger & activeColumns) 
{
    for ( int i = 0 ; i < m_listeners.size() ; ++i )
    {
        m_listeners[i]->patchHasChanged( this, activeLines, activeColumns );
    }
}

