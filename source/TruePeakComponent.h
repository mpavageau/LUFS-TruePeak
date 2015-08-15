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

#include "TextAndFloatComponent.h"

class LufsAudioProcessor;

class TruePeakComponent : public juce::Component
{
public:

    TruePeakComponent( float minChartVolume, float maxChartVolume );

    // juce::Component
    virtual void paint( juce::Graphics & g );

    inline void setProcessor( LufsAudioProcessor * processor ) { m_processor = processor; }

    void update();

    void reset();

    void pause()    { resetVolumeInertia(); }
    void resume()   { resetVolumeInertia(); }
    
    TextAndFloatComponent * getTextAndFloatComponent() { return &m_valueComponent; }

    TextAndFloatComponent m_valueComponent;

private:

    void resetVolumeInertia();
    
    int getVolumeY( const int offsetText, const int height, const float decibels );

    LufsAudioProcessor * m_processor;
    struct InertiaStruct
    {
        float m_decibelVolume;
        int m_index;
        float getCurrentVolume(int index);
    };
    InertiaStruct m_channelInertiaStruct[LUFS_TP_MAX_NB_CHANNELS];
    juce::StringArray m_channelNames;
    int m_validSize;
    float m_minChartVolume;
    float m_maxChartVolume;
};

