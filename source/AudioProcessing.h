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

class AudioProcessing
{
public:

    // oversamples by 4 a wave file using polyphase4 and saves new file to disk
    static void TestOversampling( const juce::File & input );

    // applies simple convolution with polyphase params and saves new file to disk
    static void TestSimpleConvolution( const juce::File & input );

    // TruePeak class calculates True Peak linear volume for buffer

    class TruePeak
    {
    public:
        TruePeak();

        // process: since this method needs numCoeffs values more than buffer size, 
        // numCoeffs values from previous process call are used at beginning of buffer
        void process( const juce::AudioSampleBuffer & buffer );

        float getTruePeakValue() const { return m_truePeakValue; }
        const float * getTruePeakChannelArray() const { return m_truePeakChannelArray; }

        // resets internal buffers 
        void reset();

    private:

        void processPolyphase4AbsMax( const juce::AudioSampleBuffer & buffer );

        juce::AudioSampleBuffer m_inputs; // getPolyphase4AbsMax processes this buffer  
        float m_truePeakChannelArray[ LUFS_TP_MAX_NB_CHANNELS ];
        float m_truePeakValue;
    };

private:

    // signal b must be mono
    static void convolution( const juce::AudioSampleBuffer & a, const juce::AudioSampleBuffer & b, juce::AudioSampleBuffer & result );

    static void polyphase4( const juce::AudioSampleBuffer & source, juce::AudioSampleBuffer & result );
    static float polyphase4ComputeSum( const float * input, int offset, int maxOffset, const float* coefficients, int numCoeff );

};

