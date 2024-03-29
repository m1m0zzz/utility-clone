/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

const std::unordered_map<std::string, juce::Colour> themeColours = {
    { "grey",       juce::Colour::fromRGB(143, 143, 143) },
    { "lightgrey",  juce::Colour::fromRGB(183, 183, 183) },
    { "blue",       juce::Colour::fromRGB(85, 222, 246) },
    { "orange",     juce::Colour::fromRGB(255, 177, 0) },
    { "lightblack", juce::Colour::fromRGB(42, 42, 42) },
    { "white",      juce::Colour::fromRGB(220, 220,220) },
    { "text",       juce::Colours::black },
};

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    // knob
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;
    // slider textbox
    void drawLabel(juce::Graphics&, juce::Label&) override;
    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class ToggleTextButton : public juce::TextButton
{
public:
    ToggleTextButton(juce::String text, juce::LookAndFeel* lookAndFeel) {
        setClickingTogglesState(true);
        setButtonText(text);
        setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("orange"));
        setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
        setColour(juce::TextButton::ColourIds::textColourOnId, themeColours.at("text"));
        setColour(juce::TextButton::ColourIds::textColourOffId, themeColours.at("text"));
        setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("lightblack"));
        setLookAndFeel(lookAndFeel);
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleTextButton)
};


class KnobSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    KnobSlider(juce::LookAndFeel* lookAndFeel) {
        setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 20);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, themeColours.at("text"));
        setColour(juce::Slider::ColourIds::thumbColourId, themeColours.at("lightblack"));
        setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, themeColours.at("lightblack"));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, themeColours.at("blue"));
        setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("white"));
        setLookAndFeel(lookAndFeel);
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobSlider)
};


class MiniTextSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::String parameterID;
    bool disableFlag = false;

    MiniTextSlider(juce::AudioProcessorValueTreeState& valueTreeState, const juce::String parameterID, juce::LookAndFeel* lookAndFeel = nullptr, bool disableFlag = false) :
        valueTreeState(valueTreeState), parameterID(parameterID), disableFlag(disableFlag)
    {
        setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
        setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::transparentWhite);
        setColour(juce::Slider::ColourIds::textBoxTextColourId, themeColours.at("text"));
        setColour(juce::Slider::ColourIds::textBoxOutlineColourId, themeColours.at("text"));
        setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, themeColours.at("text"));
        setVelocityBasedMode(true);
        setVelocityModeParameters(1.6, 1, 0.09);
        if (lookAndFeel != nullptr) setLookAndFeel(lookAndFeel);
    }

    void paint(juce::Graphics& g) override
    {
        const auto range = valueTreeState.getParameterRange(parameterID);
        const float percent = range.convertTo0to1(*valueTreeState.getRawParameterValue(parameterID));
        g.setColour(themeColours.at("white"));
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColour(themeColours.at("blue"));
        g.fillRect(0, 0, static_cast<int>(getWidth() * percent), getHeight());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniTextSlider)
};


//class SliderTab : public juce::TabbedComponent
//{
//public:
//    using juce::TabbedComponent::TabbedComponent;
//
//    void currentTabChanged(int index, const juce::String& name) override {
//        //DBG("currentTabChanged(" << index << ", " << name << ")");
//        if (onTabChanged) {
//            onTabChanged(index, name);
//        }
//    };
//
//    std::function<void(int index, const juce::String& name)> onTabChanged;
//private:
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderTab)
//};

//==============================================================================
/**
*/
class UtilityCloneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UtilityCloneAudioProcessorEditor (UtilityCloneAudioProcessor&, juce::AudioProcessorValueTreeState& vts, juce::UndoManager& um);
    ~UtilityCloneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    UtilityCloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::UndoManager& undoManager;

    int width = 200;
    int height = 300;
    double ratio = width / height;

    // parameter components   
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KnobSlider gainSlider{ &customLookAndFeel };
    ToggleTextButton invertPhaseToggleButton{ "Invert Phase", &customLookAndFeel };
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel };
    KnobSlider panSlider{&customLookAndFeel};
    KnobSlider stereoWidthSlider{ &customLookAndFeel };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel };
    MiniTextSlider bassMonoFrequencySlider;
    juce::TextButton stereoModeSwitchButton;

    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoMidSideSliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> bassMonoFrequencySliderAttachment;

    // ui components
    //SliderTab stereoTab{ juce::TabbedButtonBar::TabsAtTop };
    juce::Rectangle<int> columnL;
    juce::Rectangle<int> columnR;
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label gainLabel;
    juce::Label panLabel;
    juce::Label stereoModeLabel;
    juce::TextButton undoButton, redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityCloneAudioProcessorEditor)
};
