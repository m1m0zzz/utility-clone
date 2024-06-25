#pragma once

class TogglePhaseButton : public ToggleTextButton {
 public:
  using ToggleTextButton::ToggleTextButton;

  void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override {
    auto& lf = getLookAndFeel();

    lf.drawButtonBackground(
        g, *this,
        findColour(getToggleState() ? buttonOnColourId : buttonColourId),
        shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    juce::Font font(lf.getTextButtonFont(*this, getHeight()));
    g.setFont(font);
    g.setColour(findColour(getToggleState() ? TextButton::textColourOnId
                                            : TextButton::textColourOffId)
                    .withMultipliedAlpha(isEnabled() ? 1.0f : 0.5f));

    const int yIndent = std::min(4, proportionOfHeight(0.3f));
    const int cornerSize = std::min(getHeight(), getWidth()) / 2;

    const int fontHeight = round(font.getHeight() * 0.6f);
    const int leftIndent =
        std::min(fontHeight, 2 + cornerSize / (isConnectedOnLeft() ? 4 : 2));
    const int rightIndent =
        std::min(fontHeight, 2 + cornerSize / (isConnectedOnRight() ? 4 : 2));
    const int textWidth = getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
      g.drawFittedText(getButtonText(), leftIndent, yIndent, textWidth,
                       getHeight() - yIndent * 2,
                       juce::Justification::centredRight, 2);

    const float lineThickness = 1.2f;
    g.drawEllipse(leftIndent, rightIndent, fontHeight, fontHeight,
                  lineThickness);
    g.drawLine(leftIndent + fontHeight, rightIndent, leftIndent,
               rightIndent + fontHeight, lineThickness);
  };
};
