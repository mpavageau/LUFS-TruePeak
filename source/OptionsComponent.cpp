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

#include "OptionsComponent.h"

#include "TextAndFloatComponent.h"


// Warning colored frame and vertical line for True Peak values: -1

class SliderComponent : public juce::SliderPropertyComponent
{
public:
    SliderComponent(juce::Value & value, const juce::String &text, float minVolume, float maxVolume)
        : SliderPropertyComponent(value, text, minVolume, maxVolume, 0.001)
    {
        // slider has following colors:
        // backgroundColourId, thumbColourId,
        // trackColourId
        // rotarySliderFillColourId, rotarySliderOutlineColourId
        // textBoxTextColourId
        // textBoxBackgroundColourId, textBoxHighlightColourId, textBoxOutlineColourId
        slider.setColour(slider.textBoxTextColourId, juce::Colours::red); 
        slider.setColour(slider.textBoxBackgroundColourId, LUFS_COLOR_BACKGROUND); 
        slider.setColour(slider.textBoxHighlightColourId, LUFS_COLOR_FONT); 
        slider.setColour(slider.textBoxOutlineColourId, LUFS_COLOR_FONT); 

        slider.setColour(slider.thumbColourId, juce::Colour(LUFS_COLOR_BACKGROUND).brighter(0.3f)); // value LUFS_COLOR_FONT
        slider.setColour(slider.backgroundColourId, LUFS_COLOR_BACKGROUND);
        slider.setColour(slider.textBoxTextColourId, LUFS_COLOR_FONT);

        // property component has backgroundColourId and labelTextColourId
        setColour(labelTextColourId, LUFS_COLOR_FONT);
        setColour(backgroundColourId, LUFS_COLOR_BACKGROUND);
    }
    
    void setValue (double newValue) override
    {
        slider.setValue (newValue);
    }
};

OptionsComponent::OptionsComponent( juce::ApplicationProperties & settings,
        juce::Value & momentaryThreshold,
        juce::Value & shortTermThreshold,
        juce::Value & integratedThreshold,
        juce::Value & rangeThreshold,
        juce::Value & truePeakThreshold )
    : m_settings( settings )
{
    setSize( 1100, 250 );
    
    m_okButton.setButtonText( juce::String( "OK" ) );
    m_okButton.setColour( juce::TextButton::buttonColourId, LUFS_COLOR_BACKGROUND );
    m_okButton.setColour( juce::TextButton::buttonOnColourId, LUFS_COLOR_FONT );
    m_okButton.setColour( juce::TextButton::textColourOffId, LUFS_COLOR_FONT );
    m_okButton.setColour( juce::TextButton::textColourOnId, LUFS_COLOR_BACKGROUND );
    m_okButton.setBounds( 600, 175, 100, 50 );
    addAndMakeVisible( &m_okButton );
    m_okButton.addListener( this );
    m_okButton.setWantsKeyboardFocus(true); // doesn't get focus :(

    juce::Array<juce::PropertyComponent*> truePeakComponents;

    SliderComponent * momentaryComponent = new SliderComponent(momentaryThreshold, "Threshold for Momentary volume", -30.f, 0.f);
    truePeakComponents.add(momentaryComponent);

    SliderComponent * shortTermComponent = new SliderComponent(shortTermThreshold, "Threshold for Short Term volume", -30.f, 0.f);
    truePeakComponents.add(shortTermComponent);

    SliderComponent * integratedComponent = new SliderComponent(integratedThreshold, "Threshold for Integrated volume", -30.f, 0.f);
    truePeakComponents.add(integratedComponent);

    SliderComponent * rangeComponent = new SliderComponent(rangeThreshold, "Threshold for volume Range", 5.f, 40.f);
    truePeakComponents.add(rangeComponent);

    SliderComponent * truePeakComponent = new SliderComponent(truePeakThreshold, "Threshold for True Peak values (colored frame and red vertical lines)", -10.f, 10.f);
    truePeakComponents.add(truePeakComponent);

    addAndMakeVisible(m_propertyPanel);
    m_propertyPanel.addSection("Define the threshold volumes for each volume value", truePeakComponents);

    const int offset = 4;
    m_propertyPanel.setBounds(offset, offset, getWidth() - 2 * offset, 175);
}

OptionsComponent::~OptionsComponent()
{
}

void OptionsComponent::paint( juce::Graphics & g )
{
    g.fillAll( LUFS_COLOR_BACKGROUND );
}

void OptionsComponent::buttonClicked( juce::Button * _button ) 
{
    if ( _button == &m_okButton )
    {
        juce::DialogWindow * window = findParentComponentOfClass<juce::DialogWindow>();

        if ( window != nullptr )
            window->exitModalState( 51 );
    }
}

