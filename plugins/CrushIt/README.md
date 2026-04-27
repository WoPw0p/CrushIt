# Crush It — Signal Destruction Engine

A multi-module audio destruction plugin built with **JUCE 8** and **WebView UI**.

Crush It is not a distortion plugin. It's a signal destruction engine that weaponizes digital failure — temporal tearing, bit-depth collapse, sample-rate decay, and floating-point error injection.

## Modules (6)

| # | Module | Description |
|:---|:---|:---|
| 1 | **TIME WARP** | Circular buffer with transient/pitch-triggered micro-looping. Read head jumps back on detection, creating machine-stutter buzz. |
| 2 | **BIT CRUSH** | Classic bit-depth reduction (1–32 bits). Quantization staircase distortion. |
| 3 | **SAMPLE RATE** | Sample rate decimation (1–256x divisor). ZOH or linear interpolation. Aliasing artifacts. |
| 4 | **CLIP** | Waveform clipper with Hard / Soft (tanh) / Fold modes. Pre-gain drive stage. |
| 5 | **DEBRIS** | Digital noise generator: White noise, Burst, Crackle. Sync-to-host or free-running trigger with decay envelope. |
| 6 | **BIT FLIP** | IEEE 754 floating-point error injection. Invert specific bits, random bit patterns, or multiply-and-clip. Probability-gated per sample. |

### Signal Chain

```
Input → Time Warp → Bit Crush → Sample Rate → Clip → Debris → Bit Flip → Output
```

## Features

- **35 parameters** across 6 independent modules
- **Parameter randomization** — auto-randomize parameters per module, beat-synced rate control
- **Live visualization** — each module has a unique animated Canvas visualization
- **Real-time metering** — input/output peak level meters
- **Module enable/disable** — individual bypass per module with wet/dry mix

## UI

- WebView-based interface (HTML5 Canvas)
- Sci-fi / industrial tech aesthetic with scanlines and noise texture
- 800×500 fixed window
- Per-module control cards with mini knobs and dropdowns
- Signal chain flow visualization

## Building

### Prerequisites
- JUCE 8 SDK (place in `_tools/JUCE/`)
- CMake 3.22+
- macOS: Xcode CLI tools
- Windows: Visual Studio 2022
- Linux: GCC/Clang + GTK3 + WebKit2GTK

### Build

```bash
# macOS
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_OSX_ARCHITECTURES="arm64"
cmake --build build --target CrushIt_VST3
cmake --build build --target CrushIt_Standalone

# Windows (PowerShell)
.\scripts\build-and-install.ps1 -PluginName CrushIt
```

### Install

```bash
# macOS VST3
cp -R build/plugins/CrushIt/CrushIt_artefacts/VST3/"Crush It.vst3" ~/Library/Audio/Plug-Ins/VST3/

# macOS AU
cp -R build/plugins/CrushIt/CrushIt_artefacts/AU/"Crush It.component" ~/Library/Audio/Plug-Ins/Components/
```

## Tech Stack

- **DSP:** C++20, JUCE 8 (juce_dsp, juce_audio_processors)
- **UI:** HTML5 Canvas, CSS3, JavaScript (WebView native bridge)
- **Build:** CMake, Xcode / MSVC / GCC
- **Formats:** VST3, AU, Standalone

## License

MIT
