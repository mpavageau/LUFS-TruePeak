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

#include "LufsAudioProcessor.h"

#include "FloatComponent.h"
#include "TimeComponent.h"
#include "TruePeakComponent.h"
#include "Chart.h"



//==============================================================================
/** This is the editor component that our filter will display.
*/
class LufsTruePeakPluginEditor  
    : public juce::AudioProcessorEditor
    , public juce::Timer
    , public juce::Button::Listener

{
public:
    LufsTruePeakPluginEditor (LufsAudioProcessor* ownerFilter);
    ~LufsTruePeakPluginEditor();

    //==============================================================================
    void timerCallback();
    void paint (juce::Graphics& g);
    void resized();

    // Button::Listener 
    virtual void buttonClicked (juce::Button* button);

    LufsAudioProcessor* getProcessor() const
    {
        return static_cast <LufsAudioProcessor*> (getAudioProcessor());
    }

private:

    void exportToText( bool useCommasForDigitSeparation );

    TimeComponent m_timeComponent;
    FloatComponent m_momentaryComponent;
    FloatComponent m_shortTermComponent;
    FloatComponent m_integratedComponent;
    FloatComponent m_rangeComponent;

    TruePeakComponent m_truePeakComponent;

    juce::TextButton m_resetButton;
    juce::TextButton m_pauseButton;
    juce::TextButton m_exportButton;
    juce::TextButton m_aboutButton;

    juce::ScopedPointer<juce::ResizableCornerComponent> resizer;
    juce::ComponentBoundsConstrainer resizeLimits;

    juce::AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    ChartView m_chart;

    bool m_internallyPaused;
};


