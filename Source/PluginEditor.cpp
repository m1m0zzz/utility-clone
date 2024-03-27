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
    //DBG("drawRotarySlider");

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

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown) {
    // custom look
    //DBG("drawButtonBackground");

    //auto cButtonOn  = button.findColour(juce::TextButton::ColourIds::buttonOnColourId);
    //auto cButtonOff = button.findColour(juce::TextButton::ColourIds::buttonColourId);
    //auto cTextOn    = button.findColour(juce::TextButton::ColourIds::textColourOnId);
    //auto cTextOff   = button.findColour(juce::TextButton::ColourIds::textColourOffId);

    auto bounds = button.getBounds();

    g.setColour(backgroundColour);
    g.fillRect(bounds);
}

//==============================================================================
UtilityCloneAudioProcessorEditor::UtilityCloneAudioProcessorEditor(
    UtilityCloneAudioProcessor& p, juce::AudioProcessorValueTreeState& vts, juce::UndoManager& um)
    : AudioProcessorEditor(&p), audioProcessor(p), valueTreeState(vts), undoManager(um),
    undoButton("Undo"), redoButton("Redo"), bassMonoFrequencySlider(vts.getRawParameterValue("bassMonoFrequency"), 120.0f, 500.0f)
{
    // window
    setResizable(true, true);
    setResizeLimits(width, height, 400, 600);
    //getConstrainer()->setFixedAspectRatio(ratio);

    // components
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    auto gainRange = valueTreeState.getParameterRange("gain");
    gainSlider.setRange(gainRange.start, gainRange.end);
    gainSlider.setSkewFactorFromMidPoint(0);
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);

    invertPhaseToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhase", invertPhaseToggleButton));
    addAndMakeVisible(invertPhaseToggleButton);

    monoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "mono", monoToggleButton));
    addAndMakeVisible(monoToggleButton);

    panSliderAttachment.reset(new SliderAttachment(valueTreeState, "pan", panSlider));
    addAndMakeVisible(panSlider);

    stereoWidthSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoWidth", stereoWidthSlider));
    auto widthRange = valueTreeState.getParameterRange("stereoWidth");
    stereoWidthSlider.setRange(widthRange.start, widthRange.end);
    stereoWidthSlider.setSkewFactorFromMidPoint(100);
    stereoWidthSlider.setTextValueSuffix("%");

    stereoMidSideSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoMidSide", stereoMidSideSlider));

    stereoTab.addTab("Width", themeColours.at("grey"), &stereoWidthSlider, true);
    stereoTab.addTab("M/S", themeColours.at("grey"), &stereoMidSideSlider, true);
    stereoTab.onTabChanged = [this](int index, juce::String name) {
        valueTreeState.getRawParameterValue("stereoMode")->store(static_cast<float>(index));
        };
    addAndMakeVisible(stereoTab);

    bassMonoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "isBassMono", bassMonoToggleButton));
    addAndMakeVisible(bassMonoToggleButton);

    bassMonoFrequencySliderAttachment.reset(new SliderAttachment(valueTreeState, "bassMonoFrequency", bassMonoFrequencySlider));
    bassMonoFrequencySlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(bassMonoFrequencySlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    panLabel.setText("Balance", juce::dontSendNotification);
    panLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);

    //addAndMakeVisible(undoButton);
    //addAndMakeVisible(redoButton);
    //undoButton.onClick = [this] {
    //    undoManager.undo();
    //    DBG("undo");
    //};
    //redoButton.onClick = [this] {
    //    undoManager.redo();
    //    DBG("redo");
    //};

    setSize(width, height);
}

UtilityCloneAudioProcessorEditor::~UtilityCloneAudioProcessorEditor()
{
}

//==============================================================================
void UtilityCloneAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(themeColours.at("grey"));

    // hr
    g.setColour(juce::Colour::fromRGB(80, 80, 80));
    g.fillRect(width / 2, 10, 1, height - 20);
}

void UtilityCloneAudioProcessorEditor::resized()
{
    width = getWidth();
    height = getHeight();
    //DBG("w = " << width << ", h = " << height);

    const int padding = 5;
    const int compoentHeight = 26;
    const int knobHeight = 80;

    columnL.setWidth(width / 2 - 5);
    columnL.setHeight(height);

    columnR.setLeft(width / 2 + 5);
    columnR.setWidth(width / 2 - 5);
    columnR.setHeight(height);

    // column L
    auto rect = columnL.reduced(padding);
    rect.setHeight(compoentHeight);
    invertPhaseToggleButton.setBounds(rect);

    rect.setTop(40);
    rect.setHeight(knobHeight + stereoTab.getTabBarDepth());
    stereoTab.setBounds(rect);

    rect.setTop(160);
    rect.setHeight(compoentHeight);
    monoToggleButton.setBounds(rect);

    rect.setTop(190);
    rect.setHeight(compoentHeight);
    bassMonoToggleButton.setBounds(rect);

    rect.setTop(220);
    rect.setHeight(20);
    bassMonoFrequencySlider.setBounds(rect);

    // column R
    rect = columnR.reduced(padding);
    rect.setTop(10);
    rect.setHeight(compoentHeight);
    gainLabel.setBounds(rect);

    rect.setTop(30);
    rect.setHeight(knobHeight);
    gainSlider.setBounds(rect);

    rect.setTop(height / 2 + 10);
    rect.setHeight(compoentHeight);
    panLabel.setBounds(rect);

    rect.setTop(height / 2 + 30);
    rect.setHeight(knobHeight);
    panSlider.setBounds(rect);

    //undoButton.setBounds(10, 50, 50, 30);
    //redoButton.setBounds(60, 50, 50, 30);
}
