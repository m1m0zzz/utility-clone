/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//class ToggleTextButton : public juce::TextButton
//{
//public:
//    using juce::TextButton::TextButton;
//    ToggleTextButton() {}
//};

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


    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

    juce::TextButton invertPhaseToggleButton;
    std::unique_ptr<ButtonAttachment> invertPhaseToggleButtonAttachment;

    juce::TextButton monoToggleButton;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
