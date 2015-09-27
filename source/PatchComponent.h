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

#include "Patch.h"

class PatchComponent 
    : public juce::Component
    , public PatchView::Component::Listener
{
public:

    class Listener
    {
    public:
        virtual void patchHasChanged(const PatchComponent * component, const juce::BigInteger & activeLines, const juce::BigInteger & activeColumns) = 0;
    };

    PatchComponent(Patch & patch, juce::Colour backgroundColor, juce::Colour fontColor, bool enableMultiSelection);

    virtual ~PatchComponent();

    // public juce::Component
    void paint( juce::Graphics & g ) override;
    void resized() override;

    // PatchView::Component::Listener
    void patchHasChanged(const juce::BigInteger & activeLines, const juce::BigInteger & activeColumns) override;

    void addListener(PatchComponent::Listener * listener) { m_listeners.add(listener); }

    void redraw();

private:

    void drawTextWithAngle(juce::Graphics & g, float angle, int x, int y, int width, int height, juce::Colour fontColor, const juce::String & text);

    Patch & m_patch;

    PatchView m_patchView;

    juce::Array<Listener*> m_listeners;

    juce::Colour m_backgroundColor;
    juce::Colour m_fontColor;

    int m_arrayX, m_arrayY;
    bool m_columnNamesAreLong;

    static const int m_columnXInc = 45;
    static const int m_lineYInc = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( PatchComponent )
};
