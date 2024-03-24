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
    auto lineW = juce::jmin(4.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, 2 * M_PI, toAngle, true);
    g.setColour(fill);
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    radius = radius - lineW * 2;
    g.setColour(background);
    g.fillEllipse(bounds.getCentreX() - radius, bounds.getCentreY() - radius, radius * 2, radius * 2);

    juce::Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 2.4f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
    g.setColour(thumb);
    g.fillPath(p);
}

//ToggleTextButton::ToggleTextButton() {
//    setClickingTogglesState(true);
//    setButtonText("Invert Phase");
//    setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
//    setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
//    setColour(juce::TextButton::textColourOnId, juce::Colours::black);
//}

//==============================================================================
UtilitycloneAudioProcessorEditor::UtilitycloneAudioProcessorEditor (
    UtilitycloneAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    // window
    setResizable(true, true);
    setResizeLimits(width, height, 400, 600);
    //getConstrainer()->setFixedAspectRatio(ratio);

    // components
    const juce::Colour textColor = juce::Colour::fromRGB(0, 0, 0);
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, width / 2 - 20, 20);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
    gainSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(46, 52, 64));
    gainSlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(46, 52, 64));
    gainSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(136, 192, 208));
    gainSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(gainSlider);

    invertPhaseToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhase", invertPhaseToggleButton));
    invertPhaseToggleButton.setClickingTogglesState(true);
    invertPhaseToggleButton.setButtonText("Invert Phase");
    invertPhaseToggleButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
    invertPhaseToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
    invertPhaseToggleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    addAndMakeVisible(invertPhaseToggleButton);

    monoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "mono", monoToggleButton));
    monoToggleButton.setClickingTogglesState(true);
    monoToggleButton.setButtonText("Mono");
    monoToggleButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
    monoToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
    monoToggleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    addAndMakeVisible(monoToggleButton);

    panSliderAttachment.reset(new SliderAttachment(valueTreeState, "pan", panSlider));
    // TODO: prefix (pan < 0 ? L : R) abs(pan)
    panSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, width / 2 - 20, 20);
    panSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
    panSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(46, 52, 64));
    panSlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(46, 52, 64));
    panSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(136, 192, 208));
    panSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(panSlider);

    stereoModeComboBox.addItemList(stereoModeList, 1);
    stereoModeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "stereoMode", stereoModeComboBox));
    addAndMakeVisible(stereoModeComboBox);

    stereoWidthSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoWidth", stereoWidthSlider));
    stereoWidthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    stereoWidthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, width / 2 - 20, 20);
    stereoWidthSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
    stereoWidthSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(46, 52, 64));
    stereoWidthSlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(46, 52, 64));
    stereoWidthSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(136, 192, 208));
    stereoWidthSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(stereoWidthSlider);

    stereoMidSideSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoMidSide", stereoMidSideSlider));
    stereoMidSideSlider.setColour(juce::Slider::textBoxTextColourId, textColor);
    addAndMakeVisible(stereoMidSideSlider);

    bassMonoToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "isBassMono", bassMonoToggleButton));
    bassMonoToggleButton.setClickingTogglesState(true);
    bassMonoToggleButton.setButtonText("Bass Mono");
    bassMonoToggleButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
    bassMonoToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
    bassMonoToggleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    addAndMakeVisible(bassMonoToggleButton);

    bassMonoFrequencySliderAttachment.reset(new SliderAttachment(valueTreeState, "bassMonoFrequency", bassMonoFrequencySlider));
    bassMonoFrequencySlider.setTextValueSuffix(" Hz");
    bassMonoFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    bassMonoFrequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, width / 2 - 20, 20);
    bassMonoFrequencySlider.setColour(juce::Slider::textBoxTextColourId, textColor);
    bassMonoFrequencySlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::Colour(46, 52, 64));
    bassMonoFrequencySlider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour::Colour(46, 52, 64));
    bassMonoFrequencySlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour::Colour(136, 192, 208));
    bassMonoFrequencySlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(bassMonoFrequencySlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, textColor);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    panLabel.setText("Balance", juce::dontSendNotification);
    panLabel.setColour(juce::Label::textColourId, textColor);
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);

    setSize (width, height);
}

UtilitycloneAudioProcessorEditor::~UtilitycloneAudioProcessorEditor()
{
}

//==============================================================================
void UtilitycloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colour::fromRGB(140, 140, 140));

    // hr
    g.setColour(juce::Colour::fromRGB(112, 112, 112));
    g.fillRect(width / 2 - 1, 10, 2, height - 20);

    //g.setColour(juce::Colours::azure);
    //g.fillRect(columnL.toFloat());
    //g.setColour(juce::Colours::orange);
    //g.fillRect(columnR.toFloat());
}

//void pushComponent(juce::Rectangle<int> column, juce::Component component, int y = 0, int height = 39) {
//    auto a = column.reduced(5);
//    a.setTop(y);
//    a.setHeight(height);
//    component.setBounds(a);
//}

void UtilitycloneAudioProcessorEditor::resized()
{
    width  = getWidth();
    height = getHeight();
    //DBG("w = " << width << ", h = " << height);

    const int padding = 5;
    const int compoentHeight = 26;

    columnL.setWidth(width / 2 - 5);
    columnL.setHeight(height);

    columnR.setLeft(width / 2 + 5);
    columnR.setWidth(width / 2 - 5);
    columnR.setHeight(height);

    // column L
    auto rect = columnL.reduced(padding);
    rect.setHeight(compoentHeight);
    invertPhaseToggleButton.setBounds(rect);
    
    rect.setTop(35);
    rect.setHeight(compoentHeight);
    stereoModeComboBox.setBounds(rect);

    rect.setTop(70);
    rect.setHeight(80);
    stereoWidthSlider.setBounds(rect);

    //stereoWidthSlider.setBounds(10, 210, 180, 30);
    //stereoMidSideSlider.setBounds(10, 250, 180, 30);

    rect.setTop(160);
    rect.setHeight(compoentHeight);
    monoToggleButton.setBounds(rect);

    rect.setTop(190);
    rect.setHeight(compoentHeight);
    bassMonoToggleButton.setBounds(rect);

    rect.setTop(220);
    rect.setHeight(80);
    bassMonoFrequencySlider.setBounds(rect);

    // column R

    rect = columnR.reduced(padding);
    rect.setTop(10);
    rect.setHeight(compoentHeight);
    gainLabel.setBounds(rect);

    rect.setTop(30);
    rect.setHeight(80);
    gainSlider.setBounds(rect);

    rect.setTop(height / 2 + 10);
    rect.setHeight(compoentHeight);
    panLabel.setBounds(rect);

    rect.setTop(height / 2 + 30);
    rect.setHeight(80);
    panSlider.setBounds(rect);
}
