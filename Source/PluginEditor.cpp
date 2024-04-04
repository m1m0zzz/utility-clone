/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define _USE_MATH_DEFINES
#include "math.h"

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider) {

    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
    auto background = slider.findColour(juce::Slider::backgroundColourId);
    auto thumb = slider.findColour(juce::Slider::thumbColourId);

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
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
}

// slider text box
void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    const auto text = label.findColour(juce::Label::ColourIds::textColourId);
    const auto rect = juce::Rectangle(0, 0, label.getWidth(), label.getHeight());
    const auto editor = label.getCurrentTextEditor();
    if (editor != nullptr) {
        editor->applyColourToAllText(label.findColour(juce::TextEditor::ColourIds::textColourId), true);
    }

    g.setColour(text);
    g.drawText(label.getText(), rect, juce::Justification::Flags::centred);
}

juce::Label* CustomLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    juce::Label* label = LookAndFeel_V4::createSliderTextBox(slider);
    label->setColour(juce::Label::ColourIds::textColourId,       themeColours.at("text"));
    label->setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::transparentWhite);
    label->setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::transparentWhite);
    label->setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("text"));
    label->setColour(juce::TextEditor::ColourIds::textColourId,  themeColours.at("text"));
    label->setColour(juce::TextEditor::ColourIds::backgroundColourId, themeColours.at("white"));
    label->setColour(juce::TextEditor::ColourIds::outlineColourId, themeColours.at("lightblack"));
    label->setColour(juce::TextEditor::ColourIds::highlightedTextColourId, themeColours.at("text"));
    label->setColour(juce::TextEditor::ColourIds::highlightColourId, juce::Colours::transparentWhite);
    return label;
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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
}

//==============================================================================
UtilityCloneAudioProcessorEditor::UtilityCloneAudioProcessorEditor(
    UtilityCloneAudioProcessor& p, juce::AudioProcessorValueTreeState& vts, juce::UndoManager& um)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts), undoManager(um),
    undoButton("Undo"), redoButton("Redo")
{
    // window
    //setResizable(true, true);
    //setResizeLimits(width, height, 400, 600);
    //getConstrainer()->setFixedAspectRatio(ratio);

    // components
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    auto gainRange = valueTreeState.getParameterRange("gain");
    gainSlider.setRange(gainRange.start, gainRange.end);
    gainSlider.setSkewFactorFromMidPoint(0);
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);

    invertPhaseLToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhaseL", invertPhaseLToggleButton));
    addAndMakeVisible(invertPhaseLToggleButton);

    invertPhaseRToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhaseR", invertPhaseRToggleButton));
    addAndMakeVisible(invertPhaseRToggleButton);

    monoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "mono", monoToggleButton));
    monoToggleButton.onClick = [this]() {
        auto state = monoToggleButton.getToggleState();
        stereoWidthSlider.setAndUpdateDisabled(state);
        stereoMidSideSlider.setAndUpdateDisabled(state);
        bassMonoToggleButton.setAndUpdateDisabled(state);
        bassMonoFrequencySlider.setAndUpdateDisabled(state || !*isBassMono);
    };
    addAndMakeVisible(monoToggleButton);

    panSliderAttachment.reset(new SliderAttachment(valueTreeState, "pan", panSlider));
    addAndMakeVisible(panSlider);

    stereoWidthSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoWidth", stereoWidthSlider));
    auto widthRange = valueTreeState.getParameterRange("stereoWidth");
    stereoWidthSlider.setRange(widthRange.start, widthRange.end);
    stereoWidthSlider.setSkewFactorFromMidPoint(100);
    stereoWidthSlider.setTextValueSuffix("%");
    addAndMakeVisible(stereoWidthSlider);

    stereoMidSideSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoMidSide", stereoMidSideSlider));
    addAndMakeVisible(stereoMidSideSlider);
    stereoMidSideSlider.setVisible(false);

    stereoModeLabel.setText("Width", juce::dontSendNotification);
    stereoModeLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    stereoModeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoModeLabel);

    stereoModeSwitchButtonAttachment.reset(new ButtonAttachment(valueTreeState, "stereoMode", stereoModeSwitchButton));
    stereoModeSwitchButton.setButtonText("S");
    stereoModeSwitchButton.setClickingTogglesState(true);
    stereoModeSwitchButton.setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
    stereoModeSwitchButton.setColour(juce::TextButton::ColourIds::textColourOffId, themeColours.at("text"));
    stereoModeSwitchButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("lightgrey"));
    stereoModeSwitchButton.setColour(juce::TextButton::ColourIds::textColourOnId, themeColours.at("text"));
    stereoModeSwitchButton.setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("text"));
    stereoModeSwitchButton.onClick = [this]() {
        updateStereoLabel();
    };
    addAndMakeVisible(stereoModeSwitchButton);
    updateStereoLabel();

    bassMonoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "isBassMono", bassMonoToggleButton));
    bassMonoToggleButton.onClick = [this]() {
        auto state = bassMonoToggleButton.getToggleState();
        bassMonoFrequencySlider.setAndUpdateDisabled(*isMono || !state);
    };
    addAndMakeVisible(bassMonoToggleButton);

    bassMonoFrequencySliderAttachment.reset(new SliderAttachment(valueTreeState, "bassMonoFrequency", bassMonoFrequencySlider));
    bassMonoFrequencySlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(bassMonoFrequencySlider);

    bassMonoListeningButtonAttachment.reset(new ButtonAttachment(valueTreeState, "isBassMonoListening", bassMonoListeningButton));
    addAndMakeVisible(bassMonoListeningButton);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    panLabel.setText("Balance", juce::dontSendNotification);
    panLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);

    auto fontBold = inputLabel.getFont();
    fontBold.setStyleFlags(juce::Font::FontStyleFlags::bold);
    inputLabel.setText("Input", juce::dontSendNotification);
    inputLabel.setFont(fontBold);
    inputLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    inputLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inputLabel);

    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.setFont(fontBold);
    outputLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    outputLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputLabel);

    setSize(width, height);
}

UtilityCloneAudioProcessorEditor::~UtilityCloneAudioProcessorEditor()
{
}

//==============================================================================
bool UtilityCloneAudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    //DBG("keyPressed");
    if (key.getModifiers().isCommandDown() || key.getModifiers().isCtrlDown()) {
        if (key.getKeyCode() == 'z' || key.getKeyCode() == 'Z') {
            undoManager.undo();
            updateStereoLabel();
            bassMonoFrequencySlider.updateDisabled();
        }
        else if (key.getKeyCode() == 'y' || key.getKeyCode() == 'Y') {
            undoManager.redo();
            updateStereoLabel();
            bassMonoFrequencySlider.updateDisabled();
        }
    }
    return true;
}

void UtilityCloneAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(themeColours.at("grey"));

    // hr
    g.setColour(juce::Colour::fromRGB(80, 80, 80));
    g.fillRect(width / 2, 10, 1, height - 20);
}

void UtilityCloneAudioProcessorEditor::resized()
{
    width = getWidth();
    height = getHeight();

    const int padding = 5;
    const int componentHeight = 26;
    const int knobHeight = 80;
    const auto buttonSize = 20; // stereoModeSwitchButton

    columnL.setWidth(width / 2 - 5);
    columnL.setHeight(height);

    columnR.setLeft(width / 2 + 5);
    columnR.setWidth(width / 2 - 5);
    columnR.setHeight(height);

    // column L
    auto rect = columnL.reduced(padding);
    rect.setTop(padding);
    rect.setHeight(componentHeight);
    inputLabel.setBounds(rect);

    rect.setTop(35);
    rect.setWidth(rect.getWidth() / 2 - 2);
    rect.setHeight(componentHeight);
    invertPhaseLToggleButton.setBounds(rect);

    rect.setX((columnL.getWidth() + 2) / 2);
    invertPhaseRToggleButton.setBounds(rect);

    rect = columnL.reduced(padding);
    rect.setTop(80);
    rect.setWidth(columnL.getWidth() - padding - buttonSize);
    rect.setHeight(componentHeight);
    stereoModeLabel.setBounds(rect);

    rect = columnL.reduced(padding);
    rect.setTop(80 + (componentHeight - buttonSize) / 2);
    rect.setX(rect.getX() + rect.getWidth() - buttonSize);
    rect.setWidth(buttonSize);
    rect.setHeight(buttonSize);
    stereoModeSwitchButton.setBounds(rect);

    rect = columnL.reduced(padding);
    rect.setTop(110);
    rect.setHeight(knobHeight);
    stereoWidthSlider.setBounds(rect);
    stereoMidSideSlider.setBounds(rect);

    rect.setTop(200);
    rect.setHeight(componentHeight);
    monoToggleButton.setBounds(rect);

    rect.setTop(235);
    rect.setHeight(componentHeight);
    bassMonoToggleButton.setBounds(rect);

    rect.setTop(270);
    rect.setWidth(rect.getWidth() - 24);
    rect.setHeight(20);
    bassMonoFrequencySlider.setBounds(rect);

    rect = columnL.reduced(padding);
    rect.setTop(270);
    rect.setX(rect.getX() + rect.getWidth() - 20);
    rect.setWidth(20);
    rect.setHeight(20);
    bassMonoListeningButton.setBounds(rect);

    // column R
    rect = columnR.reduced(padding);
    rect.setTop(padding);
    rect.setHeight(componentHeight);
    outputLabel.setBounds(rect);

    rect.setTop(30);
    rect.setHeight(componentHeight);
    gainLabel.setBounds(rect);

    rect.setTop(50);
    rect.setHeight(knobHeight);
    gainSlider.setBounds(rect);

    rect.setTop(height / 2 + 10);
    rect.setHeight(componentHeight);
    panLabel.setBounds(rect);

    rect.setTop(height / 2 + 30);
    rect.setHeight(knobHeight);
    panSlider.setBounds(rect);

    //undoButton.setBounds(10, 50, 50, 30);
    //redoButton.setBounds(60, 50, 50, 30);
}

void UtilityCloneAudioProcessorEditor::updateStereoLabel()
{
    auto boolean = !(static_cast<bool>(*stereoMode));
    stereoWidthSlider.setVisible(boolean);
    stereoMidSideSlider.setVisible(!boolean);
    stereoModeLabel.setText(boolean ? "Width" : "Mid/Side", juce::sendNotification);
}
