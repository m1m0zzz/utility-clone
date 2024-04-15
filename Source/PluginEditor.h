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
    bool isMonoByChannelMode();

    UtilityCloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::UndoManager& undoManager;

    int width = 200;
    int height = 300;
    double ratio = width / height;
    
    // watch parameter for ui
    std::atomic<float>* isMono      = valueTreeState.getRawParameterValue("mono");
    std::atomic<float>* stereoMode  = valueTreeState.getRawParameterValue("stereoMode");
    std::atomic<float>* isBassMono  = valueTreeState.getRawParameterValue("isBassMono");
    std::atomic<float>* channelMode = valueTreeState.getRawParameterValue("channelMode");

    CustomPopupMenu menu{ &customLookAndFeel, valueTreeState, undoManager, [this]() { this->updateStereoLabel(); } };

    // parameter components
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KnobSlider gainSlider{ &customLookAndFeel, menu };
    TogglePhaseButton invertPhaseLToggleButton{ "L", &customLookAndFeel, menu };
    TogglePhaseButton invertPhaseRToggleButton{ "R", &customLookAndFeel, menu };
    juce::ComboBox channelModeComboBox;
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel, menu, isMonoByChannelMode()};
    KnobSlider panSlider{ &customLookAndFeel, menu };
    IconButton stereoModeSwitchButton{ juce::ImageCache::getFromMemory(
        BinaryData::swap_16_16_png,
        BinaryData::swap_16_16_pngSize
    ), menu };
    KnobSlider stereoWidthSlider{ &customLookAndFeel, menu, *isMono != 0 || isMonoByChannelMode() };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel, menu, *isMono != 0 || isMonoByChannelMode() };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel, menu, *isMono != 0 || isMonoByChannelMode() };
    MiniTextSlider bassMonoFrequencySlider{
        valueTreeState, "bassMonoFrequency", &customLookAndFeel, menu, *isMono != 0 || isMonoByChannelMode() || *isBassMono == 0
    };
    IconButton bassMonoListeningButton{ juce::ImageCache::getFromMemory(
        BinaryData::headphone_16_16_png,
        BinaryData::headphone_16_16_pngSize
    ), menu, isMonoByChannelMode(), true };
    ToggleTextButton dcToggleButton{ "DC", &customLookAndFeel, menu };

    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseLToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseRToggleButtonAttachment;
    std::unique_ptr<ComboBoxAttachment> channelModeComboBoxAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<ButtonAttachment> stereoModeSwitchButtonAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoMidSideSliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> bassMonoFrequencySliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoListeningButtonAttachment;
    std::unique_ptr<ButtonAttachment> dcToggleButtonAttachment;

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
