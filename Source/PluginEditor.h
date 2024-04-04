/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

const std::unordered_map<std::string, juce::Colour> themeColours = {
    { "blue",       juce::Colour::fromRGB(85, 222, 246) },
    { "deepblue",   juce::Colour::fromRGB(56, 115, 255) },
    { "orange",     juce::Colour::fromRGB(255, 177, 0) },
    { "white",      juce::Colour::fromRGB(220, 220,220) },
    { "lightgrey",  juce::Colour::fromRGB(183, 183, 183) },
    { "grey",       juce::Colour::fromRGB(143, 143, 143) },
    { "disabled",   juce::Colour::fromRGB(105, 105,105) },
    { "lightblack", juce::Colour::fromRGB(42, 42, 42) },
    { "text",       juce::Colours::black },
};

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    // knob
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;
    // slider text box
    void drawLabel(juce::Graphics&, juce::Label&) override;
    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    // square button
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

class TogglePhaseButton : public ToggleTextButton
{
public:
    using ToggleTextButton::ToggleTextButton;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto& lf = getLookAndFeel();

        lf.drawButtonBackground(g, *this,
            findColour(getToggleState() ? buttonOnColourId : buttonColourId),
            shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        // draw image
        auto width = getWidth();

        juce::Font font(lf.getTextButtonFont(*this, getHeight()));
        g.setFont(font);
        g.setColour(findColour(getToggleState() ? TextButton::textColourOnId
            : TextButton::textColourOffId)
            .withMultipliedAlpha(isEnabled() ? 1.0f : 0.5f));

        const int yIndent = std::min(4, proportionOfHeight(0.3f));
        const int cornerSize = std::min(getHeight(), getWidth()) / 2;

        const int fontHeight = round(font.getHeight() * 0.6f);
        const int leftIndent = std::min(fontHeight, 2 + cornerSize / (isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = std::min(fontHeight, 2 + cornerSize / (isConnectedOnRight() ? 4 : 2));
        const int textWidth = getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText(getButtonText(),
                leftIndent, yIndent, textWidth, getHeight() - yIndent * 2,
                juce::Justification::centredRight, 2);

        const float lineThickness = 1.2f;
        g.drawEllipse(leftIndent, rightIndent, fontHeight, fontHeight, lineThickness);
        g.drawLine(leftIndent + fontHeight, rightIndent, leftIndent, rightIndent + fontHeight, lineThickness);
    };
};

class ListeningButton : public juce::TextButton
{
public:
    using TextButton::TextButton;

    ListeningButton() {
        setClickingTogglesState(true);
        setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
        setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("deepblue"));
        setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("text"));
    }

    void paintButton(juce::Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override {
        const auto isOn = getToggleState();

        g.setColour(isOn ? findColour(buttonOnColourId) : findColour(buttonColourId));
        g.fillEllipse(0, 0, getWidth(), getHeight());

        g.setColour(findColour(juce::ComboBox::ColourIds::outlineColourId));
        g.drawEllipse(0, 0, getWidth(), getHeight(), 1);

        const auto padding = 4;
        const auto pad2 = padding * 2;
        g.drawImage(
            headphoneImage,
            padding, padding, getWidth() - pad2, getHeight() - pad2,
            0, 0, headphoneImage.getWidth(), headphoneImage.getHeight()
        );

        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colour::fromFloatRGBA(0, 0, 0, 0.05));
            g.fillEllipse(0, 0, getWidth(), getHeight());
        }
    }

private:
    juce::Image headphoneImage{
        juce::ImageCache::getFromMemory(
            BinaryData::headphone_16_16_png,
            BinaryData::headphone_16_16_pngSize
        )
    };
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
