/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UtilitycloneAudioProcessorEditor::UtilitycloneAudioProcessorEditor (
    UtilitycloneAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    gainSliderAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    addAndMakeVisible(gainSlider);

    pheseInvertToggleButtonAttachment.reset(new ButtonAttachment(valueTreeState, "invertPhase", pheseInvertToggleButton));
    pheseInvertToggleButton.setClickingTogglesState(true);
    pheseInvertToggleButton.setButtonText("Invert Phase");
    pheseInvertToggleButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(255, 183, 53));
    pheseInvertToggleButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(50, 50, 50));
    pheseInvertToggleButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    addAndMakeVisible(pheseInvertToggleButton);

    setSize (200, 400);
}

UtilitycloneAudioProcessorEditor::~UtilitycloneAudioProcessorEditor()
{
}

//==============================================================================
void UtilitycloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void UtilitycloneAudioProcessorEditor::resized()
{
    gainSlider.setBounds(10, 10, 180, 30);
    pheseInvertToggleButton.setBounds(10, 50, 180, 30);
}
