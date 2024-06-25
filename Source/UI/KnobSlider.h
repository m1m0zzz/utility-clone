#pragma once

class KnobSlider : public juce::Slider {
 public:
  using juce::Slider::Slider;

  bool disabled;  // show disabled color
  CustomPopupMenu& menu;

  KnobSlider(juce::LookAndFeel* lookAndFeel, CustomPopupMenu& menu, bool disabled = false)
      : disabled(disabled), menu(menu) {
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 20);
    setAndUpdateDisabled(disabled);
    setLookAndFeel(lookAndFeel);
  }

  void mouseDown(const juce::MouseEvent& mouseEvent) override {
    auto modifiers = juce::ModifierKeys::getCurrentModifiers();
    if (modifiers.isRightButtonDown()) {
      auto items = std::vector{
          CustomPopupMenu::ItemsID::REDO,
          CustomPopupMenu::ItemsID::UNDO,
          CustomPopupMenu::ItemsID::SHOW_DOCUMENT,
      };
      if (getName() == "stereoModeSlider") {
        items.push_back(CustomPopupMenu::ItemsID::TOGGLE_STEREO_MODE);
      }
      menu.setRegisteredItems(items);
      menu.showDefault();
    } else {
      Slider::mouseDown(mouseEvent);
    }
  }

  void updateColourAll() {
    setColour(juce::Slider::ColourIds::textBoxTextColourId, themeColours.at("text"));
    if (!disabled) {
      setColour(juce::Slider::ColourIds::thumbColourId, themeColours.at("lightblack"));
      setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                themeColours.at("lightblack"));
      setColour(juce::Slider::ColourIds::rotarySliderFillColourId, themeColours.at("blue"));
      setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("white"));
    } else {
      setColour(juce::Slider::ColourIds::thumbColourId, themeColours.at("disabled"));
      setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, themeColours.at("disabled"));
      setColour(juce::Slider::ColourIds::rotarySliderFillColourId, themeColours.at("white"));
      setColour(juce::Label::ColourIds::outlineWhenEditingColourId, themeColours.at("white"));
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
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobSlider)
};
