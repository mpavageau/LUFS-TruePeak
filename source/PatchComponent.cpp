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

#include "PatchButton.h"

PatchComponent::PatchComponent(Patch & patch, juce::Colour backgroundColor, juce::Colour fontColor, bool enableMultiSelection)
    : juce::Component(juce::String::empty)
    , m_patch(patch)
    , m_backgroundColor(backgroundColor)
    , m_fontColor(fontColor)
    , m_arrayX(0)
    , m_arrayY(0)
    , m_enableMultiSelection(enableMultiSelection)
    , m_columnNamesAreLong(false)
{
    m_arrayX = 220;
    m_arrayY = 800;
}

PatchComponent::~PatchComponent()
{
    deleteButtons();
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

    int y = m_arrayY - 3;
    int offsetX = 10;
    g.setColour(m_fontColor);
    for (int i = 0 ; i < m_patch.getLineNames().size() ; ++i)
    {
        juce::String text = m_patch.getLineNames()[ i ];

        juce::Font font(height / 2.f);
        g.setFont(font);
        g.drawText(text, offsetX, y, m_arrayX - 2 * offsetX, m_lineYInc, juce::Justification::centredRight, false);

        y += m_lineYInc;
    }
}

void PatchComponent::deleteButtons()
{
    for ( int i = 0 ; i < m_buttonArray.size() ; ++i )
    {
        delete m_buttonArray.getUnchecked( i );
    }

    m_buttonArray.clear();
}

void PatchComponent::redraw()
{
    deleteButtons();

    m_columnNamesAreLong = false;
    for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
    {
        if (m_patch.getColumnNames()[column].length() > 3 )
            m_columnNamesAreLong = true;
    }

    m_arrayY = m_columnNamesAreLong ? 170 : 40;

    for ( int line = 0 ; line < m_patch.getLineNames().size() ; ++line )
    {
        for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
        {
            PatchButton * button = new PatchButton( juce::Colours::yellow );
            addAndMakeVisible( button );
            int x = m_arrayX + 10 + m_columnXInc * column;
            int y = m_arrayY + m_lineYInc * line;
            button->setBounds( x, y, 20, 20 );
            button->setRadioGroupId( 1 + column );

            jassert(m_patch.getIndex(column, line) == m_buttonArray.size());

            button->setToggleState(m_patch.getState(column, line), juce::dontSendNotification);

            m_buttonArray.add( button );

            button->addListener(this);
        }
    }

    setSize(getWidth(), m_arrayY + m_lineYInc * m_patch.getLineNames().size());

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

void PatchComponent::buttonClicked(juce::Button * button) 
{
    bool foundIt = false;
    for ( int line = 0 ; line < m_patch.getLineNames().size() ; ++line )
    {
        for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
        {
            if (button == m_buttonArray[m_patch.getIndex(column, line)])
            {
                if ( m_patch.getState(column, line) )
                {
                    // uncheck
                    button->setToggleState(false, juce::dontSendNotification);
                    m_patch.setState(column, line, false);
                }

                foundIt = true;

                break;
            }

            if (foundIt)
                break;
        }
    }

    // notify
    juce::BigInteger activeLines = m_patch.getActiveLines();
    juce::BigInteger activeColumns = m_patch.getActiveColumns();

    for (int i = 0 ; i < m_listeners.size() ; ++i)
    {
        m_listeners[i]->patchHasChanged(this, activeLines, activeColumns);
    }
}

void PatchComponent::buttonStateChanged(juce::Button * button) 
{
    bool newStateIsOn = false;
    int buttonColumn = -1;
    int buttonLine = -1;

    for ( int line = 0 ; line < m_patch.getLineNames().size() ; ++line )
    {
        for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
        {
            if (button == m_buttonArray[m_patch.getIndex(column, line)])
            {
                newStateIsOn = button->getToggleState();
                buttonColumn = column;
                buttonLine = line;

                if (m_patch.getState(column, buttonLine) != newStateIsOn)
                    m_patch.setState(column, buttonLine, newStateIsOn);
                else 
                {
                    // no change
                    return;
                }

                break;
            }
        }
    }

    if ( newStateIsOn && !m_enableMultiSelection )
    {
        // uncheck all on same line
        for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
        {
            if ( column != buttonColumn )
            {
                PatchButton * button = m_buttonArray[m_patch.getIndex(column, buttonLine)];

                // uncheck
                button->setToggleState(false, juce::dontSendNotification);
                
                if (m_patch.getState(column, buttonLine))
                    m_patch.setState(column, buttonLine, false);
            }
        }
    }

    // notify
    juce::BigInteger activeLines = m_patch.getActiveLines();
    juce::BigInteger activeColumns = m_patch.getActiveColumns();

    for (int i = 0 ; i < m_listeners.size() ; ++i)
    {
        m_listeners[i]->patchHasChanged(this, activeLines, activeColumns);
    }
}

PatchView::PatchView(Patch & patch, juce::Colour backgroundColor, juce::Colour fontColor, bool enableMultiSelection) 
    : m_patchComponent(patch, backgroundColor, fontColor, enableMultiSelection)
{
    setViewedComponent(&m_patchComponent, false);
}

void PatchView::resized()
{
    m_patchComponent.setSize(getWidth() - 50, m_patchComponent.getHeight());
}



