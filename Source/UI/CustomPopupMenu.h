#pragma once

// common PopupMenu in this plugin
class CustomPopupMenu : public juce::PopupMenu
{
public:
    using juce::PopupMenu::PopupMenu;

    juce::LookAndFeel* lookAndFeel;
    juce::UndoManager& undoManager;
    std::atomic<float>* stereoMode;
    std::function<void()> updateStereoLabel;

    const juce::URL documentURL = juce::URL("https://github.com/m1m0zzz/utility-clone");

    enum class ItemsID {
        UNDO = 1,
        REDO,
        TOGGLE_STEREO_MODE,
        SHOW_DOCUMENT
    };

    CustomPopupMenu(juce::LookAndFeel* lookAndFeel, juce::UndoManager& undoManager,
        std::atomic<float>* stereoMode, std::function<void()> updateStereoLabel) : 
        lookAndFeel(lookAndFeel), undoManager(undoManager), stereoMode(stereoMode), updateStereoLabel(updateStereoLabel) {
        setLookAndFeel(lookAndFeel);
    }

    void setRegisteredItems(std::vector<ItemsID> ids = std::vector{
            CustomPopupMenu::ItemsID::REDO,
            CustomPopupMenu::ItemsID::UNDO,
            CustomPopupMenu::ItemsID::SHOW_DOCUMENT,
        }) {
        clear();
        if (includes(ids, ItemsID::UNDO))
            addColouredItem(static_cast<int>(ItemsID::UNDO), "Undo (Ctrl+z)",
                lookAndFeel->findColour(juce::PopupMenu::ColourIds::textColourId), undoManager.canUndo());
        if (includes(ids, ItemsID::REDO))
            addColouredItem(static_cast<int>(ItemsID::REDO), "Redo (Ctrl+y)",
                lookAndFeel->findColour(juce::PopupMenu::ColourIds::textColourId), undoManager.canRedo());
        if (includes(ids, ItemsID::TOGGLE_STEREO_MODE)) {
            addSeparator();
            addItem(
                static_cast<int>(ItemsID::TOGGLE_STEREO_MODE),
                juce::String("Toggle ") + (*stereoMode ? "Width" : "Mid/Side") + " Mode"
            );
        }
        if (includes(ids, ItemsID::SHOW_DOCUMENT)) {
            addSeparator();
            addItem(static_cast<int>(ItemsID::SHOW_DOCUMENT), "Show document (browser)");
        }
    }

    void showDefault() {
        showMenuAsync(juce::PopupMenu::Options(),
            [this](int result)
            {
                switch (result) {
                case 0:
                    break; // nothing
                case static_cast<int>(ItemsID::UNDO):
                    undoManager.undo();
                    if (updateStereoLabel) updateStereoLabel();
                    break;
                case static_cast<int>(ItemsID::REDO):
                    undoManager.redo();
                    if (updateStereoLabel) updateStereoLabel();
                    break;
                case static_cast<int>(ItemsID::TOGGLE_STEREO_MODE):
                    *stereoMode = static_cast<float>(!*stereoMode);
                    if (updateStereoLabel) updateStereoLabel();
                    break;
                case static_cast<int>(ItemsID::SHOW_DOCUMENT):
                    documentURL.launchInDefaultBrowser();
                    break;
                }
            }
        );
    }

private:
    bool includes(std::vector<ItemsID> ids, ItemsID id) {
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPopupMenu)
};
