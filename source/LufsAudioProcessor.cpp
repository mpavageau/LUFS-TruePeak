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

#include "LufsAudioProcessor.h"
#include "LufsTruePeakPluginEditor.h"

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();


void DEBUGPLUGIN_output( const char * _text, ...);

juce::String LufsAudioProcessor::makeAppNameWithVersion()
{
    juce::String appName( JucePlugin_Name );
    appName += " V";
    appName += juce::String( JucePlugin_VersionString );
    return appName;
}


//==============================================================================
LufsAudioProcessor::LufsAudioProcessor()
    : m_lufsProcessor( 6 )
{
    DEBUGPLUGIN_output("LufsAudioProcessor::LufsAudioProcessor");

    juce::PropertiesFile::Options storageParameters;
    storageParameters.applicationName = "LUFS-TruePeak";
    storageParameters.filenameSuffix = "settings";
    storageParameters.osxLibrarySubFolder = "Application Support";
    storageParameters.commonToAllUsers = false;
    storageParameters.ignoreCaseOfKeyNames = true;
    storageParameters.doNotSave = false;
    m_settings.setStorageParameters( storageParameters );

}

LufsAudioProcessor::~LufsAudioProcessor()
{
    DEBUGPLUGIN_output("LufsAudioProcessor::~LufsAudioProcessor");
}

//==============================================================================
int LufsAudioProcessor::getNumParameters()
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getNumParameters returning 0");
    return 0;
}

float LufsAudioProcessor::getParameter (int /*index*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getParameter returning 0.f");
    return 0.f;
}

void LufsAudioProcessor::setParameter (int /*index*/, float /*newValue*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::setParameter");
}

const juce::String LufsAudioProcessor::getParameterName (int /*index*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getParameterName returning empty");
    return juce::String::empty;
}

const juce::String LufsAudioProcessor::getParameterText (int /*index*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getParameterText returning empty");
    return juce::String::empty;
}

//==============================================================================
void LufsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock )
{
    DEBUGPLUGIN_output("LufsAudioProcessor::prepareToPlay");
    m_lufsProcessor.prepareToPlay( sampleRate, samplesPerBlock );
}

void LufsAudioProcessor::releaseResources()
{
    DEBUGPLUGIN_output("LufsAudioProcessor::releaseResources");
}

void LufsAudioProcessor::reset()
{
    DEBUGPLUGIN_output("LufsAudioProcessor::reset");
}

void LufsAudioProcessor::processBlock( juce::AudioSampleBuffer& buffer, juce::MidiBuffer& /*midiMessages*/ )
{
    //DEBUGPLUGIN_output("LufsAudioProcessor::processBlock");
    m_lufsProcessor.processBlock( buffer );
}

//==============================================================================
juce::AudioProcessorEditor* LufsAudioProcessor::createEditor()
{
    DEBUGPLUGIN_output("LufsAudioProcessor::createEditor");
    return new LufsTruePeakPluginEditor (this);
}

//==============================================================================
void LufsAudioProcessor::getStateInformation (juce::MemoryBlock& /*destData*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getStateInformation");
}

void LufsAudioProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
    DEBUGPLUGIN_output("LufsAudioProcessor::setStateInformation");
}

const juce::String LufsAudioProcessor::getInputChannelName (const int channelIndex) const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getInputChannelName");
    switch( channelIndex )
    {
        // VST kSpeakerArr51 is L R C Lfe Ls Rs  
    case 0: return "L";
    case 1: return "R";
    case 2: return "C";
    case 3: return "Lfe";
    case 4: return "Ls";
    case 5: return "Rs";
    }
    return juce::String (channelIndex + 1);
}

const juce::String LufsAudioProcessor::getOutputChannelName (const int channelIndex) const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getOutputChannelName");
    return getInputChannelName( channelIndex );
}

bool LufsAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::isInputChannelStereoPair returning true");
    return true;
}

bool LufsAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::isInputChannelStereoPair returning true");
    return true;
}

bool LufsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    DEBUGPLUGIN_output("LufsAudioProcessor::acceptsMidi returning true");
    return true;
   #else
    DEBUGPLUGIN_output("LufsAudioProcessor::acceptsMidi returning false");
    return false;
   #endif
}

bool LufsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
    DEBUGPLUGIN_output("LufsAudioProcessor::producesMidi returning true");
   #else
    DEBUGPLUGIN_output("LufsAudioProcessor::producesMidi returning false");
    return false;
   #endif
}

bool LufsAudioProcessor::silenceInProducesSilenceOut() const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::silenceInProducesSilenceOut returning false");
    return false;
}

double LufsAudioProcessor::getTailLengthSeconds() const
{
    DEBUGPLUGIN_output("LufsAudioProcessor::getTailLengthSeconds returning 0.0");
    return 0.0;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LufsAudioProcessor();
}
