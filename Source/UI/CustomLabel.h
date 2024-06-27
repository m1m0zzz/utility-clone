#pragma once

class CustomLabel : public juce::Label {
 public:
  using juce::Label::Label;

  CustomPopupMenu& menu;

  CustomLabel(CustomPopupMenu& menu) : menu(menu) {
    setJustificationType(juce::Justification::centred);
  }

  void mouseDown(const juce::MouseEvent& mouseEvent) override {
    auto modifiers = juce::ModifierKeys::getCurrentModifiers();
    if (modifiers.isRightButtonDown()) {
      menu.setRegisteredItems(std::vector{
          CustomPopupMenu::ItemsID::REDO,
          CustomPopupMenu::ItemsID::UNDO,
          CustomPopupMenu::ItemsID::SHOW_DOCUMENT,
      });
      menu.showDefault();
    } else {
      Label::mouseDown(mouseEvent);
    }
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLabel)
};
