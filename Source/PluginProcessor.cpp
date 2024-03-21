/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -100.0f, 35.0f, 0.0f),
            std::make_unique<juce::AudioParameterBool>("invertPhase", "Invert Phase", false),
            std::make_unique<juce::AudioParameterBool>("mono", "Mono", false),
            std::make_unique<juce::AudioParameterFloat>("pan", "Pan", -50.0f, 50.0f, 0.0f),
            std::make_unique<juce::AudioParameterChoice>("stereoMode", "Stereo Mode", stereoModeList, 0),
            std::make_unique<juce::AudioParameterFloat>("stereoWidth", "Width", 0.0f, 400.0f, 100.0f),
            std::make_unique<juce::AudioParameterFloat>("stereoMidSide", "Mid/Side", -100.0f, 100.0f, 0.0f),
            std::make_unique<juce::AudioParameterBool>("isBassMono", "Bass Mono", false),
            std::make_unique<juce::AudioParameterFloat>("bassMonoFrequency", "Bass Mono freq", 50.0f, 500.0f, 120.0f),
        })
{
    gain              = parameters.getRawParameterValue("gain");
    isInvertPhase     = parameters.getRawParameterValue("invertPhase");
    isMono            = parameters.getRawParameterValue("mono");
    pan               = parameters.getRawParameterValue("pan");
    stereoMode        = parameters.getRawParameterValue("stereoMode");
    stereoWidth       = parameters.getRawParameterValue("stereoWidth");
    stereoMidSide     = parameters.getRawParameterValue("stereoMidSide");
    isBassMono        = parameters.getRawParameterValue("isBassMono");
    bassMonoFrequency = parameters.getRawParameterValue("bassMonoFrequency");
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
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;

    lrFilter.prepare(spec);

    gainDSP.prepare(spec);
    gainDSP.setRampDurationSeconds(0.005); // should consider arguments or using SmoothValue

    pannerDSP.prepare(spec);
    pannerDSP.setRule(juce::dsp::PannerRule::sin3dB);
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
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // phase
    auto phase = *isInvertPhase ? -1.0f : 1.0f;
    buffer.applyGain(phase);

    // stereo
    if (totalNumInputChannels == 2 && !(*isMono))
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto mid = (leftChannel[i] + rightChannel[i]);
            auto side = (rightChannel[i] - leftChannel[i]);

            if (*stereoMode == 0) { // Width (0 to 400)
                mid  *= 0.5f; // no change
                side *= 0.5f * *stereoWidth / 100;
            }
            else { // Mid/Side (-100 to 100)
                float normalizedMS = (*stereoMidSide / 100 + 1.0f) * 0.5f; // 0 to 1
                float divVolume = abs(*stereoMidSide / 100) + 1.0f; // 1 to 2
                mid  *= (1.0 - normalizedMS) / divVolume;
                side *=        normalizedMS  / divVolume;
            }

            leftChannel[i] = (mid - side);
            rightChannel[i] = (mid + side);
        }
    }

    // mono
    if (*isMono) {
        // cf. https://forum.juce.com/t/how-do-i-sum-stereo-to-mono/37579/8
        buffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
        buffer.applyGain(0.5f);
    }

    // bass mono
    DBG("Bass Mono = " << *isBassMono);
    DBG("BM Freq   = " << *bassMonoFrequency);
    lrFilter.setCutoffFrequency(*bassMonoFrequency);
    if (*isBassMono && !(*isMono)) {
        // TODO
        juce::AudioSampleBuffer lowOutput;
        juce::AudioSampleBuffer highOutput;

        lowOutput.makeCopyOf(buffer);
        highOutput.makeCopyOf(buffer);

        auto* lowOutputL = lowOutput.getWritePointer(0);
        auto* lowOutputR = lowOutput.getWritePointer(1);
        auto* highOutputL = highOutput.getWritePointer(0);
        auto* highOutputR = highOutput.getWritePointer(1);

        auto* inputL = buffer.getWritePointer(0);
        auto* inputR = buffer.getWritePointer(1);

        // process filter
        for (int i = 0; i < numSamples; ++i) {
            lrFilter.processSample(0, inputL[i], lowOutputL[i], highOutputL[i]);
            lrFilter.processSample(1, inputR[i], lowOutputR[i], highOutputR[i]);
        }

        // make lowOutput mono
        lowOutput.addFrom( 0, 0, lowOutput, 1, 0, numSamples);
        lowOutput.copyFrom(1, 0, lowOutput, 0, 0, numSamples);
        lowOutput.applyGain(0.5f);

        buffer.clear();
        for (int channel = 0; channel < totalNumInputChannels; channel++)
        {
            buffer.addFrom(channel, 0, lowOutput, channel, 0, numSamples);
            buffer.addFrom(channel, 0, highOutput, channel, 0, numSamples);
        }
    }
    
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    gainDSP.setGainDecibels(*gain);
    pannerDSP.setPan(*pan / 50.0f);

    gainDSP.process(context);
    pannerDSP.process(context);
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
