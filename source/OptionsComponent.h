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

class TextAndFloatComponent;

class OptionsComponent
    : public juce::Component
    , public juce::Button::Listener
{
public:

    OptionsComponent( juce::ApplicationProperties & settings,
        juce::Value & momentaryThreshold,
        juce::Value & shortTermThreshold,
        juce::Value & integratedThreshold,
        juce::Value & rangeThreshold,
        juce::Value & truePeakThreshold );

    virtual ~OptionsComponent();

    // juce::Component
    virtual void paint( juce::Graphics & g ) override;

    // juce::Button::Listener
    virtual void buttonClicked( juce::Button* ) override;

private:

    juce::PropertyPanel m_propertyPanel;
    juce::ApplicationProperties & m_settings;
    juce::TextButton m_okButton;
};


