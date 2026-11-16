# Teensy 4.1 C

Minimal bare-metal C project for the Teensy 4.1 using CMake and Ninja. It
does not depend on the Arduino or Teensyduino core.

## Requirements

- CMake 3.20 or newer
- Ninja
- The complete Arm GNU Toolchain for Embedded (`arm-none-eabi-gcc`, newlib,
  `arm-none-eabi-objcopy`, `arm-none-eabi-objdump`, and `arm-none-eabi-size`)
- `clangd` for editor language support
- `teensy_loader_cli` for the optional `flash` target

## Build

On macOS, install the complete Arm toolchain archive in a user-owned
directory:

```sh
mkdir -p "$HOME/.local/toolchains"
curl -L 'https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-darwin-arm64-arm-none-eabi.tar.xz' \
  | tar -xJ -C "$HOME/.local/toolchains"
```

Do not use the Homebrew formula named `arm-none-eabi-gcc`: it is compiler-only,
is built with `--without-headers`, and does not include newlib. The CMake
toolchain file automatically detects the archive under
`$HOME/.local/toolchains`.

```sh
./scripts/configure
./scripts/compile
```

Configuration also generates `build/compile_commands.json`. The `.clangd`
file points clangd at that database and selects the ARM target, so run the
CMake configure command once before opening the project in an editor.

## Format and lint

Install LLVM to use the project formatting and linting scripts:

```sh
./scripts/format
./scripts/format --check
./scripts/lint
```

`./scripts/format` formats the project C sources and headers in place.

## Serial monitor

Run `./scripts/monitor` to print USB serial output from the Teensy. It finds
the `/dev/cu.usbmodem*` device automatically and reconnects after uploads or
board resets. Stop it with `Ctrl-C`.
`--check` verifies formatting without changing files. Linting uses the ARM
compile database, so run `./scripts/configure` first after changing CMake
configuration.

If the complete ARM toolchain is in another location, set its executable
prefix during configuration:

```sh
cmake --fresh --preset teensy41 \
  -DARM_GCC_PREFIX=/path/to/arm-gnu-toolchain/bin/arm-none-eabi-
```

The default build produces `build/teensy41`, `build/teensy41.s`, and
`build/teensy41.hex`. The assembly dump is refreshed automatically after a
successful link.

The `bin` target produces a raw binary:

```sh
cmake --build build --target bin
```

With `teensy_loader_cli` installed, press the Teensy program button and run:

```sh
cmake --build build --target flash
```

## UART firmware updates

Build a raw firmware image and send it from a Raspberry Pi 5 connected to
Teensy UART1 (115200 baud, 8N1):

```sh
cmake --build build --target bin
scripts/upload-uart --port /dev/ttyAMA0 build/teensy41.bin
```

The current `src/main.c` demo enters update mode when it receives `0x55` on
UART1. The SDK updater then claims RX callback ownership, stages and checks the
image, activates it, and reboots. The application must stop its own periodic
transmissions on the selected UART before calling `fw_update_uart_run()`.
Updates require enough unused program flash to stage the incoming image; the
updater reports the available staging capacity before accepting it. Keep USB
available for recovery if power is interrupted during activation.
The image travels as raw 4 KiB UART chunks with per-chunk CRC checks and ACKs;
the device verifies the complete staged image before activation. The uploader's
`--baud` value must match the baud rate compiled into the application.

The application entry point is `src/main.c`. The platform implementation is
under `sdk/`, with public headers in `sdk/include/teensy/`.

## Using the SDK from another CMake project

The ARM toolchain file makes CMake use `arm-none-eabi-gcc` for compilation and
linking; it does not change the normal CMake target model. `add_executable`
still creates an executable and `add_library(... STATIC ...)` still creates a
static archive. The SDK's `teensy_sdk` target is already a static library.
The `teensy41-sdk` interface target packages the SDK library and the Teensy
compile/link settings for final firmware targets.

When this project is added with `add_subdirectory`, a static library can use
normal CMake target commands:

```cmake
add_library(my_teensy_code STATIC my_teensy_code.c)
target_link_libraries(my_teensy_code PRIVATE teensy41-sdk)
```

The final firmware must still be an executable so the Teensy startup code,
linker script, and memory layout can be applied:

```cmake
add_executable(my_firmware main.c)
target_link_libraries(my_firmware PRIVATE my_teensy_code teensy41-sdk)
```

The older `teensy_target_options` helper remains available. A static library
is an archive of object files, so it cannot produce a `.hex` file or replace
the final firmware executable.

## C library

The SDK links the ARM toolchain's vendored `newlib-nano` through the
`teensy41-sdk` target. This provides standard `malloc`, `free`, and other C
library functions. The SDK supplies `_sbrk()` using the linker-defined RAM
heap and routes `_read()`/`_write()` through USB serial. The existing SDK
`printf` backend remains enabled so formatted output keeps using the SDK's USB
serial behavior.

The SDK currently provides GPIO, GPIO interrupts, PWM, ADC, clocks, timing,
UART, I2C, USB CDC, a WDOG3 (RTWDOG) watchdog, and a compact `printf`
implementation. Include `<teensy/watchdog.h>` to start, refresh, inspect, or
disable the watchdog. `printf` output is sent only to USB CDC after the host
has enumerated the device.
