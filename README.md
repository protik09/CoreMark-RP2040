
![Coremark_RP2040_logo](https://github.com/protik09/CoreMark-RP2040/blob/main/Coremark-RP2040.png)

# Summary

This is the [COREMARK®](https://github.com/eembc/coremark) embedded benchmark ported for the [RP2040](https://github.com/raspberrypi/pico-sdk)
chip from the folks over at Raspberry Pi.

# How to Use

Ensure that the **Pico C/C++ SDK** environment variables are set properly and you have cmake installed.
For Linux check [this link](https://github.com/raspberrypi/pico-sdk) and for Windows check
[this link](https://github.com/ndabas/pico-setup-windows)

## For Linux

```shell
cmake .
make clean
make -j4
```

* Copy the **Coremark-RP2040.uf2** file to the RP2040 board.
* Open a Serial Monitor at 115200 baud (or just maximum USB speed)
* **IMPORTANT**: Press any key to start single core benchmark.


# TODO

* Add multi-core support to the benchmark.
