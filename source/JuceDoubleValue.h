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

#pragma once 

class JuceDoubleValue 
    : public juce::Value
    , private juce::Value::Listener
{
public:

    JuceDoubleValue(juce::PropertiesFile * properties, const char * nameInSettings, double defaultValue);

    class Listener
    {
    public:
        virtual void juceValueHasChanged(double /*value*/) {}
    };

    void addListener(Listener * listener);
    void removeListener(Listener * listener);

private:
    void valueChanged(juce::Value& value);
    
    juce::PropertiesFile * m_properties;
    juce::String m_nameInSettings;
    double m_doubleValue;
    juce::Array<Listener*> m_listeners;
};


