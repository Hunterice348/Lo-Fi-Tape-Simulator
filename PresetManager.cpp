#include "PresetManager.h"

const juce::StringArray& PresetManager::getParamIDs()
{
    static const juce::StringArray ids { "DRIVE", "FILTER", "WOW", "NOISE", "MIX" };
    return ids;
}

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& a)
    : apvts(a)
{
    presetDirectory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("LoFiLabs")
        .getChildFile("Lo-Fi Tape Simulator")
        .getChildFile("Presets");

    if (!presetDirectory.exists())
        presetDirectory.createDirectory();

    buildFactoryPresets();
    scanUserPresets();

    for (auto& id : getParamIDs())
        apvts.addParameterListener(id, this);
}

PresetManager::~PresetManager()
{
    for (auto& id : getParamIDs())
        apvts.removeParameterListener(id, this);
}

void PresetManager::parameterChanged(const juce::String&, float)
{
    if (!loadingPreset)
        unsavedChanges = true;
}

void PresetManager::buildFactoryPresets()
{
    factoryPresets = {
        { "Init",           { {"DRIVE", 20}, {"FILTER", 8000},  {"WOW", 30}, {"NOISE", 10}, {"MIX", 50}  } },
        { "Vintage Warmth", { {"DRIVE", 35}, {"FILTER", 6000},  {"WOW", 20}, {"NOISE", 5},  {"MIX", 40}  } },
        { "Cassette Hiss",  { {"DRIVE", 25}, {"FILTER", 9000},  {"WOW", 40}, {"NOISE", 35}, {"MIX", 60}  } },
        { "Lo-Fi Hip Hop",  { {"DRIVE", 60}, {"FILTER", 4000},  {"WOW", 50}, {"NOISE", 25}, {"MIX", 70}  } },
        { "Dusty Tape",     { {"DRIVE", 50}, {"FILTER", 5000},  {"WOW", 60}, {"NOISE", 45}, {"MIX", 65}  } },
        { "Old Radio",      { {"DRIVE", 40}, {"FILTER", 2500},  {"WOW", 30}, {"NOISE", 20}, {"MIX", 100} } },
        { "Wobbly Memory",  { {"DRIVE", 15}, {"FILTER", 10000}, {"WOW", 85}, {"NOISE", 8},  {"MIX", 55}  } },
        { "Clean Analog",   { {"DRIVE", 10}, {"FILTER", 15000}, {"WOW", 10}, {"NOISE", 3},  {"MIX", 25}  } },
    };
}

void PresetManager::scanUserPresets()
{
    userPresetNames.clear();
    if (!presetDirectory.exists()) return;

    for (auto& f : presetDirectory.findChildFiles(juce::File::findFiles, false, "*.xml"))
        userPresetNames.add(f.getFileNameWithoutExtension());

    userPresetNames.sort(true);
}

juce::File PresetManager::presetFile(const juce::String& name) const
{
    return presetDirectory.getChildFile(name + ".xml");
}

juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray result;
    for (auto& p : factoryPresets)
        result.add(p.name);
    return result;
}

juce::StringArray PresetManager::getUserPresetNames() const { return userPresetNames; }

juce::StringArray PresetManager::getAllPresetNames() const
{
    auto result = getFactoryPresetNames();
    result.addArray(userPresetNames);
    return result;
}

bool PresetManager::isFactoryPreset(const juce::String& name) const
{
    for (auto& p : factoryPresets)
        if (p.name == name) return true;
    return false;
}

void PresetManager::applyParams(const std::map<juce::String, float>& params)
{
    loadingPreset = true;
    for (auto& kv : params)
    {
        if (auto* param = apvts.getParameter(kv.first))
            param->setValueNotifyingHost(param->convertTo0to1(kv.second));
    }
    loadingPreset = false;
    unsavedChanges = false;
}

bool PresetManager::loadPreset(const juce::String& name)
{
    if (name.isEmpty()) return false;

    for (auto& p : factoryPresets)
    {
        if (p.name == name)
        {
            applyParams(p.params);
            currentPresetName = name;
            if (onPresetChanged) onPresetChanged();
            return true;
        }
    }

    auto file = presetFile(name);
    if (file.existsAsFile())
    {
        if (auto xml = juce::XmlDocument::parse(file))
        {
            auto tree = juce::ValueTree::fromXml(*xml);
            if (tree.isValid())
            {
                loadingPreset = true;
                apvts.replaceState(tree);
                loadingPreset = false;
                unsavedChanges = false;
                currentPresetName = name;
                if (onPresetChanged) onPresetChanged();
                return true;
            }
        }
    }
    return false;
}

bool PresetManager::loadPresetByIndex(int index)
{
    auto names = getAllPresetNames();
    if (index >= 0 && index < names.size())
        return loadPreset(names[index]);
    return false;
}

bool PresetManager::savePreset(const juce::String& name)
{
    auto trimmed = name.trim();
    if (trimmed.isEmpty() || trimmed.containsAnyOf("\\/:*?\"<>|") || isFactoryPreset(trimmed))
        return false;

    auto xml = apvts.copyState().createXml();
    if (xml == nullptr) return false;

    auto file = presetFile(trimmed);
    if (!file.replaceWithText(xml->toString()))
        return false;

    if (!userPresetNames.contains(trimmed))
    {
        userPresetNames.add(trimmed);
        userPresetNames.sort(true);
    }

    currentPresetName = trimmed;
    unsavedChanges = false;

    if (onPresetListChanged) onPresetListChanged();
    if (onPresetChanged) onPresetChanged();
    return true;
}

bool PresetManager::deletePreset(const juce::String& name)
{
    if (isFactoryPreset(name)) return false;

    auto file = presetFile(name);
    if (!file.existsAsFile() || !file.deleteFile()) return false;

    userPresetNames.removeString(name);

    if (currentPresetName == name)
    {
        currentPresetName.clear();
        if (onPresetChanged) onPresetChanged();
    }
    if (onPresetListChanged) onPresetListChanged();
    return true;
}

bool PresetManager::loadNextPreset()
{
    auto names = getAllPresetNames();
    if (names.isEmpty()) return false;
    int idx = names.indexOf(currentPresetName);
    idx = (idx < 0) ? 0 : (idx + 1) % names.size();
    return loadPreset(names[idx]);
}

bool PresetManager::loadPreviousPreset()
{
    auto names = getAllPresetNames();
    if (names.isEmpty()) return false;
    int idx = names.indexOf(currentPresetName);
    idx = (idx < 0) ? names.size() - 1 : (idx - 1 + names.size()) % names.size();
    return loadPreset(names[idx]);
}
