#pragma once

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
            setColour(ColourIds::buttonOnColourId, themeColours.at("orange"));
            setColour(ColourIds::buttonColourId, themeColours.at("lightgrey"));
            setColour(ColourIds::textColourOnId, themeColours.at("text"));
            setColour(ColourIds::textColourOffId, themeColours.at("text"));
            setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("lightblack"));
        }
        else {
            setColour(ColourIds::buttonOnColourId, themeColours.at("white"));
            setColour(ColourIds::buttonColourId, themeColours.at("lightgrey"));
            setColour(ColourIds::textColourOnId, themeColours.at("disabled"));
            setColour(ColourIds::textColourOffId, themeColours.at("disabled"));
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
