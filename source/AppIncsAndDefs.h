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

// Juce include files

#include "AppConfig.h"

#include "modules/juce_audio_basics/juce_audio_basics.h"
#include "modules/juce_audio_devices/juce_audio_devices.h"
#include "modules/juce_audio_formats/juce_audio_formats.h"
#include "modules/juce_audio_processors/juce_audio_processors.h"
#include "modules/juce_audio_utils/juce_audio_utils.h"
#include "modules/juce_core/juce_core.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "modules/juce_graphics/juce_graphics.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_gui_extra/juce_gui_extra.h"

//#define TESTCOLORS

#define DEFAULT_MIN_VOLUME ( -100.f )

#define LUFS_EDITOR_WIDTH 870
#define LUFS_EDITOR_HEIGHT 500

#define LUFS_COLOR_BACKGROUND juce::Colour( 0xff1A0001 )
#define LUFS_COLOR_FONT juce::Colour(0xff1A0001).interpolatedWith(juce::Colour(juce::Colours::white), 0.7f )

#define COLOR_LUFSTIME LUFS_COLOR_FONT
#define COLOR_MOMENTARY juce::Colour(0xffFFCC00)
#define COLOR_SHORTTERM juce::Colour(0xff66CBFF)
#define COLOR_INTEGRATED juce::Colour(0xffFF6600) 
#define COLOR_RANGE juce::Colour(0xff1A0001).interpolatedWith(juce::Colour(juce::Colours::white), 0.4f )
#define COLOR_BACKGROUND_GRAPH juce::Colours::darkgrey

