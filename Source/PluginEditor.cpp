/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setTextValueSuffix(" dB");
    /*gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, width * 2 / 3, 50);*/
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
    addAndMakeVisible(panSlider);

    stereoModeComboBox.addItemList(stereoModeList, 1);
    stereoModeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "stereoMode", stereoModeComboBox));
    addAndMakeVisible(stereoModeComboBox);

    stereoWidthSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoWidth", stereoWidthSlider));
    addAndMakeVisible(stereoWidthSlider);

    stereoMidSideSliderAttachment.reset(new SliderAttachment(valueTreeState, "stereoMidSide", stereoMidSideSlider));
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
    addAndMakeVisible(bassMonoFrequencySlider);

    setSize (width, height);
}

UtilitycloneAudioProcessorEditor::~UtilitycloneAudioProcessorEditor()
{
}

//==============================================================================
void UtilitycloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

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
    const int windowWidth  = getWidth();
    const int windowHeight = getHeight();
    //DBG("w = " << windowWidth << ", h = " << windowHeight);

    const int padding = 5;
    const int compoentHeight = 30;

    columnL.setWidth(windowWidth / 2);
    columnL.setHeight(windowHeight);

    columnR.setLeft(windowWidth / 2);
    columnR.setWidth(windowWidth / 2);
    columnR.setHeight(windowHeight);

    // column L
    auto rect = columnL.reduced(padding);
    rect.setHeight(compoentHeight);
    invertPhaseToggleButton.setBounds(rect);
    
    rect.setTop(40);
    rect.setHeight(compoentHeight);
    stereoModeComboBox.setBounds(rect);

    //stereoWidthSlider.setBounds(10, 210, 180, 30);
    //stereoMidSideSlider.setBounds(10, 250, 180, 30);

    rect.setTop(180);
    rect.setHeight(compoentHeight);
    monoToggleButton.setBounds(rect);

    rect.setTop(220);
    rect.setHeight(compoentHeight);
    bassMonoToggleButton.setBounds(rect);

    rect.setTop(260);
    rect.setHeight(compoentHeight);
    bassMonoFrequencySlider.setBounds(rect);

    // column R

    rect = columnR.reduced(padding);
    rect.setTop(0);
    rect.setHeight(compoentHeight);
    gainSlider.setBounds(rect);

    rect.setTop(height / 2);
    rect.setHeight(compoentHeight);
    panSlider.setBounds(rect);
}
