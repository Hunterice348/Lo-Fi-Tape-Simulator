#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LoFiTapeSimulatorAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    LoFiTapeSimulatorAudioProcessorEditor(LoFiTapeSimulatorAudioProcessor&);
    ~LoFiTapeSimulatorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    LoFiTapeSimulatorAudioProcessor& audioProcessor;
    
    juce::Slider driveSlider, filterSlider, wowSlider, noiseSlider, mixSlider;
    juce::Label driveLabel, filterLabel, wowLabel, noiseLabel, mixLabel;
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment, filterAttachment, wowAttachment, noiseAttachment, mixAttachment;
    
    float reelAngle = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoFiTapeSimulatorAudioProcessorEditor)
};
