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

// Polyphase peak calculation and coefficients are taken from 
// Annex 2 of https://www.itu.int/rec/R-REC-BS.1770/: 
// Guidelines for accurate measurement of “true-peak” level, 


#include "AppIncsAndDefs.h"

#include "AudioProcessing.h"

const float filterPhase0[] =
{
    0.0017089843750f, 0.0109863281250f, -0.0196533203125f, 0.0332031250000f,
    -0.0594482421875f, 0.1373291015625f, 0.9721679687500f, -0.1022949218750f, 
    0.0476074218750f, -0.0266113281250f, 0.0148925781250f, -0.0083007812500f 
};

const float filterPhase1[] =
{
    -0.0291748046875f, 0.0292968750000f, -0.0517578125000f, 0.0891113281250f, 
    -0.1665039062500f, 0.4650878906250f, 0.7797851562500f, -0.2003173828125f,
    0.1015625000000f, -0.0582275390625f, 0.0330810546875f, -0.0189208984375f 
};

const float filterPhase2[] =
{
    -0.0189208984375f, 0.0330810546875f, -0.0582275390625f, 0.1015625000000f, 
    -0.2003173828125f, 0.7797851562500f, 0.4650878906250f, -0.1665039062500f, 
    0.0891113281250f, -0.0517578125000f, 0.0292968750000f, -0.0291748046875f 
};

const float filterPhase3[] =
{
    -0.0083007812500f, 0.0148925781250f, -0.0266113281250f, 0.0476074218750f,
    -0.1022949218750f, 0.9721679687500f, 0.1373291015625f, -0.0594482421875f, 
    0.0332031250000f, -0.0196533203125f, 0.0109863281250f, 0.0017089843750f
};

const float * filterPhaseArray[] = { filterPhase0, filterPhase1, filterPhase2, filterPhase3 };

const int numCoeffs = sizeof(filterPhase0) / sizeof(float);

void AudioProcessing::TestOversampling( const juce::File & input )
{
    juce::AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats();

    juce::AudioFormatReader * reader = audioFormatManager.createReaderFor( input );

    if ( reader != nullptr )
    {
        // read file
        juce::AudioSampleBuffer origin( (int)reader->numChannels, (int)reader->lengthInSamples );
        reader->read( &origin, 0, (int)reader->lengthInSamples, 0, true, true );

        // Use polyphase FIR filter with coefficients for upsampling 4 times at 48KHz
        juce::AudioSampleBuffer output;
        polyphase4( origin, output );


        juce::String outputName = input.getFullPathName().substring(0, input.getFullPathName().length() - 4);
        juce::File outputFile( outputName + "_polyphase4.wav" );
        juce::FileOutputStream * outputStream = new juce::FileOutputStream( outputFile );

        juce::WavAudioFormat wavAudioFormat;

        juce::StringPairArray emptyArray;
        juce::AudioFormatWriter * writer = wavAudioFormat.createWriterFor( 
            outputStream, reader->sampleRate, reader->numChannels, 24, emptyArray, 0 );

        if ( writer != nullptr )
        {
            writer->writeFromAudioSampleBuffer( output, 0, output.getNumSamples() );
            
            delete writer;
        }

        delete reader;
    }
}

void AudioProcessing::convolution( const juce::AudioSampleBuffer & a, const juce::AudioSampleBuffer & b, juce::AudioSampleBuffer & result )
{
    jassert( b.getNumChannels() == 1 );

    int sampleSize = a.getNumSamples() + b.getNumSamples();

    result.setSize( a.getNumChannels(), sampleSize );

    const float * bSignal = b.getArrayOfReadPointers()[ 0 ];
    
    for ( int ch = 0 ; ch < a.getNumChannels() ; ++ch )
    {
        const float * aSignal = a.getArrayOfReadPointers()[ ch ];
        float * res = result.getArrayOfWritePointers()[ ch ];

        for ( int i = 0 ; i < sampleSize ; ++i )
        {
            float sum = 0.f;

            for ( int j = 0 ; j < b.getNumSamples() ; ++j )
            {
                int indexA = i - j;

                if ( ( indexA >= 0 ) && ( indexA < a.getNumSamples() ) )
                    sum += ( aSignal[indexA] * bSignal[j] );
            }

            res[i] = sum;
        }
    }
}

/**
    Upsample by 4 signal (polyphase coefficients are for a 48 kHz signal).

    Processes 4 samples of the output signal in a single loop iteration.
    In one loop, each of the 4 processed samples is filtered with its own set of coefficients

*/
void AudioProcessing::polyphase4( const juce::AudioSampleBuffer & source, juce::AudioSampleBuffer & result )
{
    int sampleSize = source.getNumSamples();

    result.setSize( source.getNumChannels(), 4*source.getNumSamples() + numCoeffs, false, false, true );
    
    for ( int ch = 0 ; ch < source.getNumChannels() ; ++ch )
    {
        const float * input = source.getArrayOfReadPointers()[ ch ];
        float * res = result.getArrayOfWritePointers()[ ch ];

        for ( int i = 0 ; i < sampleSize ; ++i )
        {
            res[4*i]      = polyphase4ComputeSum( input, i, source.getNumSamples(), filterPhase0, numCoeffs );
            res[4*i+1]    = polyphase4ComputeSum( input, i, source.getNumSamples(), filterPhase1, numCoeffs );
            res[4*i+2]    = polyphase4ComputeSum( input, i, source.getNumSamples(), filterPhase2, numCoeffs );
            res[4*i+3]    = polyphase4ComputeSum( input, i, source.getNumSamples(), filterPhase3, numCoeffs );
        }
    }
}

float AudioProcessing::polyphase4ComputeSum( const float * input, int offset, int maxOffset, const float* coefficients, int numCoeff )
{
    float sum = 0.f;

    for ( int j = 0 ; j < numCoeff ; ++j )
    {
        int index = offset - j;
        if ( ( index >= 0 ) && ( index < maxOffset ) )
        {
            sum += ( input[index] * coefficients[j] );
        }
    }

    return sum;
}


AudioProcessing::TruePeak::TruePeak()
{

}

void AudioProcessing::TruePeak::process( const juce::AudioSampleBuffer & buffer )
{
    if (m_inputs.getNumSamples() >= numCoeffs)
    {
        // we have enough data from a previous process 

        for ( int ch = 0 ; ch < buffer.getNumChannels() ; ++ch )
        {
            m_inputs.copyFrom( ch, 0, &m_inputs.getArrayOfReadPointers()[ch][m_inputs.getNumSamples() - numCoeffs], numCoeffs );
        }

        // resize if necessary
        if ( m_inputs.getNumSamples() != numCoeffs + buffer.getNumSamples() )
            m_inputs.setSize( buffer.getNumChannels(), numCoeffs + buffer.getNumSamples(), true, false, true );
    }
    else
    {
        // setSize clears buffer content too

        m_inputs.setSize( buffer.getNumChannels(), numCoeffs + buffer.getNumSamples(), false, true );
    }

    // copy buffer to inputs with numCoefs offset
    for ( int ch = 0 ; ch < buffer.getNumChannels() ; ++ch )
    {
        m_inputs.copyFrom( ch, numCoeffs, buffer, ch, 0, buffer.getNumSamples() );
    }

    processPolyphase4AbsMax( m_inputs );
}

void AudioProcessing::TruePeak::reset()
{
    m_inputs.setSize(0, 0);
}

void AudioProcessing::TruePeak::processPolyphase4AbsMax( const juce::AudioSampleBuffer & buffer )
{
    // reset current tru peak
    m_truePeakValue = 0.f;
    memset( m_truePeakChannelArray, 0, sizeof( m_truePeakChannelArray ) );

    int sampleSize = buffer.getNumSamples();

    for ( int ch = 0 ; ch < buffer.getNumChannels() ; ++ch )
    {
        const float * input = buffer.getArrayOfReadPointers()[ ch ];

        for ( int i = 0 ; i < sampleSize ; ++i )
        {
            for ( int j = 0 ; j < 4 ; ++j ) // number of polyphase filters
            {
                float absSample = fabs( polyphase4ComputeSum( input, i, buffer.getNumSamples(), filterPhaseArray[j], numCoeffs ) );

                if ( absSample > m_truePeakValue )
                    m_truePeakValue = absSample;

                if ( absSample > m_truePeakChannelArray[ch] )
                    m_truePeakChannelArray[ch] = absSample;
            }
        }
    }
}

/**
    Applies polyphase FIR filter 

    Coefficients create a 24 kHz low pass filter for a 192 kHz wave
*/
void AudioProcessing::TestSimpleConvolution( const juce::File & input )
{
    const int polyphase4Size = sizeof(filterPhase0) / sizeof(float);
    const int convolutionSize = 4 * polyphase4Size;
    juce::AudioSampleBuffer convolutionFilter( 1, convolutionSize );

    float * data = convolutionFilter.getWritePointer( 0 );
    for ( int i = 0 ; i < polyphase4Size ; ++ i )
    {
        *data++ = filterPhase0[ i ];
        *data++ = filterPhase1[ i ];
        *data++ = filterPhase2[ i ];
        *data++ = filterPhase3[ i ];
    }

    juce::AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats();

    juce::AudioFormatReader * reader = audioFormatManager.createReaderFor( input );

    if ( reader != nullptr )
    {
        // read file
        juce::AudioSampleBuffer origin( (int)reader->numChannels, (int)reader->lengthInSamples );
        reader->read( &origin, 0, (int)reader->lengthInSamples, 0, true, true );

        // Convolve
        juce::AudioSampleBuffer output;
        convolution( origin, convolutionFilter, output );
        output.applyGain(0.25f); // filter values use sample with 3 zeros per valid sample (1 / 4)

        juce::String outputName = input.getFullPathName().substring(0, input.getFullPathName().length() - 4);
        juce::File outputFile( outputName + "_convolution.wav" );
        juce::FileOutputStream * outputStream = new juce::FileOutputStream( outputFile );

        juce::WavAudioFormat wavAudioFormat;

        juce::StringPairArray emptyArray;
        juce::AudioFormatWriter * writer = wavAudioFormat.createWriterFor( 
            outputStream, reader->sampleRate, reader->numChannels, 24, emptyArray, 0 );

        if ( writer != nullptr )
        {
            writer->writeFromAudioSampleBuffer( output, 0, output.getNumSamples() );

            delete writer;
        }

        delete reader;
    }
}
