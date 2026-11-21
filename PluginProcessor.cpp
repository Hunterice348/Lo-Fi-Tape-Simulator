#include "PluginProcessor.h"
#include "PluginEditor.h"

LoFiTapeSimulatorAudioProcessor::LoFiTapeSimulatorAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()),
      gen(std::random_device{}()),
      noiseDist(-1.0f, 1.0f)
{
}

LoFiTapeSimulatorAudioProcessor::~LoFiTapeSimulatorAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout 
LoFiTapeSimulatorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"DRIVE", 1}, "Drive", juce::NormalisableRange<float>(0.0f, 100.0f), 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FILTER", 1}, "Filter", juce::NormalisableRange<float>(200.0f, 20000.0f, 1.0f, 0.3f), 8000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"WOW", 1}, "Wow & Flutter", juce::NormalisableRange<float>(0.0f, 100.0f), 30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"NOISE", 1}, "Noise", juce::NormalisableRange<float>(0.0f, 100.0f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"MIX", 1}, "Mix", juce::NormalisableRange<float>(0.0f, 100.0f), 50.0f));
    
    return { params.begin(), params.end() };
}

void LoFiTapeSimulatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    maxDelaySamples = sampleRate * 0.1;
    delayBuffers.resize(2);
    for (auto& buf : delayBuffers) buf.resize(maxDelaySamples, 0.0f);
    delayWritePos = 0;
    leftFilter.reset();
    rightFilter.reset();
    
    auto spec = juce::dsp::ProcessSpec();
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    leftFilter.prepare(spec);
    rightFilter.prepare(spec);
}

void LoFiTapeSimulatorAudioProcessor::releaseResources() {}

void LoFiTapeSimulatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    float drive = *apvts.getRawParameterValue("DRIVE") / 100.0f;
    float filterCutoff = *apvts.getRawParameterValue("FILTER");
    float wowAmount = *apvts.getRawParameterValue("WOW") / 100.0f;
    float noiseAmount = *apvts.getRawParameterValue("NOISE") / 100.0f;
    float mix = *apvts.getRawParameterValue("MIX") / 100.0f;
    
    applyDrive(buffer, drive);
    applyFilter(buffer, filterCutoff);
    applyWowFlutter(buffer, wowAmount);
    applyNoise(buffer, noiseAmount);
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* dryData = dryBuffer.getReadPointer(channel);
        auto* wetData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            wetData[sample] = dryData[sample] * (1.0f - mix) + wetData[sample] * mix;
    }
}

void LoFiTapeSimulatorAudioProcessor::applyDrive(juce::AudioBuffer<float>& buffer, float drive)
{
    if (drive < 0.01f) return;
    float gain = 1.0f + drive * 8.0f;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            data[sample] = std::tanh(data[sample] * gain) * 0.8f;
    }
}

void LoFiTapeSimulatorAudioProcessor::applyFilter(juce::AudioBuffer<float>& buffer, float cutoff)
{
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), cutoff, 0.707f);
    *leftFilter.coefficients = *coefficients;
    *rightFilter.coefficients = *coefficients;
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        buffer.setSample(0, sample, leftFilter.processSample(buffer.getSample(0, sample)));
        if (buffer.getNumChannels() > 1)
            buffer.setSample(1, sample, rightFilter.processSample(buffer.getSample(1, sample)));
    }
}

void LoFiTapeSimulatorAudioProcessor::applyWowFlutter(juce::AudioBuffer<float>& buffer, float amount)
{
    if (amount < 0.01f) return;
    double sampleRate = getSampleRate();
    float lfoFreq = 0.5f + amount * 3.0f;
    float modulationDepth = amount * 0.015f * sampleRate;
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float lfo = std::sin(lfoPhase * juce::MathConstants<float>::twoPi);
        lfoPhase += lfoFreq / sampleRate;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
        
        float delayMod = modulationDepth * (1.0f + lfo);
        int readPos = delayWritePos - (int)delayMod;
        if (readPos < 0) readPos += maxDelaySamples;
        float frac = delayMod - std::floor(delayMod);
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            delayBuffers[channel][delayWritePos] = buffer.getSample(channel, sample);
            int readPos2 = (readPos + 1) % maxDelaySamples;
            float s1 = delayBuffers[channel][readPos];
            float s2 = delayBuffers[channel][readPos2];
            buffer.setSample(channel, sample, s1 * (1.0f - frac) + s2 * frac);
        }
        delayWritePos = (delayWritePos + 1) % maxDelaySamples;
    }
}

void LoFiTapeSimulatorAudioProcessor::applyNoise(juce::AudioBuffer<float>& buffer, float amount)
{
    if (amount < 0.01f) return;
    float noiseLevel = amount * 0.03f;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            data[sample] += noiseDist(gen) * noiseLevel;
    }
}

juce::AudioProcessorEditor* LoFiTapeSimulatorAudioProcessor::createEditor()
{
    return new LoFiTapeSimulatorAudioProcessorEditor(*this);
}

bool LoFiTapeSimulatorAudioProcessor::hasEditor() const { return true; }
const juce::String LoFiTapeSimulatorAudioProcessor::getName() const { return "Lo-Fi Tape Simulator"; }
void LoFiTapeSimulatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
void LoFiTapeSimulatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LoFiTapeSimulatorAudioProcessor();
}
