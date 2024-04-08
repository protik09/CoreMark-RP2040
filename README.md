# CoreMark RP2040❤️

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
<!-- [![HitCount](http://hits.dwyl.com/protik09/CoreMark-RP2040.svg)](http://hits.dwyl.com/protik09/CoreMark-RP2040) -->

![Coremark_RP2040_logo](https://github.com/protik09/CoreMark-RP2040/blob/main/Coremark-RP2040.png?raw=true)

This is the [COREMARK®](https://github.com/eembc/coremark) embedded benchmark ported for the [RP2040](https://github.com/raspberrypi/pico-sdk)
chip from the folks over at Raspberry Pi.

## How to Use

Ensure that the **Pico C/C++ SDK** environment variables are set properly and you have cmake installed.
For Linux check [this link](https://github.com/raspberrypi/pico-sdk) and for Windows check
[this link](https://github.com/ndabas/pico-setup-windows)

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

### For Linux
![Shell Script](https://img.shields.io/badge/shell_script-%23121011.svg?style=for-the-badge&logo=gnu-bash&logoColor=white)

```shell
cmake .
make clean
make -j4
```

* Copy the **Coremark-RP2040.uf2** file to the RP2040 board.
* Open a Serial Monitor at 115200 baud (or just maximum USB speed)
* **IMPORTANT**: Press any key to start single core benchmark.

## TODO

* Add multi-core support to the benchmark.
