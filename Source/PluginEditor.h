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


    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment; // 追加
    
    juce::AudioProcessorValueTreeState& valueTreeState; // 追加
    juce::Slider gainSlider; // 追加
    std::unique_ptr<SliderAttachment> gainSliderAttachment; // 追加

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
