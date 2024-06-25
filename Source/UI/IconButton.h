#pragma once

class IconButton : public juce::TextButton {
 public:
  using TextButton::TextButton;

  enum ColourIds {
    buttonColourId,
    buttonOnColourId,
    outlineColourId,
    imageColourId,
    imageDisabledColourId,
    buttonDisabledOnColourId,
  };

  bool disabled;  // show disabled color
  CustomPopupMenu& menu;

  IconButton(juce::Image iconImage, CustomPopupMenu& menu, bool disabled = false,
             bool enableMaskImage = false)
      : iconImage(iconImage), menu(menu), disabled(disabled), enableMaskImage(enableMaskImage) {
    setClickingTogglesState(true);
    setColour(buttonColourId, themeColours.at("lightgrey"));
    setColour(buttonOnColourId, themeColours.at("deepblue"));
    setColour(outlineColourId, themeColours.at("lightblack"));
    setColour(imageColourId, themeColours.at("text"));
    setColour(buttonDisabledOnColourId, themeColours.at("white"));
    setColour(imageDisabledColourId, themeColours.at("disabled"));
    setAndUpdateDisabled(disabled);
  }

  void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool) override {
    auto rect = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(getWidth()),
                                       static_cast<float>(getHeight()))
                    .reduced(0.5f, 0.5f);
    auto c = getToggleState()
                 ? (disabled ? findColour(buttonDisabledOnColourId) : findColour(buttonOnColourId))
                 : findColour(buttonColourId);
    g.setColour(c);
    g.fillEllipse(rect);

    g.setColour(findColour(outlineColourId));
    g.drawEllipse(rect, 1.0f);

    const auto padding = 4;
    const auto pad2 = padding * 2;
    g.setColour(disabled ? findColour(imageDisabledColourId) : findColour(imageColourId));
    g.drawImage(iconImage, padding, padding, getWidth() - pad2, getHeight() - pad2, 0, 0,
                iconImage.getWidth(), iconImage.getHeight(), enableMaskImage);

    if (shouldDrawButtonAsHighlighted) {
      g.setColour(juce::Colour::fromFloatRGBA(0, 0, 0, 0.05f));
      g.fillEllipse(rect);
    }
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

  void setIconImage(juce::Image newImage) { iconImage = newImage; }

  juce::Image getIconImage() { return iconImage; }

  void updateDisabled() { repaint(); }

  void setAndUpdateDisabled(bool flag) {
    disabled = flag;
    updateDisabled();
  }

  bool getEnableMaskImage() { return enableMaskImage; }

  void settEnableMaskImage(bool enableMaskImage) {
    enableMaskImage = enableMaskImage;
    repaint();
  }

 private:
  bool enableMaskImage = false;
  juce::Image iconImage;
};
