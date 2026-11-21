# 📼 Lo-Fi Tape Simulator VST3

[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS-blue.svg)]()

A vintage-style audio effect plugin that simulates the warm, imperfect sound of analog cassette tapes. Built with **JUCE** and **C++**.

![Lo-Fi Tape Simulator UI](assets/lofi-tape-simulator-ui.png)

## ✨ Features

- 🎚️ **Drive**: Analog-style saturation using hyperbolic tangent clipping
- 🎛️ **Filter**: Low-pass filter to mimic limited tape frequency response
- 🌀 **Wow & Flutter**: Realistic pitch modulation using LFO delay lines
- 📻 **Noise**: Tape hiss simulation
- 🎨 **Mix**: Dry/wet balance control
- 🎬 **Animated UI**: Rotating reel-to-reel visualization

## 🎵 Perfect For

- Lo-fi hip hop
- Bedroom pop
- Indie music
- Vintage sound design
- Adding warmth to digital recordings

## 🚀 Installation

### Building from Source

#### Prerequisites
- CMake 3.15+
- C++17 Compiler (MSVC, Clang, GCC)
- JUCE Framework

#### Steps
```bash
# Clone the repository
git clone https://github.com/Hunterice348/Lo-Fi-Tape-Simulator.git
cd Lo-Fi-Tape-Simulator

# Download JUCE
git clone https://github.com/juce-framework/JUCE.git

# Build (Windows)
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Build (macOS)
cmake -B build -G Xcode
cmake --build build --config Release