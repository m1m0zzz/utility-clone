/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "UI/Constant.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/CustomPopupMenu.h"
#include "UI/CustomLabel.h"
#include "UI/KnobSlider.h"
#include "UI/IconButton.h"
#include "UI/MiniTextSlider.h"
#include "UI/ToggleTextButton.h"
#include "UI/TogglePhaseButton.h"

//==============================================================================
/**
*/
class UtilityCloneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UtilityCloneAudioProcessorEditor (UtilityCloneAudioProcessor&, juce::AudioProcessorValueTreeState& vts, juce::UndoManager& um);
    ~UtilityCloneAudioProcessorEditor() override;

    //==============================================================================
    bool keyPressed(const juce::KeyPress& key) override;
    void mouseDown(const juce::MouseEvent& mouseEvent) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateStereoLabel();

    UtilityCloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::UndoManager& undoManager;

    int width = 200;
    int height = 300;
    double ratio = width / height;
    
    // watch parameter for ui
    std::atomic<float>* isMono     = valueTreeState.getRawParameterValue("mono");
    std::atomic<float>* stereoMode = valueTreeState.getRawParameterValue("stereoMode");
    std::atomic<float>* isBassMono = valueTreeState.getRawParameterValue("isBassMono");
    
    CustomPopupMenu menu{ &customLookAndFeel , undoManager, stereoMode, [this]() { this->updateStereoLabel(); } };

    // parameter components
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KnobSlider gainSlider{ &customLookAndFeel, menu };
    TogglePhaseButton invertPhaseLToggleButton{ "L", &customLookAndFeel, menu };
    TogglePhaseButton invertPhaseRToggleButton{ "R", &customLookAndFeel, menu };
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel, menu };
    KnobSlider panSlider{ &customLookAndFeel, menu };
    IconButton stereoModeSwitchButton{ juce::ImageCache::getFromMemory(
        BinaryData::swap_16_16_png,
        BinaryData::swap_16_16_pngSize
    ), menu };
    KnobSlider stereoWidthSlider{ &customLookAndFeel, menu, *isMono != 0 };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel, menu, *isMono != 0 };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel, menu, *isMono != 0 };
    MiniTextSlider bassMonoFrequencySlider{
        valueTreeState, "bassMonoFrequency", &customLookAndFeel, menu, *isMono != 0 || *isBassMono == 0
    };
    IconButton bassMonoListeningButton{ juce::ImageCache::getFromMemory(
        BinaryData::headphone_16_16_png,
        BinaryData::headphone_16_16_pngSize
    ), menu };

    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseLToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseRToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<ButtonAttachment> stereoModeSwitchButtonAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoMidSideSliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> bassMonoFrequencySliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoListeningButtonAttachment;

    // ui components
    juce::Rectangle<int> columnL;
    juce::Rectangle<int> columnR;
    CustomLabel inputLabel{ menu };
    CustomLabel outputLabel{ menu };
    CustomLabel gainLabel{ menu };
    CustomLabel panLabel{ menu };
    CustomLabel stereoModeLabel{ menu };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityCloneAudioProcessorEditor)
};
