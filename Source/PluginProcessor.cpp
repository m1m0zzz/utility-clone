/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define M_PI 3.141592653589793238L

//==============================================================================
UtilitycloneAudioProcessor::UtilitycloneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    , parameters(*this, nullptr, juce::Identifier("Utility-clone"),
        {
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 2.0f, 1.0f),
            std::make_unique<juce::AudioParameterBool>("invertPhase", "Invert Phase", false),
            std::make_unique<juce::AudioParameterBool>("mono", "Mono", false),
            std::make_unique<juce::AudioParameterFloat>("pan", "Pan", -1.0f, 1.0f, 0.0f),
        })
{
    gain = parameters.getRawParameterValue("gain");
    isInvertPhase = parameters.getRawParameterValue("invertPhase");
    isMono = parameters.getRawParameterValue("mono");
    pan = parameters.getRawParameterValue("pan");
}

UtilitycloneAudioProcessor::~UtilitycloneAudioProcessor()
{
}

//==============================================================================
const juce::String UtilitycloneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UtilitycloneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UtilitycloneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UtilitycloneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UtilitycloneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UtilitycloneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UtilitycloneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UtilitycloneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UtilitycloneAudioProcessor::getProgramName (int index)
{
    return {};
}

void UtilitycloneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UtilitycloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    previousGain = *gain;
    float normalisedPan = 0.5f * (*pan + 1.0f);
    float leftValue = std::sin(0.5 * M_PI * (1.0 - normalisedPan));
    float rightValue = std::sin(0.5 * M_PI * normalisedPan);
    previousPanLeftValue  = leftValue;
    previousPanRightValue = rightValue;
}

void UtilitycloneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UtilitycloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UtilitycloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto phase = *isInvertPhase ? -1.0f : 1.0f;
    auto currentGain = *gain * phase;

    // mono
    if (*isMono) {
        // cf. https://forum.juce.com/t/how-do-i-sum-stereo-to-mono/37579/8
        buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
        buffer.applyGain(0.5f);
    }

    // gain
    if (juce::approximatelyEqual(currentGain, previousGain)) {
        buffer.applyGain(currentGain);
    } else {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }

    // pan
    /*if (getTotalNumInputChannels() != 2) {
        return;
    }*/

    // sin 3 dB
    float normalisedPan = 0.5f * (*pan + 1.0f);
    float leftValue  = std::sin(0.5 * M_PI * (1.0 - normalisedPan));
    float rightValue = std::sin(0.5 * M_PI * normalisedPan);

    // pan: L channel
    if (juce::approximatelyEqual(leftValue, previousPanLeftValue)) {
        DBG("L: nomal");
        buffer.applyGain(0, 0, buffer.getNumSamples(), leftValue * boostValue);
    }
    else {
        DBG("L: ramp");
        buffer.applyGainRamp(0, 0, buffer.getNumSamples(), previousPanLeftValue, leftValue * boostValue);
        previousPanLeftValue = leftValue;
    }

    // pan: R channel
    if (juce::approximatelyEqual(rightValue, previousPanRightValue)) {
        DBG("R: nomal");
        buffer.applyGain(1, 0, buffer.getNumSamples(), rightValue * boostValue);
    }
    else {
        DBG("R: ramp");
        buffer.applyGainRamp(1, 0, buffer.getNumSamples(), rightValue * boostValue);
        previousPanRightValue = rightValue;
    }
}

//==============================================================================
bool UtilitycloneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UtilitycloneAudioProcessor::createEditor()
{
    return new UtilitycloneAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void UtilitycloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UtilitycloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UtilitycloneAudioProcessor();
}
