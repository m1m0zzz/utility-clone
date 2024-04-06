#pragma once

class ListeningButton : public juce::TextButton
{
public:
    using TextButton::TextButton;

    ListeningButton() {
        setClickingTogglesState(true);
        setColour(juce::TextButton::ColourIds::buttonColourId, themeColours.at("lightgrey"));
        setColour(juce::TextButton::ColourIds::buttonOnColourId, themeColours.at("deepblue"));
        setColour(juce::ComboBox::ColourIds::outlineColourId, themeColours.at("text"));
    }

    void paintButton(juce::Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override {
        const auto isOn = getToggleState();

        g.setColour(isOn ? findColour(buttonOnColourId) : findColour(buttonColourId));
        g.fillEllipse(0, 0, getWidth(), getHeight());

        g.setColour(findColour(juce::ComboBox::ColourIds::outlineColourId));
        g.drawEllipse(0, 0, getWidth(), getHeight(), 1);

        const auto padding = 4;
        const auto pad2 = padding * 2;
        g.drawImage(
            headphoneImage,
            padding, padding, getWidth() - pad2, getHeight() - pad2,
            0, 0, headphoneImage.getWidth(), headphoneImage.getHeight()
        );

        if (shouldDrawButtonAsHighlighted) {
            g.setColour(juce::Colour::fromFloatRGBA(0, 0, 0, 0.05));
            g.fillEllipse(0, 0, getWidth(), getHeight());
        }
    }

private:
    juce::Image headphoneImage{
        juce::ImageCache::getFromMemory(
            BinaryData::headphone_16_16_png,
            BinaryData::headphone_16_16_pngSize
        )
    };
};
