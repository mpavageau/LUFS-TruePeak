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

#include "AudioProcessing.h"
#include "LufsTruePeakComponent.h"
#include "OptionsComponent.h"

const juce::String g_windowStateString( "windowState" );

class MainWindow : public juce::DocumentWindow
{
public:
    //==============================================================================
    MainWindow() : juce::DocumentWindow( LufsAudioProcessor::makeAppNameWithVersion(),
                          juce::Colours::lightgrey,
                          juce::DocumentWindow::allButtons,
                          true)
    {
        LufsTruePeakComponent * lufsTruePeakComponent = new LufsTruePeakComponent( true );
        setContentOwned( lufsTruePeakComponent, true );

        // Centre the window on the screen
        centreWithSize( getWidth(), getHeight() );

        setResizable( true, true );
        setResizeLimits( getWidth(), getHeight(), 3000, 2000 );

        // And show it!
        setVisible (true);

        juce::String windowState = lufsTruePeakComponent->getProcessor()->m_settings.getUserSettings()->getValue( g_windowStateString );
        restoreWindowStateFromString( windowState );

        setColour( MainWindow::backgroundColourId, LUFS_COLOR_BACKGROUND );
        setColour( MainWindow::textColourId, LUFS_COLOR_FONT );
    }

    ~MainWindow()
    {
        if ( getContentComponent() )
        {
            const juce::String windowState = getWindowStateAsString();
            LufsTruePeakComponent * component = (LufsTruePeakComponent*)getContentComponent();
            component->getProcessor()->m_settings.getUserSettings()->setValue( g_windowStateString, windowState );
        }
    }

    //==============================================================================
    void closeButtonPressed() override
    {
#ifndef DEBUG
        int res = juce::AlertWindow::showOkCancelBox( juce::AlertWindow::NoIcon,
            juce::String("Do you really want to quit?"),
            juce::String::empty, 
            "yes",
            "no" );

        if (res != 1)
            return;
#endif

        juce::JUCEApplication::quit();
    }
};

//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
    the initialisation and shutdown of the whole application.
*/
class Application : public juce::JUCEApplication
{
public:
    //==============================================================================
    Application() {}

    //==============================================================================
    void initialise (const juce::String& commandLine ) override
    {
#if defined (LUFS_TRUEPEAK_WINDOWS)
        juce::StringArray tokens;
        tokens.addTokens(commandLine, false);

        if ( tokens.size() > 0 && tokens[0].length() )
        {
            if ( tokens[0].toLowerCase().endsWith( ".wav" ) )
            {
                juce::File file( tokens[0] );

                if ( file.exists() )
                {
                    if ( tokens.size() > 1 && tokens[1].length() && tokens[1].getIntValue() > 0)
                    {
//                        int bufferSize = tokens[1].getIntValue();

                        float truePeak = AudioProcessing::ProcessTruePeak( file, 256 );
                        DBG(juce::String("AudioProcessing::ProcessTruePeak 256 ") + juce::String(truePeak, 3));

                        truePeak = AudioProcessing::ProcessTruePeak( file, 65536 );
                        DBG(juce::String("AudioProcessing::ProcessTruePeak 65536 ") + juce::String(truePeak, 3));

                        truePeak = AudioProcessing::ProcessTruePeak( file );
                        DBG(juce::String("AudioProcessing::ProcessTruePeak ENTIRE FILE ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 64 );
                        DBG(juce::String("LufsProcessor::testTruePeak 64 ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 256 );
                        DBG(juce::String("LufsProcessor::testTruePeak 256 ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 512 );
                        DBG(juce::String("LufsProcessor::testTruePeak 512 ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 1024 );
                        DBG(juce::String("LufsProcessor::testTruePeak 1024 ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 2048 );
                        DBG(juce::String("LufsProcessor::testTruePeak 2048 ") + juce::String(truePeak, 3));

                        truePeak = LufsProcessor::testTruePeak( file, 48000, 65536 );
                        DBG(juce::String("LufsProcessor::testTruePeak 65536 ") + juce::String(truePeak, 3));
                    }
                    else
                    {
                        AudioProcessing::TestOversampling( file ); // oversamples by 4
                    }
                }

                    //AudioProcessing::TestSimpleConvolution( file ); // applies oversampling FIR (coefs are a 24 kHz low pass for a 192 kHz wave)

                systemRequestedQuit();
            }
        }
        else
#else
        commandLine; // unused var
#endif
        {
            m_mainWindow = new MainWindow();
        }
    }

    void shutdown() override
    {
        // This method is where you should clear-up your app's resources..

        // The m_mainWindow variable is a ScopedPointer, so setting it to a null
        // pointer will delete the window.
        m_mainWindow = nullptr;
    }

    //==============================================================================
    const juce::String getApplicationName() override
    {
        return "LUFS-TruePeak";
    }

    const juce::String getApplicationVersion() override
    {
        // The ProjectInfo::versionString value is automatically updated by the Jucer, and
        // can be found in the JuceHeader.h file that it generates for our project.
        return "Version 0";//ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed() override
    {
        return true;
    }

    void anotherInstanceStarted (const juce::String& /*commandLine*/) override
    {
    }

private:
    juce::ScopedPointer<MainWindow> m_mainWindow;
};


#ifdef LUFS_TRUEPEAK_APPLICATION
//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (Application)
#endif
