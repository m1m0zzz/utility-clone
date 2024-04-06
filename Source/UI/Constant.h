#pragma once

const std::unordered_map<std::string, juce::Colour> themeColours = {
    { "blue",       juce::Colour::fromRGB(85, 222, 246) },
    { "lightblue",  juce::Colour::fromRGB(191, 233, 255) },
    { "deepblue",   juce::Colour::fromRGB(56, 115, 255) },
    { "orange",     juce::Colour::fromRGB(255, 177, 0) },
    { "white",      juce::Colour::fromRGB(220, 220,220) },
    { "lightgrey",  juce::Colour::fromRGB(183, 183, 183) },
    { "grey",       juce::Colour::fromRGB(143, 143, 143) },
    { "disabled",   juce::Colour::fromRGB(105, 105,105) },
    { "lightblack", juce::Colour::fromRGB(42, 42, 42) },
    { "text",       juce::Colours::black },
};

enum class E_MENUS {
    UNDO = 1,
    REDO,
    TOGGLE_STEREO_MODE,
    SHOW_DOCUMENT
};
