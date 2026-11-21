#pragma once
#include <JuceHeader.h>

class LoFiTapeSimulatorAudioProcessor : public juce::AudioProcessor
{
public:
    LoFiTapeSimulatorAudioProcessor();
    ~LoFiTapeSimulatorAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Параметры
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    // Обработка эффектов
    void applyDrive(juce::AudioBuffer<float>& buffer, float drive);
    void applyFilter(juce::AudioBuffer<float>& buffer, float cutoff);
    void applyWowFlutter(juce::AudioBuffer<float>& buffer, float amount);
    void applyNoise(juce::AudioBuffer<float>& buffer, float amount);
    
    // Состояние фильтра
    juce::IIRFilter leftFilter, rightFilter;
    
    // Delay line для wow & flutter
    juce::dsp::DelayLine<float> delayLine;
    float lfoPhase = 0.0f;
    
    // Генератор шума
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> noiseDist;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoFiTapeSimulatorAudioProcessor)
};
