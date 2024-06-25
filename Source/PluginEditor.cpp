/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UtilityCloneAudioProcessorEditor::UtilityCloneAudioProcessorEditor(
    UtilityCloneAudioProcessor& p, juce::AudioProcessorValueTreeState& vts,
    juce::UndoManager& um)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      valueTreeState(vts),
      undoManager(um) {
  // window
  // setResizable(true, true);
  // setResizeLimits(width, height, 400, 600);
  // getConstrainer()->setFixedAspectRatio(ratio);

  // components
  gainSliderAttachment.reset(
      new SliderAttachment(valueTreeState, "gain", gainSlider));
  auto gainRange = valueTreeState.getParameterRange("gain");
  gainSlider.setRange(gainRange.start, gainRange.end);
  gainSlider.setSkewFactorFromMidPoint(0);
  gainSlider.setTextValueSuffix(" dB");
  addAndMakeVisible(gainSlider);

  invertPhaseLToggleButtonAttachment.reset(new ButtonAttachment(
      valueTreeState, "invertPhaseL", invertPhaseLToggleButton));
  addAndMakeVisible(invertPhaseLToggleButton);

  invertPhaseRToggleButtonAttachment.reset(new ButtonAttachment(
      valueTreeState, "invertPhaseR", invertPhaseRToggleButton));
  addAndMakeVisible(invertPhaseRToggleButton);

  channelModeComboBox.addItemList(channelModeList, 1);
  channelModeComboBoxAttachment.reset(new ComboBoxAttachment(
      valueTreeState, "channelMode", channelModeComboBox));
  channelModeComboBox.setColour(juce::ComboBox::ColourIds::backgroundColourId,
                                themeColours.at("lightgrey"));
  channelModeComboBox.setColour(juce::ComboBox::ColourIds::textColourId,
                                themeColours.at("text"));
  channelModeComboBox.setColour(juce::ComboBox::ColourIds::outlineColourId,
                                themeColours.at("lightblack"));
  channelModeComboBox.setColour(juce::ComboBox::ColourIds::arrowColourId,
                                themeColours.at("text"));
  channelModeComboBox.setLookAndFeel(&customLookAndFeel);
  channelModeComboBox.onChange = [this]() {
    monoToggleButton.setAndUpdateDisabled(isMonoByChannelMode());
    bassMonoListeningButton.setAndUpdateDisabled(isMonoByChannelMode());
    stereoWidthSlider.setAndUpdateDisabled(isMonoByChannelMode() || *isMono);
    stereoMidSideSlider.setAndUpdateDisabled(isMonoByChannelMode() || *isMono);
    bassMonoToggleButton.setAndUpdateDisabled(isMonoByChannelMode() || *isMono);
    bassMonoFrequencySlider.setAndUpdateDisabled(isMonoByChannelMode() ||
                                                 *isMono || !*isBassMono);
  };
  addAndMakeVisible(channelModeComboBox);

  monoToggleButtonAttachment.reset(
      new ButtonAttachment(valueTreeState, "mono", monoToggleButton));
  monoToggleButton.onClick = [this]() {
    auto state = monoToggleButton.getToggleState();
    stereoWidthSlider.setAndUpdateDisabled(isMonoByChannelMode() || state);
    stereoMidSideSlider.setAndUpdateDisabled(isMonoByChannelMode() || state);
    bassMonoToggleButton.setAndUpdateDisabled(isMonoByChannelMode() || state);
    bassMonoFrequencySlider.setAndUpdateDisabled(isMonoByChannelMode() ||
                                                 state || !*isBassMono);
  };
  addAndMakeVisible(monoToggleButton);

  panSliderAttachment.reset(
      new SliderAttachment(valueTreeState, "pan", panSlider));
  addAndMakeVisible(panSlider);

  stereoWidthSliderAttachment.reset(
      new SliderAttachment(valueTreeState, "stereoWidth", stereoWidthSlider));
  auto widthRange = valueTreeState.getParameterRange("stereoWidth");
  stereoWidthSlider.setRange(widthRange.start, widthRange.end);
  stereoWidthSlider.setSkewFactorFromMidPoint(100);
  stereoWidthSlider.setTextValueSuffix("%");
  stereoWidthSlider.setName("stereoModeSlider");
  addAndMakeVisible(stereoWidthSlider);

  stereoMidSideSliderAttachment.reset(new SliderAttachment(
      valueTreeState, "stereoMidSide", stereoMidSideSlider));
  stereoMidSideSlider.setName("stereoModeSlider");
  addAndMakeVisible(stereoMidSideSlider);
  stereoMidSideSlider.setVisible(false);

  stereoModeLabel.setText("Width", juce::dontSendNotification);
  stereoModeLabel.setColour(juce::Label::textColourId, themeColours.at("text"));
  stereoModeLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(stereoModeLabel);

  stereoModeSwitchButtonAttachment.reset(new ButtonAttachment(
      valueTreeState, "stereoMode", stereoModeSwitchButton));
  stereoModeSwitchButton.onClick = [this]() { updateStereoLabel(); };
  stereoModeSwitchButton.setColour(IconButton::ColourIds::buttonOnColourId,
                                   themeColours.at("lightgrey"));
  addAndMakeVisible(stereoModeSwitchButton);
  updateStereoLabel();

  bassMonoToggleButtonAttachment.reset(
      new ButtonAttachment(valueTreeState, "isBassMono", bassMonoToggleButton));
  bassMonoToggleButton.onClick = [this]() {
    auto state = bassMonoToggleButton.getToggleState();
    bassMonoFrequencySlider.setAndUpdateDisabled(
        *isMono || isMonoByChannelMode() || !state);
  };
  addAndMakeVisible(bassMonoToggleButton);

  bassMonoFrequencySliderAttachment.reset(new SliderAttachment(
      valueTreeState, "bassMonoFrequency", bassMonoFrequencySlider));
  bassMonoFrequencySlider.setTextValueSuffix(" Hz");
  addAndMakeVisible(bassMonoFrequencySlider);

  bassMonoListeningButtonAttachment.reset(new ButtonAttachment(
      valueTreeState, "isBassMonoListening", bassMonoListeningButton));
  addAndMakeVisible(bassMonoListeningButton);

  dcToggleButtonAttachment.reset(
      new ButtonAttachment(valueTreeState, "isDc", dcToggleButton));
  dcToggleButton.setColour(ToggleTextButton::ColourIds::buttonOnColourId,
                           themeColours.at("blue"));
  dcToggleButton.updateColourAll();
  addAndMakeVisible(dcToggleButton);

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

UtilityCloneAudioProcessorEditor::~UtilityCloneAudioProcessorEditor() {}

//==============================================================================
bool UtilityCloneAudioProcessorEditor::keyPressed(const juce::KeyPress& key) {
  // DBG("keyPressed");
  if (key.getModifiers().isCommandDown() || key.getModifiers().isCtrlDown()) {
    if (key.getKeyCode() == 'z' || key.getKeyCode() == 'Z') {
      undoManager.undo();
      updateStereoLabel();
    } else if (key.getKeyCode() == 'y' || key.getKeyCode() == 'Y') {
      undoManager.redo();
      updateStereoLabel();
    }
  }
  return true;
}

void UtilityCloneAudioProcessorEditor::mouseDown(
    const juce::MouseEvent& mouseEvent) {
  auto modifiers = juce::ModifierKeys::getCurrentModifiers();
  if (!modifiers.isRightButtonDown()) return;

  menu.setRegisteredItems(std::vector{
      CustomPopupMenu::ItemsID::REDO,
      CustomPopupMenu::ItemsID::UNDO,
      CustomPopupMenu::ItemsID::SHOW_DOCUMENT,
  });
  menu.showDefault();
}

void UtilityCloneAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(themeColours.at("grey"));

  // hr
  g.setColour(juce::Colour::fromRGB(80, 80, 80));
  g.fillRect(width / 2, 10, 1, height - 20);
}

void UtilityCloneAudioProcessorEditor::resized() {
  width = getWidth();
  height = getHeight();

  const int padding = 5;
  const int componentHeight = 22;
  const int knobHeight = 80;
  const auto buttonSize = 20;  // stereoModeSwitchButton

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

  rect.setX((columnL.getWidth() / 2) + 2);
  invertPhaseRToggleButton.setBounds(rect);

  rect = columnL.reduced(padding);
  rect.setTop(65);
  rect.setHeight(componentHeight);
  channelModeComboBox.setBounds(rect);

  rect = columnL.reduced(padding);
  rect.setTop(95);
  rect.setWidth(columnL.getWidth() - padding - buttonSize);
  rect.setHeight(componentHeight);
  stereoModeLabel.setBounds(rect);

  rect = columnL.reduced(padding);
  rect.setTop(95 + (componentHeight - buttonSize) / 2);
  rect.setX(rect.getX() + rect.getWidth() - buttonSize);
  rect.setWidth(buttonSize);
  rect.setHeight(buttonSize);
  stereoModeSwitchButton.setBounds(rect);

  rect = columnL.reduced(padding);
  rect.setTop(125);
  rect.setHeight(knobHeight);
  stereoWidthSlider.setBounds(rect);
  stereoMidSideSlider.setBounds(rect);

  rect.setTop(210);
  rect.setHeight(componentHeight);
  monoToggleButton.setBounds(rect);

  rect.setTop(240);
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

  rect.setTop(270);
  rect.setX(rect.getX() + rect.getWidth() - 35);
  rect.setHeight(componentHeight);
  rect.setWidth(30);
  dcToggleButton.setBounds(rect);
}

void UtilityCloneAudioProcessorEditor::updateStereoLabel() {
  auto boolean = !(static_cast<bool>(*stereoMode));
  stereoWidthSlider.setVisible(boolean);
  stereoMidSideSlider.setVisible(!boolean);
  stereoModeLabel.setText(boolean ? "Width" : "Mid/Side",
                          juce::sendNotification);
}

bool UtilityCloneAudioProcessorEditor::isMonoByChannelMode() {
  return channelModeList[*channelMode] == "Right" ||
         channelModeList[*channelMode] == "Left";
}
