/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class ToggleTextButton : public juce::TextButton
{
public:
    ToggleTextButton(juce::String text, juce::LookAndFeel* lookAndFeel) {
        setClickingTogglesState(true);
        setButtonText(text);
        setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour::fromRGB(255, 177, 0));
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(183, 183, 183));
        setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
        setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
        //setLookAndFeel(lookAndFeel); // ˆê‰ñ‚µ‚©“K—p‚³‚ê‚È‚¢
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
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(42, 42, 42));
        setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(42, 42, 42));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(85, 222, 246));
        setLookAndFeel(lookAndFeel);
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobSlider)
};

class GainKnob : public KnobSlider
{
public:
    using KnobSlider::KnobSlider;

    //juce::String getTextFromValue(double value) override
    //{
    //    return juce::Decibels::toString(value);
    //}

    //double getValueFromText(const juce::String& text) override
    //{
    //    auto minusInfinitydB = -100.0;

    //    // TODO: changed "-INF"
    //    // TODO: +XX dB (delete: "+")
    //    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
    //    return decibelText.equalsIgnoreCase("-inf") ? minusInfinitydB
    //                                                : decibelText.getDoubleValue();
    //}
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainKnob)
};

class SliderTab : public juce::TabbedComponent
{
public:
    using juce::TabbedComponent::TabbedComponent;

    void currentTabChanged(int index, const juce::String& name) override {
        //DBG("currentTabChanged(" << index << ", " << name << ")");
        if (onTabChanged) {
            onTabChanged(index, name);
        }
    };

    std::function<void(int index, const juce::String& name)> onTabChanged = nullptr;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderTab)
};

//==============================================================================
/**
*/
class UtilitycloneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UtilitycloneAudioProcessorEditor (UtilitycloneAudioProcessor&, juce::AudioProcessorValueTreeState& vts, std::atomic<float>* stereo);
    ~UtilitycloneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    UtilitycloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    std::atomic<float>* stereoMode; // Width or Mid/Side

    int width = 200;
    int height = 300;
    double ratio = width / height;

    // parameter components   
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GainKnob gainSlider{ &customLookAndFeel };
    ToggleTextButton invertPhaseToggleButton{ "Invert Phase", &customLookAndFeel };
    ToggleTextButton monoToggleButton{ "Mono", &customLookAndFeel };
    KnobSlider panSlider{&customLookAndFeel};
    juce::ComboBox stereoModeComboBox;
    KnobSlider stereoWidthSlider{ &customLookAndFeel };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel };
    ToggleTextButton bassMonoToggleButton{ "Bass Mono", &customLookAndFeel };
    KnobSlider bassMonoFrequencySlider{ &customLookAndFeel };

    std::unique_ptr<SliderAttachment> gainSliderAttachment;
    std::unique_ptr<ButtonAttachment> invertPhaseToggleButtonAttachment;
    std::unique_ptr<ButtonAttachment> monoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> panSliderAttachment;
    std::unique_ptr<ComboBoxAttachment> stereoModeComboBoxAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthSliderAttachment;
    std::unique_ptr<SliderAttachment> stereoMidSideSliderAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoToggleButtonAttachment;
    std::unique_ptr<SliderAttachment> bassMonoFrequencySliderAttachment;

    // ui components
    SliderTab stereoTab{ juce::TabbedButtonBar::TabsAtTop };
    juce::Rectangle<int> columnL{ 0, 0, width / 2, height };
    juce::Rectangle<int> columnR{ width / 2, 0, width / 2, height };
    juce::Label gainLabel;
    juce::Label panLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
