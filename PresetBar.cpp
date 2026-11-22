#include "PresetBar.h"

PresetBar::PresetBar(PresetManager& pm) : presetManager(pm)
{
    addAndMakeVisible(prevButton);
    addAndMakeVisible(nextButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(presetBox);

    prevButton.addListener(this);
    nextButton.addListener(this);
    saveButton.addListener(this);
    deleteButton.addListener(this);
    presetBox.addListener(this);

    prevButton.setTooltip("Previous preset");
    nextButton.setTooltip("Next preset");
    saveButton.setTooltip("Save current settings as a user preset");
    deleteButton.setTooltip("Delete the current user preset");

    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF5C3D1F));
    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF5C3D1F));

    presetManager.onPresetChanged = [this]() { updateCurrentName(); updateDeleteEnabled(); };
    presetManager.onPresetListChanged = [this]() { refreshList(); };

    refreshList();
    updateCurrentName();
    updateDeleteEnabled();
    startTimerHz(5);
}

PresetBar::~PresetBar()
{
    stopTimer();
    presetManager.onPresetChanged = nullptr;
    presetManager.onPresetListChanged = nullptr;
}

void PresetBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xFF2A1810).withAlpha(0.6f));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
    g.setColour(juce::Colour(0xFF8B6F47).withAlpha(0.6f));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0f, 1.0f);
}

void PresetBar::resized()
{
    auto area = getLocalBounds().reduced(4);
    prevButton.setBounds(area.removeFromLeft(28));
    area.removeFromLeft(2);
    nextButton.setBounds(area.removeFromLeft(28));
    area.removeFromLeft(8);
    deleteButton.setBounds(area.removeFromRight(56));
    area.removeFromRight(4);
    saveButton.setBounds(area.removeFromRight(56));
    area.removeFromRight(8);
    presetBox.setBounds(area);
}

void PresetBar::refreshList()
{
    updatingBox = true;
    presetBox.clear(juce::dontSendNotification);

    auto factory = presetManager.getFactoryPresetNames();
    auto user = presetManager.getUserPresetNames();

    if (!factory.isEmpty())
    {
        presetBox.addSectionHeading("Factory");
        for (int i = 0; i < factory.size(); ++i)
            presetBox.addItem(factory[i], i + 1);
    }
    if (!user.isEmpty())
    {
        presetBox.addSectionHeading("User");
        int offset = factory.size();
        for (int i = 0; i < user.size(); ++i)
            presetBox.addItem(user[i], offset + i + 1);
    }
    updatingBox = false;
    updateCurrentName();
}

void PresetBar::updateCurrentName()
{
    updatingBox = true;
    auto name = presetManager.getCurrentPresetName();
    presetBox.setText(name.isEmpty() ? "<no preset>" : name, juce::dontSendNotification);
    updatingBox = false;
}

void PresetBar::updateDeleteEnabled()
{
    auto name = presetManager.getCurrentPresetName();
    deleteButton.setEnabled(!name.isEmpty() && !presetManager.isFactoryPreset(name));
}

void PresetBar::timerCallback()
{
    auto name = presetManager.getCurrentPresetName();
    auto display = name.isEmpty() ? juce::String("<no preset>") : name;
    if (presetManager.hasUnsavedChanges()) display += " *";

    if (presetBox.getText() != display)
    {
        updatingBox = true;
        presetBox.setText(display, juce::dontSendNotification);
        updatingBox = false;
    }
}

void PresetBar::comboBoxChanged(juce::ComboBox* cb)
{
    if (updatingBox || cb != &presetBox) return;
    auto name = presetBox.getText().trim();
    if (name.isEmpty() || name == "<no preset>" || name == presetManager.getCurrentPresetName())
        return;
    presetManager.loadPreset(name);
}

void PresetBar::buttonClicked(juce::Button* b)
{
    if (b == &prevButton) presetManager.loadPreviousPreset();
    else if (b == &nextButton) presetManager.loadNextPreset();
    else if (b == &saveButton) showSaveDialog();
    else if (b == &deleteButton) showDeleteConfirmation();
}

void PresetBar::showSaveDialog()
{
    auto* aw = new juce::AlertWindow("Save Preset", "Enter a name for the preset:", juce::MessageBoxIconType::NoIcon);
    auto defaultName = presetManager.getCurrentPresetName();
    if (defaultName.isEmpty() || presetManager.isFactoryPreset(defaultName))
        defaultName = "My Preset";

    aw->addTextEditor("name", defaultName, "Name:");
    aw->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    aw->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    aw->enterModalState(true, juce::ModalCallbackFunction::create(
        [this, aw](int result)
        {
            if (result != 1) return;
            auto name = aw->getTextEditorContents("name").trim();
            if (name.isEmpty()) return;
            if (presetManager.isFactoryPreset(name))
            {
                juce::AlertWindow::showAsync(
                    juce::MessageBoxOptions().withIconType(juce::MessageBoxIconType::WarningIcon)
                        .withTitle("Cannot Save")
                        .withMessage("Cannot overwrite a factory preset. Please choose another name.")
                        .withButton("OK"), nullptr);
                return;
            }
            if (presetManager.getUserPresetNames().contains(name))
            {
                juce::AlertWindow::showAsync(
                    juce::MessageBoxOptions().withIconType(juce::MessageBoxIconType::QuestionIcon)
                        .withTitle("Overwrite?")
                        .withMessage("Preset \"" + name + "\" already exists. Overwrite?")
                        .withButton("Yes").withButton("No"),
                    [this, name](int r) { if (r == 1) presetManager.savePreset(name); });
                return;
            }
            presetManager.savePreset(name);
        }), true);
}

void PresetBar::showDeleteConfirmation()
{
    auto name = presetManager.getCurrentPresetName();
    if (name.isEmpty() || presetManager.isFactoryPreset(name)) return;

    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions().withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle("Delete Preset")
            .withMessage("Delete preset \"" + name + "\"?")
            .withButton("Delete").withButton("Cancel"),
        [this, name](int result) { if (result == 1) presetManager.deletePreset(name); });
}
