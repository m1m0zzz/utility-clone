/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::StringArray stereoModeList = juce::StringArray("Width", "Mid/Side");

//==============================================================================
/**
*/
class UtilitycloneAudioProcessor  : public juce::AudioProcessor
{
public:

    //==============================================================================
    UtilitycloneAudioProcessor();
    ~UtilitycloneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioProcessorValueTreeState parameters;

    juce::dsp::ProcessSpec spec;
    juce::dsp::Gain<float> gainDSP;
    juce::dsp::Panner<float> pannerDSP;

    std::atomic<float>* gain = nullptr;
    std::atomic<float>* isInvertPhase = nullptr;
    std::atomic<float>* isMono = nullptr;
    std::atomic<float>* pan = nullptr;
    std::atomic<float>* stereoMode = nullptr; // Width or Mid/Side
    std::atomic<float>* stereoWidth = nullptr;
    std::atomic<float>* stereoMidSide = nullptr;

    std::atomic<float>* isBassMono = nullptr;
    std::atomic<float>* bassMonoFrequency = nullptr;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessor)
};
