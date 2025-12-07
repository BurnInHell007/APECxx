# Welcome to APECxx (Audio Processing Engine Cxx)

## 🎵 The "Why" Behind the Noise
Welcome! **APECxx** is my journey into the deep end of audio programming.

I started this as a pet project with a simple but challenging motive: **to stop using audio libraries as "black boxes" and actually understand what happens under the hood.** I wanted to build a production-ready audio engine from scratch, moving from simple file handling to complex, real-time signal processing.

This isn't just about making sound; it's about mastering Modern C++, understanding memory constraints, and taming the chaos of real-time threads.

---

## 🎯 The Goal
The end game is to build a fully functional, cross-platform audio engine that can:
* **Read & Write Audio:** Parse WAV files manually (no shortcuts!).
* **Process Effects:** Apply gain, fades, EQ, and reverb in real-time.
* **Visualize Sound:** A GUI with waveform viewers and spectrum analyzers.

---

## 🛠️ Under the Hood: The Tech Stack
This project is a playground for advanced C++ concepts. I am strictly avoiding "C-style" legacy code in favor of modern safety and performance patterns.

### Core Language Features
* **Smart Pointers (`unique_ptr`, `shared_ptr`):** For strict ownership management and automatic resource cleanup (RAII).
* **Templates & Metaprogramming:** Creating generic audio buffers and effect processors that are type-safe and fast.
* **Concurrency:** Lock-free ring buffers and atomic operations to ensure the audio thread never stutters.
* **Move Semantics:** Efficiently passing heavy audio buffers around without unnecessary copying.

### Libraries & Tools
* **PortAudio / RtAudio:** For hardware communication.
* **JUCE / Dear ImGui:** For the eventual GUI.
* **Google Test:** ensuring the math is actually correct.

---

## 🗺️ The Roadmap
I am breaking this massive undertaking into 8 distinct phases. Here is where the project stands:

1.  **Phase 1: Foundation** — Building the basic `AudioBuffer` and WAV file parser.
2.  **Phase 2: Basic Effects** — Implementing Gain, Fades, and Mixing logic.
3.  **Phase 3: DSP & Filters** — Creating Biquad filters and multi-band Equalizers.
4.  **Phase 4: Real-Time** — Hooking into the microphone and speakers with lock-free buffers.
5.  **Phase 5: Architecture** — Building a plugin system with JSON configuration.
6.  **Phase 6: Advanced DSP** — Reverb, Delay, and Compression algorithms.
7.  **Phase 7: The GUI** — Visualizing the audio with FFTs and real-time waveforms.
8.  **Phase 8: Polish** — Optimization (SIMD), Benchmarking, and Documentation.

---

## Installation and Usage
Follow these instruction to sucessfully replicate the results.

Run `cmake` to generate build system:
```bash
mkdir build
cd build
cmake ..
```

Build project by issuing:
```bash
cmake --build .
```

Run all tests by issuing:
```bash
ctest
```

In the build folder now you can access `APECxx_Run.exe`, use it as follow:
```bash
.\APECxx_Run.exe <input_path.wav> <output_path.wav>
```

For powerShell
```powershell
# Windows (MinGW)
# Clean build
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path build
Set-Location build

# Configure
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build
mingw32-make -j4

# Run main program
.\bin\audio_tool.exe

# Run tests
.\bin\audio_tests.exe

# Or use CTest
ctest --output-on-failure
```

---

## 🤝 Connect
This project is open for anyone curious about DSP or C++. If you have suggestions, spot a memory leak, or just want to chat about audio programming or your interest in C++, feel free to reach out!

**Developed by [Vijay Kumar B]**

[LinkedIn](https://linkedin.com/in/vijay-kumar-b-vkb007)

[X.com](https://x.com/burninhell_oo7)
