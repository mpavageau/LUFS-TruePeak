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

class TextAndFloatComponent : public juce::Component
{
public:

    TextAndFloatComponent(const char * name, juce::Colour color);

    // juce::Component
    virtual void paint( juce::Graphics & g ) override;
    virtual void resized() override;

    void setVolume( const float volume );

    void setThresholdVolume(float thresholdVolume);

private:

    FloatComponent m_floatComponent;

    void paintFrame( juce::Graphics& g );

    juce::Colour m_color;

    float m_thresholdVolume;
    bool m_showWarningFrame;
};


