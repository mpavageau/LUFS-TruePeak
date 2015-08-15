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

#include "JuceDoubleValue.h"

JuceDoubleValue::JuceDoubleValue(juce::PropertiesFile * properties, const char * nameInSettings, double defaultValue)
    : m_properties(properties)
    , m_nameInSettings(nameInSettings)
{
    m_doubleValue = m_properties->getDoubleValue(m_nameInSettings, defaultValue);

    (static_cast<juce::Value*>(this))->setValue(m_doubleValue);

    (static_cast<juce::Value*>(this))->addListener(this);
}

void JuceDoubleValue::valueChanged( juce::Value& value )
{
    m_properties->setValue(m_nameInSettings, value);

    for (int i = 0 ; i < m_listeners.size() ; ++i)
        m_listeners.getUnchecked(i)->juceValueHasChanged((double)value.getValue());
}

void JuceDoubleValue::addListener(Listener * listener)
{
    m_listeners.add(listener);

    listener->juceValueHasChanged((double)getValue());
}

void JuceDoubleValue::removeListener(Listener * listener)
{
    m_listeners.removeAllInstancesOf(listener);
}
