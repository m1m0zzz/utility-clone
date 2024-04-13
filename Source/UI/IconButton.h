#pragma once

class IconButton : public juce::TextButton
{
public:
    using TextButton::TextButton;

    enum ColourIds {
        buttonColourId,
        buttonOnColourId,
        outlineColourId,
    };

    CustomPopupMenu& menu;

    IconButton(juce::Image iconImage, CustomPopupMenu& menu) : iconImage(iconImage), menu(menu) {
        setClickingTogglesState(true);
        setColour(buttonColourId, themeColours.at("lightgrey"));
        setColour(buttonOnColourId, themeColours.at("deepblue"));
        setColour(outlineColourId, themeColours.at("text"));
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool) override {
        auto c = getToggleState() ? findColour(buttonOnColourId) : findColour(buttonColourId);
        g.setColour(c);
        g.fillEllipse(0, 0, getWidth(), getHeight());

        g.setColour(findColour(outlineColourId));
        g.drawEllipse(0, 0, getWidth(), getHeight(), 1);

        const auto padding = 4;
        const auto pad2 = padding * 2;
        g.drawImage(
            iconImage,
            padding, padding, getWidth() - pad2, getHeight() - pad2,
            0, 0, iconImage.getWidth(), iconImage.getHeight()
        );

        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colour::fromFloatRGBA(0, 0, 0, 0.05));
            g.fillEllipse(0, 0, getWidth(), getHeight());
        }
    }

    void mouseDown(const juce::MouseEvent& mouseEvent) override {
        auto modifiers = juce::ModifierKeys::getCurrentModifiers();
        if (modifiers.isRightButtonDown()) {
            menu.setRegisteredItems();
            menu.showDefault();
        }
        else {
            TextButton::mouseDown(mouseEvent);
        }
    }

    void setIconImage(juce::Image newImage) {
        iconImage = newImage;
    }

    juce::Image getIconImage() {
        return iconImage;
    }

private:
    juce::Image iconImage;
};
