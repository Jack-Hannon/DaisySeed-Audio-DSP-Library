# DaisySeed Audio DSP Library

A collection of DSP algorithms developed for my senior design project. They were built for real-time audio processing on a Daisy Seed module inside a programmable audio amplifier.

## Algorithms

### State Variable Filter
A 2nd-order multimode filter with separate low-pass, band-pass, and high-pass outputs.

**Controls:** cutoff frequency, resonance

### Delay
An echo effect built on a fractional delay line with Hermite interpolation. Feedback saturation and frequency-selective damping added to model analog delay characteristics.

**Controls:** delay length, feedback gain, mix

### Phaser / Flanger
Time-varying filter effects built from modulated delay lines and all-pass filters.

**Controls:** modulation speed, modulation depth, feedback gain

### Reverb
A stereo reverb based on the Gerzon architecture.

**Controls:** room size, feedback gain, mix

### Frequency Shifter / Ring Modulator
A modulation effect that blends between single-sideband modulation (frequency shifting) and double-sideband modulation (ring modulation).

**Controls:** sideband weights, frequency shift (+/-)

## Getting Started

This repository contains only my algorithms, not the full Daisy development environment. To build and run them, you will need Electro-Smith's official examples repository, which includes the required libraries ([libDaisy](https://github.com/electro-smith/libDaisy) and [DaisySP](https://github.com/electro-smith/DaisySP)) and the build setup:

**[electro-smith/DaisyExamples](https://github.com/electro-smith/DaisyExamples)**

1. Clone DaisyExamples and follow the setup instructions in its README.
2. Copy the algorithm files from this repository into a Daisy Seed project.
3. Build and flash the project to your Daisy Seed.

## Acknowledgements

Built on the [Electro-Smith](https://electro-smith.com/) Daisy platform.
