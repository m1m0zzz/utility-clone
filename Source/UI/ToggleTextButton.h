#pragma once

class ToggleTextButton : public juce::TextButton {
 public:
  bool disabled;  // show disabled color
  CustomPopupMenu& menu;

  enum ColourIds {
    buttonColourId,
    buttonOnColourId,
    textColourId,
    outlineColourId,
    buttonDisabledOnColourId,
    textDisabledColourId,
  };

  ToggleTextButton(juce::String text, juce::LookAndFeel* lookAndFeel,
                   CustomPopupMenu& menu, bool disabled = false)
      : disabled(disabled), menu(menu) {
    setClickingTogglesState(true);
    setButtonText(text);
    setColour(buttonColourId, themeColours.at("lightgrey"));
    setColour(buttonOnColourId, themeColours.at("orange"));
    setColour(textColourId, themeColours.at("text"));
    setColour(outlineColourId, themeColours.at("lightblack"));
    setColour(buttonDisabledOnColourId, themeColours.at("white"));
    setColour(textDisabledColourId, themeColours.at("disabled"));
    setAndUpdateDisabled(disabled);
    setLookAndFeel(lookAndFeel);
  }

  void mouseDown(const juce::MouseEvent& mouseEvent) override {
    auto modifiers = juce::ModifierKeys::getCurrentModifiers();
    if (modifiers.isRightButtonDown()) {
      menu.setRegisteredItems();
      menu.showDefault();
    } else {
      TextButton::mouseDown(mouseEvent);
    }
  }

  void updateColourAll() {
    if (!disabled) {
      setColour(TextButton::ColourIds::buttonOnColourId,
                findColour(buttonOnColourId));
      setColour(TextButton::ColourIds::buttonColourId,
                findColour(buttonColourId));
      setColour(TextButton::ColourIds::textColourOnId,
                findColour(textColourId));
      setColour(TextButton::ColourIds::textColourOffId,
                findColour(textColourId));
      setColour(juce::ComboBox::ColourIds::outlineColourId,
                findColour(outlineColourId));
    } else {
      setColour(TextButton::ColourIds::buttonOnColourId,
                findColour(buttonDisabledOnColourId));
      setColour(TextButton::ColourIds::buttonColourId,
                findColour(buttonColourId));
      setColour(TextButton::ColourIds::textColourOnId,
                findColour(textDisabledColourId));
      setColour(TextButton::ColourIds::textColourOffId,
                findColour(textDisabledColourId));
      setColour(juce::ComboBox::ColourIds::outlineColourId,
                findColour(outlineColourId));
    }
  }

  void updateDisabled() {
    updateColourAll();
    repaint();
  }

  void setAndUpdateDisabled(bool flag) {
    disabled = flag;
    updateDisabled();
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleTextButton)
};
