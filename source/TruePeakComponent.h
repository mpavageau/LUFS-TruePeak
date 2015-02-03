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
class LufsAudioProcessor;

class TruePeakComponent : public juce::Component
{
public:

    TruePeakComponent();

    // juce::Component
    virtual void paint( juce::Graphics & g );

    inline void setProcessor( LufsAudioProcessor * processor ) { m_processor = processor; }

    void update();

    void reset();

private:

    int getVolumeY( const int height, const float decibels );

    FloatComponent m_valueComponent;
    LufsAudioProcessor * m_processor;
    int m_validSize;
};

