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
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    gainSlider.setTextValueSuffix(" dB");
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

    setSize (200, 400);
}

UtilitycloneAudioProcessorEditor::~UtilitycloneAudioProcessorEditor()
{
}

//==============================================================================
void UtilitycloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void UtilitycloneAudioProcessorEditor::resized()
{
    invertPhaseToggleButton.setBounds(10, 10, 180, 30);
    monoToggleButton.setBounds(10, 50, 180, 30);
    gainSlider.setBounds(10, 90, 180, 30);
    panSlider.setBounds(10, 130, 180, 30);
    stereoModeComboBox.setBounds(10, 170, 180, 30);
    stereoWidthSlider.setBounds(10, 210, 180, 30);
    stereoMidSideSlider.setBounds(10, 250, 180, 30);
}
