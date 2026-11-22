# 📼 Lo-Fi Tape Simulator VST3

A vintage-style audio effect plugin that simulates the warm, imperfect sound of analog cassette tapes. Built with **JUCE** and **C++**.

## ✨ Features

- 🎚️ **Drive** — analog-style saturation (hyperbolic tangent)
- 🎛️ **Filter** — low-pass filter for tape frequency response
- 🌀 **Wow & Flutter** — pitch modulation via LFO delay lines
- 📻 **Noise** — tape hiss simulation
- 🎨 **Mix** — dry/wet balance
- 🎬 **Animated UI** — rotating reel-to-reel visualization
- 📁 **Preset System** — 8 factory presets + unlimited user presets

## 📁 Preset System

### Factory Presets
- Init
- Vintage Warmth
- Cassette Hiss
- Lo-Fi Hip Hop
- Dusty Tape
- Old Radio
- Wobbly Memory
- Clean Analog

### User Presets
- `SAVE` button — save current settings under any name
- `DEL` button — delete the current user preset
- `< >` buttons — navigate presets sequentially
- `*` next to name — indicates unsaved changes

### Preset Storage Location
- **Windows**: `%APPDATA%\LoFiLabs\Lo-Fi Tape Simulator\Presets\`
- **macOS**: `~/Library/Application Support/LoFiLabs/Lo-Fi Tape Simulator/Presets/`
- **Linux**: `~/.config/LoFiLabs/Lo-Fi Tape Simulator/Presets/`

Presets are saved as plain XML — fully portable between machines.

### Native DAW Programs
The plugin exposes factory and user presets as **native programs** in your DAW, so you can switch presets from a hardware controller.

## 🚀 Building

GitHub Actions builds the VST3 automatically on push. See `.github/workflows/build.yml`.

## 📜 License
MIT
