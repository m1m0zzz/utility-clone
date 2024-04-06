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
#include "UI/KnobSlider.h"
#include "UI/ListeningButton.h"
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
    UtilityCloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::UndoManager& undoManager;
    juce::PopupMenu menu;

    int width = 200;
    int height = 300;
    double ratio = width / height;
    const juce::URL documentURL = juce::URL("https://github.com/m1m0zzz/utility-clone");

    // watch parameter for ui
    std::atomic<float>* isMono     = valueTreeState.getRawParameterValue("mono");
    std::atomic<float>* stereoMode = valueTreeState.getRawParameterValue("stereoMode");
    std::atomic<float>* isBassMono = valueTreeState.getRawParameterValue("isBassMono");

    // parameter components
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KnobSlider gainSlider{ &customLookAndFeel };
    TogglePhaseButton invertPhaseLToggleButton{ "L", &customLookAndFeel };
    TogglePhaseButton invertPhaseRToggleButton{ "R", &customLookAndFeel };
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel };
    KnobSlider panSlider{&customLookAndFeel};
    juce::TextButton stereoModeSwitchButton;
    KnobSlider stereoWidthSlider{ &customLookAndFeel, *isMono != 0 };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel, *isMono != 0 };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel, *isMono != 0 };
    MiniTextSlider bassMonoFrequencySlider{
        valueTreeState, "bassMonoFrequency", &customLookAndFeel, *isMono != 0 || *isBassMono == 0
    };
    ListeningButton bassMonoListeningButton;

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
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label gainLabel;
    juce::Label panLabel;
    juce::Label stereoModeLabel;
    juce::TextButton undoButton, redoButton;

    void updateStereoLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityCloneAudioProcessorEditor)
};
