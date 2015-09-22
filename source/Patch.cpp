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


