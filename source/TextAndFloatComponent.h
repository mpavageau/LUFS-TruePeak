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

#include "FloatComponent.h"

#include "JuceDoubleValue.h"

class TextAndFloatComponent 
    : public juce::Component
    , public JuceDoubleValue::Listener
{
public:

    class ValueChangedUpdateObject
    {
    public:
        virtual ~ValueChangedUpdateObject() {};
        virtual void valueChangeUpdate() = 0;
    };

    TextAndFloatComponent(const char * name, juce::Colour color, bool invertedWarning);

    // juce::Component
    virtual void paint( juce::Graphics & g ) override;
    virtual void resized() override;
    virtual void mouseDown (const juce::MouseEvent& event) override;

    void setVolume( const float volume );

    void resetWarning();

    juce::Value m_settingsThresholdVolume;

    float getThresholdVolume() const { return m_thresholdVolume; }

    void setValueChangedUpdateObject(ValueChangedUpdateObject * valueChangedUpdateObject) { m_valueChangedUpdateObject = valueChangedUpdateObject; }

private:

    virtual void juceValueHasChanged(double value) override;

    void setThresholdVolume(float thresholdVolume);

    FloatComponent m_floatComponent;

    void paintFrame( juce::Graphics& g );

    juce::Colour m_color;

    ValueChangedUpdateObject * m_valueChangedUpdateObject;

    float m_thresholdVolume;
    bool m_showWarningFrame;
    bool m_invertedWarning; // true when warning is shown when volume is < than threshold
};


