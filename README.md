# AnalogFx

**AnalogFx** is a high-fidelity, analog-modeled channel strip plugin designed for professional mixing and mastering. It combines the warmth of classic hardware with the precision of modern digital processing.

## 🚀 Features

*   **Preamps**: Modeled Telefunken, Neve, and Modern saturation stages.
*   **Compressors**: High-quality emulations of NC76 (1176-style), LA-2A, Fairchild, and a Modern VCA.
*   **EQs**: Dirt EQ, Neve 1073, Pultec EQP-1A, and Modern Surgical filters.
*   **Output Color**: Tape, British Iron, and Valve summing emulations.
*   **Safe Mode**: Industry-standard -18 dBFS calibration for perfect mixer headroom.
*   **Performance**: GPU-accelerated rendering and ultra-low CPU overhead.

## 📦 Download

You can find the latest builds for **Windows (VST3)**, **macOS (VST3/AU)**, and **Linux (VST3)** in the [Releases](https://github.com/danny1marshall1587-maker/AnalogFx/releases) section.

## 🛠 Build Requirements

*   **CMake** (3.15+)
*   **JUCE** (Integrated via FetchContent)
*   **C++17 Compliant Compiler**

### Building from Source

```bash
cmake -B build
cmake --build build --config Release
```

---
*Created with care by Danny Marshall*
