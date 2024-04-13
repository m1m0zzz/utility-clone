/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

template <typename T>
T calcSkew(T min, T max) {
    T mid = sqrt(min * max);
    return static_cast<T>(log10(0.5)) / log10((mid - min) / (max - min));
}

//==============================================================================
UtilityCloneAudioProcessor::UtilityCloneAudioProcessor()
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
    , parameters(*this, &undoManager, juce::Identifier("Utility-clone"),
        {
            std::make_unique<juce::AudioParameterFloat>(
                "gain", "Gain", juce::NormalisableRange(-100.0f, 35.0f), 0.0f, "Gain", juce::AudioProcessorParameter::genericParameter,
                [](float value, int) {
                    int digit = static_cast<int>(log10(abs(value)));
                    return (value <= -100.0f) ? "-inf" : juce::String(value, 2 - (digit < 0 ? 0 : digit));
                }
            ),
            std::make_unique<juce::AudioParameterBool>("invertPhaseL", "Invert Phase L", false),
            std::make_unique<juce::AudioParameterBool>("invertPhaseR", "Invert Phase R", false),
            std::make_unique<juce::AudioParameterBool>("mono", "Mono", false),
            std::make_unique<juce::AudioParameterFloat>(
                "pan", "Pan", juce::NormalisableRange(-50.0f, 50.0f, 1.0f), 0.0f, "Pan", juce::AudioProcessorParameter::genericParameter,
                [](float value, int) {
                    return (value == 0) ? "C" : (
                        juce::String(abs(value)) + ((value < 0) ? "L" : "R")
                    );
                }
            ),
            std::make_unique<juce::AudioParameterChoice>("stereoMode", "Stereo Mode", stereoModeList, 0),
            std::make_unique<juce::AudioParameterFloat>("stereoWidth", "Width", juce::NormalisableRange(0.0f, 400.0f, 1.0f), 100.0f),
            std::make_unique<juce::AudioParameterFloat>(
                "stereoMidSide", "Mid/Side", juce::NormalisableRange(-100.0f, 100.0f, 1.0f), 0.0f, "Mid/Side", juce::AudioProcessorParameter::genericParameter,
                [](float value, int) {
                    return (value == 0) ? "0" : (
                        juce::String(abs(value)) + ((value < 0) ? "M" : "S")
                    );
                }
            ),
            std::make_unique<juce::AudioParameterBool>("isBassMono", "Bass Mono", false),
            std::make_unique<juce::AudioParameterFloat>(
                "bassMonoFrequency", "Bass Mono freq",
                juce::NormalisableRange(50.0f, 500.0f, 1.0f, calcSkew(50.0f, 500.0f), false),
                120.0f
            ),
            std::make_unique<juce::AudioParameterBool>("isBassMonoListening", "Bass Mono Listening", false),
        })
{
    gain                = parameters.getRawParameterValue("gain");
    isInvertPhaseL      = parameters.getRawParameterValue("invertPhaseL");
    isInvertPhaseR      = parameters.getRawParameterValue("invertPhaseR");
    isMono              = parameters.getRawParameterValue("mono");
    pan                 = parameters.getRawParameterValue("pan");
    stereoMode          = parameters.getRawParameterValue("stereoMode");
    stereoWidth         = parameters.getRawParameterValue("stereoWidth");
    stereoMidSide       = parameters.getRawParameterValue("stereoMidSide");
    isBassMono          = parameters.getRawParameterValue("isBassMono");
    bassMonoFrequency   = parameters.getRawParameterValue("bassMonoFrequency");
    isBassMonoListening = parameters.getRawParameterValue("isBassMonoListening");
}

UtilityCloneAudioProcessor::~UtilityCloneAudioProcessor()
{
}

//==============================================================================
const juce::String UtilityCloneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UtilityCloneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UtilityCloneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UtilityCloneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UtilityCloneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UtilityCloneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UtilityCloneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UtilityCloneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UtilityCloneAudioProcessor::getProgramName (int index)
{
    return {};
}

void UtilityCloneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UtilityCloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;

    width.reset(sampleRate, 0.001);
    midSide.reset(sampleRate, 0.001);

    lrFilter.prepare(spec);

    gainDSP.prepare(spec);
    gainDSP.setRampDurationSeconds(0.005); // should consider arguments or using SmoothValue

    pannerDSP.prepare(spec);
    pannerDSP.setRule(juce::dsp::PannerRule::sin3dB);
}

void UtilityCloneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UtilityCloneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UtilityCloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // phase
    auto phaseL = *isInvertPhaseL ? -1.0f : 1.0f;
    auto phaseR = *isInvertPhaseR ? -1.0f : 1.0f;
    buffer.applyGain(0, 0, numSamples, phaseL);
    if (totalNumInputChannels == 2)
        buffer.applyGain(1, 0, numSamples, phaseR);

    // stereo
    //DBG("stereoMode: " << *stereoMode);
    width.setTargetValue(*stereoWidth);
    midSide.setTargetValue(*stereoMidSide);
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
                side *= 0.5f * width.getNextValue() / 100;
            }
            else { // Mid/Side (-100 to 100)
                float normalizedMS = (midSide.getNextValue() / 100 + 1.0f) * 0.5f; // 0 to 1
                float divVolume = abs(midSide.getNextValue() / 100) + 1.0f; // 1 to 2
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
    //DBG("Bass Mono = " << *isBassMono);
    //DBG("BM Freq   = " << *bassMonoFrequency);
    //DBG("BM L = " << *isBassMonoListening);
    lrFilter.setCutoffFrequency(*bassMonoFrequency);
    if ((*isBassMono && !*isMono) || *isBassMonoListening) {
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
        if (*isBassMono) {
            lowOutput.addFrom( 0, 0, lowOutput, 1, 0, numSamples);
            lowOutput.copyFrom(1, 0, lowOutput, 0, 0, numSamples);
            lowOutput.applyGain(0.5f);
        }

        buffer.clear();
        for (int channel = 0; channel < totalNumInputChannels; channel++)
        {
            buffer.addFrom(channel, 0, lowOutput, channel, 0, numSamples);
            if (!*isBassMonoListening) {
                buffer.addFrom(channel, 0, highOutput, channel, 0, numSamples);
            }
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
bool UtilityCloneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UtilityCloneAudioProcessor::createEditor()
{
    return new UtilityCloneAudioProcessorEditor (*this, parameters, undoManager);
}

//==============================================================================
void UtilityCloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void UtilityCloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UtilityCloneAudioProcessor();
}
