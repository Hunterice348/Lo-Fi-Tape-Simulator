#pragma once
#include <JuceHeader.h>
#include "PresetManager.h"

class PresetBar : public juce::Component,
                  private juce::ComboBox::Listener,
                  private juce::Button::Listener,
                  private juce::Timer
{
public:
    explicit PresetBar(PresetManager& pm);
    ~PresetBar() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PresetManager& presetManager;
    juce::TextButton prevButton { "<" };
    juce::TextButton nextButton { ">" };
    juce::TextButton saveButton { "SAVE" };
    juce::TextButton deleteButton { "DEL" };
    juce::ComboBox presetBox;

    bool updatingBox = false;

    void comboBoxChanged(juce::ComboBox*) override;
    void buttonClicked(juce::Button*) override;
    void timerCallback() override;

    void refreshList();
    void updateCurrentName();
    void updateDeleteEnabled();
    void showSaveDialog();
    void showDeleteConfirmation();
};
