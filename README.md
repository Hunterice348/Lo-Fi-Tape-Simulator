# 📼 Lo-Fi Tape Simulator VST3

[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS-blue.svg)]()
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()
[![JUCE](https://img.shields.io/badge/JUCE-7.0-orange.svg)](https://juce.com/)
[![Status](https://img.shields.io/badge/status-source%20code-yellow.svg)]()

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

## 🛠️ Building from Source

### Prerequisites

| Tool | Version | Link |
|------|---------|------|
| CMake | 3.15+ | [cmake.org](https://cmake.org/download/) |
| C++ Compiler | C++17 | [Visual Studio 2022](https://visualstudio.microsoft.com/) / [Xcode](https://developer.apple.com/xcode/) |
| JUCE Framework | 7.0+ | [juce.com](https://juce.com/) |

> ⚠️ **Важно:** Папка `JUCE` **не включена** в репозиторий (см. `.gitignore`). Её нужно скачать отдельно перед сборкой.

### Шаг 1. Клонируй проект

```bash
git clone https://github.com/Hunterice348/Lo-Fi-Tape-Simulator.git
cd Lo-Fi-Tape-Simulator
