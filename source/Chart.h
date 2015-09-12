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

class LufsAudioProcessor;
class ChartView;

class Chart : public juce::Component
{
public:
    Chart( float _minChartVolume, float _maxChartVolume );

    void update();

    // juce::Component
    virtual void paint( juce::Graphics& g );

    inline void setProcessor( LufsAudioProcessor * processor ) { m_processor = processor; }
    int getVolumeY( const int height, const float decibels );

    inline void setChartView( ChartView * _chartView ) { m_chartView = _chartView; }

    float getMinChartVolume() const { return m_minChartVolume; }
    float getMaxChartVolume() const { return m_maxChartVolume; }

    void setTruePeakThreshold( float truePeakThreshold );

private:

    void paintValues( juce::Graphics& g, const juce::Colour _color, const float * _data, const int _itemsPerPixel, const int _offset, const int _pixels );
    void paintTruePeakLines( juce::Graphics& g, const float * _data, const int _offset, const int _pixels );

    LufsAudioProcessor * m_processor;
    ChartView * m_chartView;
    float m_minChartVolume;
    float m_maxChartVolume;
    float m_truePeakThreshold;
    int m_validSize;
};

class ChartView : public juce::Viewport
{
public:
    ChartView( float _minChartVolume, float _maxChartVolume );

    // juce::Component
    virtual void resized();
    virtual void mouseWheelMove( const juce::MouseEvent& event, const juce::MouseWheelDetails & wheel ) override;

    void resetScrolling();

    // juce::Viewport
    // virtual void visibleAreaChanged( const juce::Rectangle<int>& newVisibleArea );

    inline void setProcessor( LufsAudioProcessor * processor ) { m_chart.setProcessor( processor ); }

    void update();

    void setTruePeakThreshold( float truePeakThreshold ) { m_chart.setTruePeakThreshold( truePeakThreshold ); }

    Chart m_chart;
};


