#pragma once

class MiniTextSlider : public juce::Slider {
 public:
  using juce::Slider::Slider;

  juce::AudioProcessorValueTreeState& valueTreeState;
  juce::String parameterID;
  bool disabled;  // show disabled color
  CustomPopupMenu& menu;

  MiniTextSlider(juce::AudioProcessorValueTreeState& valueTreeState,
                 const juce::String parameterID, juce::LookAndFeel* lookAndFeel,
                 CustomPopupMenu& menu, bool disabled = false)
      : valueTreeState(valueTreeState),
        parameterID(parameterID),
        menu(menu),
        disabled(disabled) {
    setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    setColour(juce::Slider::ColourIds::trackColourId,
              juce::Colours::transparentWhite);
    setColour(juce::Slider::ColourIds::textBoxTextColourId,
              themeColours.at("text"));
    setColour(juce::Slider::ColourIds::textBoxOutlineColourId,
              themeColours.at("text"));
    setColour(juce::Slider::ColourIds::textBoxBackgroundColourId,
              themeColours.at("text"));
    setVelocityBasedMode(true);
    setVelocityModeParameters(1.6, 1, 0.09);
    setLookAndFeel(lookAndFeel);
  }

  void paint(juce::Graphics& g) override {
    const auto range = valueTreeState.getParameterRange(parameterID);
    const float percent =
        range.convertTo0to1(*valueTreeState.getRawParameterValue(parameterID));
    g.setColour(disabled ? themeColours.at("grey") : themeColours.at("white"));
    g.fillRect(0, 0, getWidth(), getHeight());
    if (!disabled) {
      g.setColour(themeColours.at("blue"));
      g.fillRect(0, 0, static_cast<int>(getWidth() * percent), getHeight());
    }
    g.setColour(themeColours.at("text"));
    g.drawRect(0, 0, getWidth(), getHeight());
  }

  void mouseDown(const juce::MouseEvent& mouseEvent) override {
    auto modifiers = juce::ModifierKeys::getCurrentModifiers();
    if (modifiers.isRightButtonDown()) {
      menu.setRegisteredItems();
      menu.showDefault();
    } else {
      Slider::mouseDown(mouseEvent);
    }
  }

  void updateDisabled() { repaint(); }

  void setAndUpdateDisabled(bool flag) {
    disabled = flag;
    updateDisabled();
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniTextSlider)
};
