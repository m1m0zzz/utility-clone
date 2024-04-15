#pragma once

#define _USE_MATH_DEFINES
#include "math.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        setColour(juce::PopupMenu::ColourIds::backgroundColourId, themeColours.at("white"));
        setColour(juce::PopupMenu::ColourIds::textColourId, themeColours.at("text"));
        setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, themeColours.at("lightblue"));
        setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, themeColours.at("text"));
    }

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

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override {
        // ref: https://github.com/juce-framework/JUCE/blob/master/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.cpp#L1651
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == juce::Slider::TextBoxLeft || textBoxPos == juce::Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth = std::max(0, std::min(slider.getTextBoxWidth(), localBounds.getWidth() - minXSpace));
        textBoxWidth = static_cast<int>(textBoxWidth * 0.85); // custamized this line
        auto textBoxHeight = std::max(0, std::min(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        juce::Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != juce::Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth(textBoxWidth);
                layout.textBoxBounds.setHeight(textBoxHeight);

                if (textBoxPos == juce::Slider::TextBoxLeft)           layout.textBoxBounds.setX(0);
                else if (textBoxPos == juce::Slider::TextBoxRight)     layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == juce::Slider::TextBoxAbove)          layout.textBoxBounds.setY(0);
                else if (textBoxPos == juce::Slider::TextBoxBelow)     layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight - 8); // custamized this line
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce(1, 1);   // bar border
        }
        else
        {
            if (textBoxPos == juce::Slider::TextBoxLeft)       layout.sliderBounds.removeFromLeft(textBoxWidth);
            else if (textBoxPos == juce::Slider::TextBoxRight) layout.sliderBounds.removeFromRight(textBoxWidth);
            else if (textBoxPos == juce::Slider::TextBoxAbove) layout.sliderBounds.removeFromTop(textBoxHeight);
            else if (textBoxPos == juce::Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom(textBoxHeight);

            const int thumbIndent = getSliderThumbRadius(slider);

            if (slider.isHorizontal())    layout.sliderBounds.reduce(thumbIndent, 0);
            else if (slider.isVertical()) layout.sliderBounds.reduce(0, thumbIndent);
        }

        return layout;
    }

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

    void drawComboBox(juce::Graphics& g, int width, int height, bool,
        int, int, int, int, juce::ComboBox& box) override
    {
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRect(boxBounds);

        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRect(boxBounds, 1.0f);

        juce::Rectangle<int> arrowZone(width - 20, 0, std::min(15, height), height);
        juce::Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }
};
