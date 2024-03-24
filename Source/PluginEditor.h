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
};

class ToggleTextButton : public juce::TextButton
{
public:
    ToggleTextButton(juce::String text) {
        setClickingTogglesState(true);
        setButtonText(text);
        setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
        setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
        setColour(juce::TextButton::textColourOnId, juce::Colours::black);
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
        setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(46, 52, 64));
        setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(46, 52, 64));
        setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(136, 192, 208));
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
    UtilitycloneAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;
    
    juce::AudioProcessorValueTreeState& valueTreeState;

    int width = 200;
    int height = 300;
    double ratio = width / height;

    // parameter components   
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    GainKnob gainSlider{ &customLookAndFeel };
    ToggleTextButton invertPhaseToggleButton{"Invert Phase"};
    ToggleTextButton monoToggleButton{"Mono"};
    KnobSlider panSlider{&customLookAndFeel};
    juce::ComboBox stereoModeComboBox;
    KnobSlider stereoWidthSlider{ &customLookAndFeel };
    KnobSlider stereoMidSideSlider{ &customLookAndFeel };
    ToggleTextButton bassMonoToggleButton{"Bass Mono"};
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
    juce::Rectangle<int> columnL{ 0, 0, width / 2, height };
    juce::Rectangle<int> columnR{ width / 2, 0, width / 2, height };
    juce::Label gainLabel;
    juce::Label panLabel;

    //std::unique_ptr<juce::Slider> tb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilitycloneAudioProcessorEditor)
};
