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
        int res = juce::AlertWindow::showOkCancelBox( juce::AlertWindow::NoIcon,
            juce::String("Do you really want to quit?"),
            juce::String::empty, 
            "yes",
            "no" );

        if (res != 1)
            return;

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
        if ( commandLine.length() )
        {
            if ( commandLine.toLowerCase().endsWith( ".wav" ) )
            {
                juce::File file( commandLine );

                if ( file.exists() )
                    AudioProcessing::TestOversampling( file ); // oversamples by 4

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
