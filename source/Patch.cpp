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

#include "Patch.h"

#include "PatchButton.h"

Patch::Patch()
    : m_dirty(true)
    , m_floatArray(nullptr)
    , m_patchArray(nullptr)
    , m_arraySize(0)
    , m_inputChannelCount(0)
{
}

Patch::~Patch()
{
    if (m_floatArray != nullptr) delete [] m_floatArray;
    if (m_patchArray != nullptr) delete [] m_patchArray;
}

void Patch::setColumnNames(const juce::StringArray & columnNames)
{
    m_columnNames = columnNames;

    if (m_floatArray != nullptr) delete [] m_floatArray;
    if (m_patchArray != nullptr) delete [] m_patchArray;

    m_arraySize = columnNames.size();

    m_patchArray = new int [m_arraySize];
    typedef float * FLOAT_PTR;
    m_floatArray = new FLOAT_PTR [m_arraySize];

    for (int i = 0 ; i < columnNames.size() ; ++i)
        m_patchArray[i] = -1;

    m_dirty = true;
}

void Patch::setLineNames(const juce::StringArray & lineNames)
{
    m_lineNames = lineNames;

    m_dirty = true;
}

void Patch::initFromXml(const juce::XmlElement * element)
{
    m_devicePatches.setFromXmlAttributes(*element);
}

juce::XmlElement * Patch::createStateXml(const juce::String & patchString) const
{
    juce::XmlElement * element = new juce::XmlElement( patchString );

    for (int i = 0 ; i < m_devicePatches.size() ; ++i)
    {
        m_devicePatches.copyToXmlAttributes(*element);
    }

    return element;
}

void Patch::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    // store or get patch
    juce::String deviceTagName = generateDeviceTagName(device->getTypeName(), device->getName());
    if (m_deviceTagName == deviceTagName)
    {
        // store patch
        juce::String patch = m_patch.toString(2);
        m_devicePatches.set(m_deviceTagName, patch);
    }
    else
    {
        // get patch
        m_patch = getValidDevicePatch(device->getTypeName(), device->getName(), device->getInputChannelNames().size());

        m_deviceTagName = deviceTagName;
    }


    int preferredSampleCount = device->getCurrentBufferSizeSamples();

    m_buffer.setSize(1, 16 * preferredSampleCount, false, true);

    m_lineNames = device->getInputChannelNames();

    m_inputChannelCount = 0;
    for ( int line = 0 ; line < m_lineNames.size() ; ++line )
    {
        for ( int column = 0 ; column < m_columnNames.size() ; ++column )
        {
            const int index = getIndex(column, line);
            
            if (m_patch[index])
            {
                ++m_inputChannelCount;
                break;
            }
        }
    }

    for ( int column = 0 ; column < m_columnNames.size() ; ++column )
    {
        m_patchArray[column] = -1;
    }

    int inputChannel = 0;
    for ( int line = 0 ; line < m_lineNames.size() ; ++line )
    {
        for ( int column = 0 ; column < m_columnNames.size() ; ++column )
        {
            const int index = getIndex(column, line);
            
            if (m_patch[index])
            {
                jassert(inputChannel < m_inputChannelCount);
                m_patchArray[column] = inputChannel;
                ++inputChannel;
            }
        }
    }

    m_dirty = false;
}

const juce::AudioSampleBuffer Patch::getBuffer(float ** channelData, int sampleCount)
{
    jassert(m_dirty == false);

    if (sampleCount > m_buffer.getNumSamples())
    {
        // humpf, really allocate in callback?
        m_buffer.setSize(1, 16 * sampleCount, false, true);
    }

    for (int i = 0 ; i < m_arraySize ; ++i)
    {
        if (m_patchArray[i] >= 0 && m_patchArray[i] < m_inputChannelCount)
        {
            m_floatArray[i] = channelData[m_patchArray[i]];

            // safety
            if (m_floatArray[i] == nullptr)
                m_floatArray[i] = m_buffer.getArrayOfWritePointers()[0];
        }
        else
        {
            m_floatArray[i] = m_buffer.getArrayOfWritePointers()[0];
        }

        jassert(m_floatArray[i] != nullptr);
    }

    juce::AudioSampleBuffer buffer(m_floatArray, m_arraySize, sampleCount);

    return buffer;
}

int Patch::getIndex(int column, int line) const
{
    jassert( column < m_columnNames.size() );
    jassert( line < m_lineNames.size() );

    return line * m_columnNames.size() + column;
}

juce::String Patch::generateDeviceTagName(const juce::String & deviceType, const juce::String & deviceName) const
{
    return (deviceType + "-" + deviceName).retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");
}

const juce::BigInteger Patch::getDevicePatch(const juce::String & deviceTagName) const
{
    juce::String default("0");
    juce::var value = m_devicePatches.getWithDefault(deviceTagName, default);
    juce::String stringValue = value.toString();

    juce::BigInteger patch;
    patch.parseString(stringValue, 2);

    return patch;
}

const juce::BigInteger Patch::getValidDevicePatch(const juce::String & deviceType, const juce::String & deviceName, int lineCount) 
{
    juce::BigInteger patch = getDevicePatch(generateDeviceTagName(deviceType, deviceName));

    int highestBit = patch.getHighestBit();
    juce::String t = patch.toString(2);
    DBG(t);
    int columnCount = m_columnNames.size();
    if (highestBit >= columnCount * lineCount)
    {
        // reset patch
        patch = 0;
        m_devicePatches.set(generateDeviceTagName(deviceType, deviceName), "0");
    }

    return patch;
}

juce::BigInteger Patch::getActiveColumns() const
{
    juce::BigInteger activeColumns;

    for ( int line = 0 ; line < m_lineNames.size() ; ++line )
    {
        for ( int column = 0 ; column < m_columnNames.size() ; ++column )
        {
            const int index = getIndex(column, line);

            if (m_patch[index])
                activeColumns.setBit(column, true);
        }
    }

    return activeColumns;
}

juce::BigInteger Patch::getActiveLines() const
{
    return getActiveLines(m_deviceTagName);
}

juce::BigInteger Patch::getActiveLines(const juce::String & deviceTagName) const
{
    juce::BigInteger patch = getDevicePatch(deviceTagName);

    juce::BigInteger activeLines;

    for ( int line = 0 ; line < m_lineNames.size() ; ++line )
    {
        for ( int column = 0 ; column < m_columnNames.size() ; ++column )
        {
            const int index = getIndex(column, line);

            if (m_patch[index])
                activeLines.setBit(line, true);
        }
    }

    return activeLines;
}

juce::BigInteger Patch::getActiveLines(const juce::String & deviceType, const juce::String & deviceName) const
{
    return getActiveLines(generateDeviceTagName(deviceType, deviceName));
}

bool Patch::getState(int column, int line) const
{
    return m_patch[getIndex(column, line)];
}

void Patch::setState(int column, int line, bool state) 
{
    m_patch.setBit(getIndex(column, line), state);
}


// PatchView

PatchView::PatchView(Patch & patch, int columnXInc, int lineYInc, bool enableMultiSelection)
    : m_component(patch, columnXInc, lineYInc, enableMultiSelection)
{
    setViewedComponent(&m_component, false);
}

PatchView::Component::~Component()
{
    deleteButtons();
}

/*void PatchView::resized()
{
    m_component.setSize(getWidth() - 50, getHeight());
}*/

PatchView::Component::Component(Patch & patch, int columnXInc, int lineYInc, bool enableMultiSelection) 
    : m_patch(patch)
    , m_columnXInc(columnXInc)
    , m_lineYInc(lineYInc)
    , m_enableMultiSelection(enableMultiSelection)
{
}

void PatchView::Component::redraw()
{
    deleteButtons();

    for ( int line = 0 ; line < m_patch.getLineNames().size() ; ++line )
    {
        for ( int column = 0 ; column < m_patch.getColumnNames().size() ; ++column )
        {
            PatchButton * button = new PatchButton( juce::Colours::yellow );
            addAndMakeVisible( button );
            int m_arrayX = 220;
            int x = m_arrayX + 10 + m_columnXInc * column;
            int y = 1 + m_lineYInc * line;
            button->setBounds( x, y, 20, 20 );
            button->setRadioGroupId( 1 + column );

            jassert(m_patch.getIndex(column, line) == m_buttonArray.size());

            DBG(juce::String("setToggleState column " + juce::String(column) + juce::String(" line ") + juce::String(line) + " -> " + (m_patch.getState(column, line) ? "ON" : "off")));// );// + (
            button->setToggleState(m_patch.getState(column, line), juce::dontSendNotification);

            m_buttonArray.add( button );

            button->addListener(this);
        }
    }

    int m_arrayX = 220;
    setSize( m_arrayX + 20 + m_patch.getColumnNames().size() * m_columnXInc, m_patch.getLineNames().size() * m_lineYInc );
}

void PatchView::Component::buttonClicked(juce::Button * button) 
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
                    m_buttonArray[m_patch.getIndex(column, line)]->setToggleState(false, juce::dontSendNotification);
                    m_patch.setState(column, line, false);

                    // notify
                    juce::BigInteger activeLines = m_patch.getActiveLines();
                    juce::BigInteger activeColumns = m_patch.getActiveColumns();

                    for (int i = 0 ; i < m_listeners.size() ; ++i)
                    {
                        m_listeners[i]->patchHasChanged(activeLines, activeColumns);
                    }
                }

                foundIt = true;

                break;
            }

            if (foundIt)
                break;
        }
    }
}

void PatchView::Component::buttonStateChanged(juce::Button * button) 
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

                // uncheck if necessary 
                if (m_patch.getState(column, buttonLine))
                {
                    button->setToggleState(false, juce::sendNotification);
                
                    m_patch.setState(column, buttonLine, false);
                }
            }
        }
    }

    // notify
    juce::BigInteger activeLines = m_patch.getActiveLines();
    juce::BigInteger activeColumns = m_patch.getActiveColumns();

    for (int i = 0 ; i < m_listeners.size() ; ++i)
    {
        m_listeners[i]->patchHasChanged(activeLines, activeColumns);
    }
}

void PatchView::Component::deleteButtons()
{
    for ( int i = 0 ; i < m_buttonArray.size() ; ++i )
    {
        delete m_buttonArray.getUnchecked( i );
    }

    m_buttonArray.clear();
}

void PatchView::Component::paint( juce::Graphics & g )
{ 
//    g.fillAll(juce::Colours::red); 

    int m_arrayX = 220;
    int m_arrayY = 0;
    int height = 30;
    juce::Colour m_fontColor( LUFS_COLOR_FONT );


    int y = m_arrayY - 3;
    g.setColour(m_fontColor);
    for (int i = 0 ; i < m_patch.getLineNames().size() ; ++i)
    {
        juce::String text = m_patch.getLineNames()[ i ];

        juce::Font font(height / 2.f);
        g.setFont(font);
        g.drawText(text, 0, y, m_arrayX, m_lineYInc, juce::Justification::centredRight, false);

        y += m_lineYInc;
    }
}


