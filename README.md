# Audio Processing Engine Cxx (APECxx)

![Language](https://img.shields.io/badge/language-C%2B%2B23-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![Status](https://img.shields.io/badge/status-Phase%202%20Active-orange.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)

## 🎵 Overview

**APECxx** is a modular audio processing engine built from scratch in modern C++. Designed with a focus on **Zero-Cost Abstractions** and **Memory Safety**, this engine provides a foundation for real-time digital signal processing (DSP), effect chaining, and audio manipulation.

Currently transitioning from **Phase 1 (Foundation & I/O)** to **Phase 2 (DSP Effects)**, the engine allows for robust WAV file handling, strict resource management via RAII, and a templated buffer system.

---

## 🏗️ Project Structure

The project follows a separation-of-concerns architecture, keeping interface headers distinct from implementation logic.

```text
├───include
│   ├───Effects       # Abstract base classes and Effect headers
│   └───WavIO         # WAV parsing and writer headers
├───src
│   ├───Effects       # Implementation of Gain, Fade, Mix
│   └───WavIO         # Implementation of RIFF parsing/writing
├───tests             # Google Test unit test suite
└───wav-file          # Sample assets for testing
```

## 🎧 Audio Showcase : Before & After

The Input and output files after each phase and effects

| Experiement | Description | Input Audio | Output Audio |
|:-----------:|:-----------:|:-----------:|:------------:|
| Pipeline Testing | Loss less Read/Write | ▶️[Input.wav](./wav-files/sample-1.0.wav) | ▶️[Output.wav](./wav-files/output.wav) |
| Pipeline Testing | Loss less Read/Write | ▶️[Input.wav](./wav-files/sample-1.0.wav) | ▶️[Output.wav](./wav-files/output.wav) |
| Pipeline Testing | Loss less Read/Write | ▶️[Input.wav](./wav-files/sample-1.0.wav) | ▶️[Output.wav](./wav-files/output.wav) |
