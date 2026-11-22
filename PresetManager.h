#pragma once
#include <JuceHeader.h>
#include <map>
#include <vector>

class PresetManager : private juce::AudioProcessorValueTreeState::Listener
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager() override;

    juce::StringArray getFactoryPresetNames() const;
    juce::StringArray getUserPresetNames() const;
    juce::StringArray getAllPresetNames() const;

    bool loadPreset(const juce::String& name);
    bool loadPresetByIndex(int index);
    bool savePreset(const juce::String& name);
    bool deletePreset(const juce::String& name);

    bool loadNextPreset();
    bool loadPreviousPreset();

    juce::String getCurrentPresetName() const { return currentPresetName; }
    bool isFactoryPreset(const juce::String& name) const;
    bool hasUnsavedChanges() const { return unsavedChanges; }
    void setCurrentPresetName(const juce::String& name) { currentPresetName = name; }

    std::function<void()> onPresetChanged;
    std::function<void()> onPresetListChanged;

private:
    struct PresetDef
    {
        juce::String name;
        std::map<juce::String, float> params;
    };

    juce::AudioProcessorValueTreeState& apvts;
    juce::File presetDirectory;
    std::vector<PresetDef> factoryPresets;
    juce::StringArray userPresetNames;
    juce::String currentPresetName;
    bool unsavedChanges = false;
    bool loadingPreset = false;

    static const juce::StringArray& getParamIDs();

    void parameterChanged(const juce::String& paramID, float newValue) override;
    void buildFactoryPresets();
    void scanUserPresets();
    void applyParams(const std::map<juce::String, float>& params);
    juce::File presetFile(const juce::String& name) const;
};
