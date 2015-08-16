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

#include "CustomLookAndFeel.h"

void CustomLookAndFeel::drawPropertyPanelSectionHeader (juce::Graphics& g, const juce::String& name,
        bool isOpen, int width, int height)
{
    const float buttonSize = height * 0.75f;
    const float buttonIndent = (height - buttonSize) * 0.5f;

    drawTreeviewPlusMinusBox (g, juce::Rectangle<float> (buttonIndent, buttonIndent, buttonSize, buttonSize), juce::Colours::white, isOpen, false);

    const int textX = (int) (buttonIndent * 2.0f + buttonSize + 2.0f);

    g.setColour (LUFS_COLOR_FONT);
    g.setFont (juce::Font (height * 0.7f, juce::Font::bold));
    g.drawText (name, textX, 0, width - textX - 4, height, juce::Justification::centredLeft, true);
}

juce::Rectangle<int> CustomLookAndFeel::getPropertyComponentContentPosition (juce::PropertyComponent& component)
{
    //const int textW = juce::jmin (200, component.getWidth() / 2);
    const int textW = 2 * component.getWidth() / 5; // larger area for text
    return juce::Rectangle<int> (textW, 1, component.getWidth() - textW - 1, component.getHeight() - 3);
}

juce::AlertWindow * CustomLookAndFeel::createAlertWindow (const juce::String& title, const juce::String& message,
                                                const juce::String& button1, const juce::String& button2, const juce::String& button3,
                                                juce::AlertWindow::AlertIconType iconType,
                                                int numButtons, juce::Component* associatedComponent)
{
    juce::AlertWindow* aw = new juce::AlertWindow (title, message, iconType, associatedComponent);

    aw->setColour(aw->backgroundColourId, LUFS_COLOR_BACKGROUND);
    aw->setColour(aw->textColourId, LUFS_COLOR_FONT);
    aw->setColour(aw->outlineColourId, LUFS_COLOR_FONT);

    if (numButtons == 1)
    {
        aw->addButton (button1, 0,
                        juce::KeyPress (juce::KeyPress::escapeKey),
                        juce::KeyPress (juce::KeyPress::returnKey));
    }
    else
    {
        const juce::KeyPress button1ShortCut ((int) juce::CharacterFunctions::toLowerCase (button1[0]), 0, 0);
        juce::KeyPress button2ShortCut ((int) juce::CharacterFunctions::toLowerCase (button2[0]), 0, 0);
        if (button1ShortCut == button2ShortCut)
            button2ShortCut = juce::KeyPress();

        if (numButtons == 2)
        {
            aw->addButton (button1, 1, juce::KeyPress (juce::KeyPress::returnKey), button1ShortCut);
            aw->addButton (button2, 0, juce::KeyPress (juce::KeyPress::escapeKey), button2ShortCut);
        }
        else if (numButtons == 3)
        {
            aw->addButton (button1, 1, button1ShortCut);
            aw->addButton (button2, 2, button2ShortCut);
            aw->addButton (button3, 0, juce::KeyPress (juce::KeyPress::escapeKey));
        }
    }

    return aw;
}

static juce::Colour createBaseColour (juce::Colour buttonColour,
                                bool hasKeyboardFocus,
                                bool isMouseOverButton,
                                bool isButtonDown) noexcept
{
    //const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
    const float sat = hasKeyboardFocus ? 0.2f : 1.1f;
    const juce::Colour baseColour (buttonColour.withMultipliedSaturation (sat));

    if (isButtonDown)      return baseColour.contrasting (0.2f);
    if (isMouseOverButton) return baseColour.contrasting (0.1f);

    return baseColour;
}

void CustomLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                            juce::Button& button,
                                            const juce::Colour& /*backgroundColour*/,
                                            bool isMouseOverButton,
                                            bool isButtonDown)
{
    const int width = button.getWidth();
    const int height = button.getHeight();

    const float outlineThickness = button.isEnabled() ? ((isButtonDown || isMouseOverButton) ? 1.2f : 0.7f) : 0.4f;
    const float halfThickness = outlineThickness * 0.5f;

    const float indentL = button.isConnectedOnLeft()   ? 0.1f : halfThickness;
    const float indentR = button.isConnectedOnRight()  ? 0.1f : halfThickness;
    const float indentT = button.isConnectedOnTop()    ? 0.1f : halfThickness;
    const float indentB = button.isConnectedOnBottom() ? 0.1f : halfThickness;


    const juce::Colour backgroundColour(juce::Colour(LUFS_COLOR_BACKGROUND).brighter(0.3f));

    const juce::Colour baseColour (createBaseColour (backgroundColour,
                                                                    button.hasKeyboardFocus (true),
                                                                    isMouseOverButton, isButtonDown)
                                .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    drawGlassLozenge (g,
                        indentL,
                        indentT,
                        width - indentL - indentR,
                        height - indentT - indentB,
                        baseColour, outlineThickness, -1.0f,
                        button.isConnectedOnLeft(),
                        button.isConnectedOnRight(),
                        button.isConnectedOnTop(),
                        button.isConnectedOnBottom());
}

void CustomLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
    juce::Font font (getTextButtonFont (button, button.getHeight()));
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                            : juce::TextButton::textColourOffId)
                        .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
    g.setColour(LUFS_COLOR_FONT);

    const int yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
    const int cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
    const int leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

    g.drawFittedText (button.getButtonText(),
                        leftIndent,
                        yIndent,
                        button.getWidth() - leftIndent - rightIndent,
                        button.getHeight() - yIndent * 2,
                        juce::Justification::centred, 2);
}

