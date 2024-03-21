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
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::Slider gainSlider;
    juce::TextButton invertPhaseToggleButton;
    juce::TextButton monoToggleButton;
    juce::Slider panSlider;
    juce::ComboBox stereoModeComboBox;
    juce::Slider stereoWidthSlider;
    juce::Slider stereoMidSideSlider;
    juce::TextButton bassMonoToggleButton;
    juce::Slider bassMonoFrequencySlider;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<ComboBoxAttachment> stereoModeComboBoxAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoMidSideSliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> bassMonoFrequencySliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
