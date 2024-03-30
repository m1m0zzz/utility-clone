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
    { "disabled",   juce::Colour::fromRGB(105, 105,105) },
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
    bool disabled; // show disabled color

    ToggleTextButton(juce::String text, juce::LookAndFeel* lookAndFeel, bool disabled = false) : disabled(disabled) {
        setClickingTogglesState(true);
        setButtonText(text);
        setAndUpdateDisabled(disabled);
        setLookAndFeel(lookAndFeel);
    }

    void updateColourAll() {
        if (!disabled) {
            setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("orange"));
            setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
            setColour(juce::TextButton::ColourIds::textColourOnId, themeColours.at("text"));
            setColour(juce::TextButton::ColourIds::textColourOffId, themeColours.at("text"));
            setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("lightblack"));
        }
        else {
            setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("white"));
            setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
            setColour(juce::TextButton::ColourIds::textColourOnId, themeColours.at("disabled"));
            setColour(juce::TextButton::ColourIds::textColourOffId, themeColours.at("disabled"));
            setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("lightblack"));
        }
    }

    void updateDisabled() {
        updateColourAll();
        repaint();
    }

    void setAndUpdateDisabled(bool flag) {
        disabled = flag;
        updateDisabled();
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleTextButton)
};


class KnobSlider : public juce::Slider
{
public:
    using juce::Slider::Slider;

    bool disabled; // show disabled color

    KnobSlider(juce::LookAndFeel* lookAndFeel, bool disabled = false) : disabled(disabled) {
        setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 20);
        setAndUpdateDisabled(disabled);
        setLookAndFeel(lookAndFeel);
    }
    
    void updateColourAll() {
        setColour(juce::Slider::ColourIds::textBoxTextColourId, themeColours.at("text"));
        if (!disabled) {
            setColour(juce::Slider::ColourIds::thumbColourId, themeColours.at("lightblack"));
            setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, themeColours.at("lightblack"));
            setColour(juce::Slider::ColourIds::rotarySliderFillColourId, themeColours.at("blue"));
            setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("white"));
        }
        else {
            setColour(juce::Slider::ColourIds::thumbColourId, themeColours.at("disabled"));
            setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, themeColours.at("disabled"));
            setColour(juce::Slider::ColourIds::rotarySliderFillColourId, themeColours.at("white"));
            setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("white"));
        }
    }

    void updateDisabled() {
        updateColourAll();
        repaint();
    }

    void setAndUpdateDisabled(bool flag) {
        disabled = flag;
        updateDisabled();
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
    bool disabled; // show disabled color

    MiniTextSlider(juce::AudioProcessorValueTreeState& valueTreeState, const juce::String parameterID, 
        juce::LookAndFeel* lookAndFeel = nullptr, bool disabled = false) :
        valueTreeState(valueTreeState), parameterID(parameterID), disabled(disabled)
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
        g.setColour(disabled ? themeColours.at("grey") : themeColours.at("white"));
        g.fillRect(0, 0, getWidth(), getHeight());
        if (!disabled) {
            g.setColour(themeColours.at("blue"));
            g.fillRect(0, 0, static_cast<int>(getWidth() * percent), getHeight());
        }
        g.setColour(themeColours.at("text"));
        g.drawRect(0, 0, getWidth(), getHeight());
    }

    void updateDisabled() {
        repaint();
    }

    void setAndUpdateDisabled(bool flag) {
        disabled = flag;
        updateDisabled();
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniTextSlider)
};

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

    // param copy
    std::atomic<float>* gain              = valueTreeState.getRawParameterValue("gain");
    std::atomic<float>* isInvertPhase     = valueTreeState.getRawParameterValue("invertPhase");
    std::atomic<float>* isMono            = valueTreeState.getRawParameterValue("mono");
    std::atomic<float>* pan               = valueTreeState.getRawParameterValue("pan");
    std::atomic<float>* stereoMode        = valueTreeState.getRawParameterValue("stereoMode");
    std::atomic<float>* stereoWidth       = valueTreeState.getRawParameterValue("stereoWidth");
    std::atomic<float>* stereoMidSide     = valueTreeState.getRawParameterValue("stereoMidSide");
    std::atomic<float>* isBassMono        = valueTreeState.getRawParameterValue("isBassMono");
    std::atomic<float>* bassMonoFrequency = valueTreeState.getRawParameterValue("bassMonoFrequency");

    // parameter components   
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    KnobSlider gainSlider{ &customLookAndFeel };
    ToggleTextButton invertPhaseToggleButton{ "Invert Phase", &customLookAndFeel };
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel };
    KnobSlider panSlider{&customLookAndFeel};
    juce::TextButton stereoModeSwitchButton;
    KnobSlider stereoWidthSlider{ &customLookAndFeel, *isMono != 0 };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel };
    MiniTextSlider bassMonoFrequencySlider{ valueTreeState, "bassMonoFrequency", &customLookAndFeel, *isBassMono == 0 };

    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<ButtonAttachment> stereoModeSwitchButtonAttachment;
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

    void updateStereoLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityCloneAudioProcessorEditor)
};
