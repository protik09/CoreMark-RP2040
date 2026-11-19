# CoreMark RP2040❤️

![Build Status](https://github.com/protik09/CoreMark-RP2040/actions/workflows/generate-release.yml/badge.svg) ![GitHub release (latest by date)](https://img.shields.io/github/v/release/protik09/CoreMark-RP2040) [![GitHub stars](https://img.shields.io/github/stars/protik09/CoreMark-RP2040.svg)](https://github.com/username/repository/stargazers)
<!-- [![HitCount](http://hits.dwyl.com/protik09/CoreMark-RP2040.svg)](http://hits.dwyl.com/protik09/CoreMark-RP2040) -->

![Coremark_RP2040_logo](https://github.com/protik09/CoreMark-RP2040/blob/main/Coremark-RP2040.png?raw=true)

This is the [COREMARK®](https://github.com/eembc/coremark) embedded benchmark ported for the [RP2040](https://github.com/raspberrypi/pico-sdk)
chip from the folks over at Raspberry Pi.

## Quick Start (No Build Required)

You don't need the SDK or a compiler to run the benchmark. All you need is to copy the UF2 file onto the Pico:

1. Download **Coremark-RP2040.uf2** from the [latest release](https://github.com/protik09/CoreMark-RP2040/releases/latest) ([direct download](https://github.com/protik09/CoreMark-RP2040/releases/latest/download/Coremark-RP2040.uf2)).
2. Hold down the **BOOTSEL** button on the Pico while you plug it into your computer over USB, then let go. The Pico shows up as a USB drive called **RPI-RP2**.
3. Drag **Coremark-RP2040.uf2** onto the **RPI-RP2** drive. The Pico restarts by itself and runs the benchmark firmware.
4. Open a serial monitor on the Pico's USB serial port and press any key to start.

You only need the build steps below if you want to change the code or build the firmware yourself.

## How to Build

Ensure that the **Pico C/C++ SDK** environment variables are set properly and you have cmake installed.

For Linux check here:  [![Static Badge](https://img.shields.io/badge/-Linux-c31c4a?style=flat&logo=raspberrypi)](https://github.com/raspberrypi/pico-sdk
)

For Windows, the setup files can be found here:  [![Static Badge](https://img.shields.io/badge/-Windows-blue?logo=windows11)](https://github.com/raspberrypi/pico-setup-windows/releases/latest/download/pico-setup-windows-x64-standalone.exe
)

### For Microsoft Windows

![Windows Terminal](https://img.shields.io/badge/Windows%20Terminal-%234D4D4D.svg?style=for-the-badge&logo=windows-terminal&logoColor=white)

Add the **PICO_INSTALL_PATH** to User Environment Variables. 

For example, if you have installed the SDK in *"C:\Program Files\Raspberry Pi\Pico SDK v1.5.1"* then add **PICO_INSTALL_PATH** as *C:\Program Files\Raspberry Pi\Pico SDK v1.5.1* to User Environment Variables.

Then run the following commands in CMD in the project directory.

```cmd
scripts/setup_env.bat
```

You can use the following command to clean the project directory.

```cmd
make clean || ./scripts/clean.bat

```

You can use the following command to build the project.

```cmd
make all || cmake . && ninja build
```

The useful files will be in the **artifacts_to_upload** directory.

### For Linux

![Shell Script](https://img.shields.io/badge/shell_script-%23121011.svg?style=for-the-badge&logo=gnu-bash&logoColor=white)

```bash
make all
```

The useful files will be in the **artifacts_to_upload** directory.

Copy the **Coremark-RP2040.uf2** file to the RP2040 board.

Open a Serial Monitor at 115200 baud (or just maximum USB speed)

**IMPORTANT**: Press any key to start the benchmark.

## Benchmarking steps

CoreMark runs three times in a row, and each run prints a complete, standard CoreMark report:

1. **Core 0 only**: one context on core 0.
2. **Core 1 only**: one context on core 1.
3. **Core 0 + Core 1**: two contexts, one per core, running at the same time.

A short summary at the end lists the Iterations/Sec of each run and the dual-core speed-up. Check each run's
report for `Correct operation validated`.

### Things you should know about the results of the benchmark

- In the RP2040 results, core 1 alone will most likely have a slightly higher score than core 0 by itself, because core 0 also handles the USB serial port.
- The both-cores result will likely be below 2x, because both cores fetch code from the same flash cache - which can become a bit of a bottleneck.

## RELEASES

[![Static Badge](https://img.shields.io/badge/-LATEST_RELEASES-E1CFB3?style=flat&logo=githubactions)](https://github.com/protik09/CoreMark-RP2040/releases/latest)
