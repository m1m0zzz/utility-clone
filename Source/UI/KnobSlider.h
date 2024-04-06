#pragma once

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
