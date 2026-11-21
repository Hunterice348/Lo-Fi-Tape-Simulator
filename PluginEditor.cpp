#include "PluginProcessor.h"
#include "PluginEditor.h"

LoFiTapeSimulatorAudioProcessorEditor::LoFiTapeSimulatorAudioProcessorEditor(LoFiTapeSimulatorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(600, 400);
    startTimerHz(60);
    
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, 
                              std::unique_ptr<SliderAttachment>& attachment, const juce::String& paramID)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFFD4A574));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFF8B6F47));
        addAndMakeVisible(slider);
        
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xFFE8D5C4));
        addAndMakeVisible(label);
        
        attachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, paramID, slider);
    };
    
    setupSlider(driveSlider, driveLabel, "DRIVE", driveAttachment, "DRIVE");
    setupSlider(filterSlider, filterLabel, "FILTER", filterAttachment, "FILTER");
    setupSlider(wowSlider, wowLabel, "WOW", wowAttachment, "WOW");
    setupSlider(noiseSlider, noiseLabel, "NOISE", noiseAttachment, "NOISE");
    setupSlider(mixSlider, mixLabel, "MIX", mixAttachment, "MIX");
}

LoFiTapeSimulatorAudioProcessorEditor::~LoFiTapeSimulatorAudioProcessorEditor() { stopTimer(); }

void LoFiTapeSimulatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF2C1810));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xFF3D2817), 0, 0, juce::Colour(0xFF1A0F0A), 0, getHeight(), false));
    g.fillAll();
    
    g.setColour(juce::Colour(0xFFD4A574));
    g.setFont(32.0f);
    g.drawText("LO-FI TAPE", getLocalBounds().removeFromTop(60), juce::Justification::centred);
    g.setFont(16.0f);
    g.drawText("SIMULATOR", getLocalBounds().removeFromTop(80), juce::Justification::centred);
    
    auto reelArea = getLocalBounds().removeFromTop(150).removeFromTop(100);
    
    auto leftReelArea = reelArea.removeFromLeft(100).reduced(10);
    g.setColour(juce::Colour(0xFF8B6F47));
    g.fillEllipse(leftReelArea);
    g.setColour(juce::Colour(0xFF2C1810));
    for (int i = 0; i < 6; ++i)
    {
        float angle = reelAngle + i * juce::MathConstants<float>::pi / 3.0f;
        float cx = leftReelArea.getCentreX(), cy = leftReelArea.getCentreY();
        g.drawLine(cx + std::cos(angle) * 15, cy + std::sin(angle) * 15, cx + std::cos(angle) * 35, cy + std::sin(angle) * 35, 3.0f);
    }
    
    reelArea.removeFromLeft(20);
    auto rightReelArea = reelArea.removeFromLeft(100).reduced(10);
    g.setColour(juce::Colour(0xFF8B6F47));
    g.fillEllipse(rightReelArea);
    g.setColour(juce::Colour(0xFF2C1810));
    for (int i = 0; i < 6; ++i)
    {
        float angle = -reelAngle + i * juce::MathConstants<float>::pi / 3.0f;
        float cx = rightReelArea.getCentreX(), cy = rightReelArea.getCentreY();
        g.drawLine(cx + std::cos(angle) * 15, cy + std::sin(angle) * 15, cx + std::cos(angle) * 35, cy + std::sin(angle) * 35, 3.0f);
    }
}

void LoFiTapeSimulatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(160);
    auto sliderArea = area.removeFromTop(150);
    int sliderWidth = sliderArea.getWidth() / 5;
    
    auto positionSlider = [&sliderWidth](juce::Slider& slider, juce::Label& label, juce::Rectangle<int>& area)
    {
        auto sliderBounds = area.removeFromLeft(sliderWidth).reduced(10);
        label.setBounds(sliderBounds.removeFromTop(30));
        slider.setBounds(sliderBounds);
    };
    
    positionSlider(driveSlider, driveLabel, sliderArea);
    positionSlider(filterSlider, filterLabel, sliderArea);
    positionSlider(wowSlider, wowLabel, sliderArea);
    positionSlider(noiseSlider, noiseLabel, sliderArea);
    positionSlider(mixSlider, mixLabel, sliderArea);
}

void LoFiTapeSimulatorAudioProcessorEditor::timerCallback()
{
    reelAngle += 0.05f;
    if (reelAngle > juce::MathConstants<float>::twoPi) reelAngle -= juce::MathConstants<float>::twoPi;
    repaint();
}
