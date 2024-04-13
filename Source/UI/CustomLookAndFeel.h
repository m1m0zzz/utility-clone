#pragma once

#define _USE_MATH_DEFINES
#include "math.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    // knob
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override {

        auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
        auto background = slider.findColour(juce::Slider::backgroundColourId);
        auto thumb = slider.findColour(juce::Slider::thumbColourId);

        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(3.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(outline);
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, 2 * M_PI, toAngle, true);
        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path p;
        radius = radius - lineW * 2;
        auto pointerLength = radius;
        auto pointerThickness = 2.4f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
        g.setColour(thumb);
        g.fillPath(p);
    };

    // slider text box
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        const auto text = label.findColour(juce::Label::ColourIds::textColourId);
        const auto rect = juce::Rectangle(0, 0, label.getWidth(), label.getHeight());
        const auto editor = label.getCurrentTextEditor();
        if (editor != nullptr) {
            editor->applyColourToAllText(label.findColour(juce::TextEditor::ColourIds::textColourId), true);
            editor->setJustification(juce::Justification::Flags::centred);
        }

        g.setColour(text);
        g.drawText(label.getText(), rect, juce::Justification::Flags::centred);
    };

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        juce::Label* label = LookAndFeel_V4::createSliderTextBox(slider);
        label->setColour(juce::Label::ColourIds::textColourId, themeColours.at("text"));
        label->setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::transparentWhite);
        label->setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::transparentWhite);
        label->setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("text"));
        label->setColour(juce::TextEditor::ColourIds::textColourId, themeColours.at("text"));
        label->setColour(juce::TextEditor::ColourIds::backgroundColourId, themeColours.at("white"));
        label->setColour(juce::TextEditor::ColourIds::outlineColourId, themeColours.at("lightblack"));
        label->setColour(juce::TextEditor::ColourIds::highlightedTextColourId, themeColours.at("text"));
        label->setColour(juce::TextEditor::ColourIds::highlightColourId, juce::Colours::transparentWhite);
        return label;
    };

    // square button
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto isOn = button.getToggleState();
        auto bounds = juce::Rectangle(0, 0, button.getWidth(), button.getHeight());
        auto bg = button.findColour(juce::TextButton::ColourIds::buttonColourId);
        auto bgOn = button.findColour(juce::TextButton::ColourIds::buttonOnColourId);
        auto outline = button.findColour(juce::ComboBox::ColourIds::outlineColourId);

        g.setColour(isOn ? bgOn : bg);
        g.fillRect(bounds);
        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colour::fromFloatRGBA(0, 0, 0, 0.05));
            g.fillRect(bounds);
        }
        g.setColour(outline);
        g.drawRect(bounds);
    };
};
