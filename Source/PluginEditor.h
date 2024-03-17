/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class UtilitycloneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UtilitycloneAudioProcessorEditor (UtilitycloneAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~UtilitycloneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UtilitycloneAudioProcessor& audioProcessor;


    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment; // �ǉ�
    
    juce::AudioProcessorValueTreeState& valueTreeState; // �ǉ�
    juce::Slider gainSlider; // �ǉ�
    std::unique_ptr<SliderAttachment> gainSliderAttachment; // �ǉ�

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
