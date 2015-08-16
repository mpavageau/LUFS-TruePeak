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

class CustomLookAndFeel : public juce::LookAndFeel_V2
{
public:
    virtual ~CustomLookAndFeel() {}

    virtual void drawPropertyPanelSectionHeader (juce::Graphics& g, const juce::String& name, bool isOpen, int width, int height) override;

    virtual juce::Rectangle<int> getPropertyComponentContentPosition (juce::PropertyComponent& component) override;

    juce::AlertWindow * createAlertWindow (const juce::String& title, const juce::String& message,
                                                    const juce::String& button1, const juce::String& button2, const juce::String& button3,
                                                    juce::AlertWindow::AlertIconType iconType,
                                                    int numButtons, juce::Component* associatedComponent) override;

    virtual void drawButtonBackground (juce::Graphics& g,
                                               juce::Button& button,
                                               const juce::Colour& /*backgroundColour*/,
                                               bool isMouseOverButton,
                                               bool isButtonDown) override;

    virtual void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool /*isMouseOverButton*/, bool /*isButtonDown*/) override;

};


