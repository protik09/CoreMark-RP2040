"""Shrinks an Arm GCC install before it is cached.

Arm GCC ships prebuilt C/C++ libraries ("multilibs") for 20+ Arm CPU
variants, about half of the toolchain's size. The CoreMark builds only use the
RP2040's and RP2350's. This asks GCC which multilibs the Pico SDK's compiler
flags select and deletes the others, so the CI cache is smaller and restores
faster. The compiler and the libraries it uses are unchanged, so the firmware
is byte-for-byte the same.

Usage: python prune_arm_multilibs.py <Arm GCC install directory>
"""
import os
import shutil
import subprocess
import sys

# The Pico SDK's CPU flags (the CoreMark report shows them as "Compiler flags").
# If an SDK update changes them and the link step can't find libc/libgcc, update these.
SDK_CPU_FLAGS = [
    ["-mcpu=cortex-m0plus", "-mthumb", "-mfloat-abi=soft"],  # RP2040
    ["-mcpu=cortex-m33", "-mthumb", "-march=armv8-m.main+fp+dsp", "-mfloat-abi=softfp"],  # RP2350
]


def main():
    root = sys.argv[1]
    gcc = os.path.join(root, "bin", "arm-none-eabi-gcc")

    def gcc_output(*args):
        return subprocess.run([gcc, *args], check=True, capture_output=True, text=True).stdout.strip()

    keep = {gcc_output(*flags, "-print-multi-directory") for flags in SDK_CPU_FLAGS}
    multilibs = [line.split(";")[0] for line in gcc_output("-print-multi-lib").splitlines()]
    version = gcc_output("-dumpversion")
    # Each multilib has a directory under each of these.
    bases = [
        os.path.join(root, "lib", "gcc", "arm-none-eabi", version),
        os.path.join(root, "arm-none-eabi", "lib"),
        os.path.join(root, "arm-none-eabi", "include", "c++", version, "arm-none-eabi"),
    ]

    removed = 0
    for multilib in multilibs:
        # "." is the default multilib at the top of each base directory.
        if multilib == "." or multilib in keep:
            continue
        # Never delete a directory that holds a multilib we keep.
        if any(k.startswith(multilib + "/") for k in keep):
            continue
        for base in bases:
            path = os.path.join(base, *multilib.split("/"))
            if os.path.isdir(path):
                shutil.rmtree(path)
                removed += 1

    print(f"Kept multilibs: {', '.join(sorted(keep))}. Removed {removed} directories.")


if __name__ == "__main__":
    main()
