# MCU UHMI

This repository contains the embedded firmware and host-side communication stack for a screen/MCU control system used with router or gateway platforms. It supports two MCU families:

- Microchip PIC32MZ1024EFE100
- STMicroelectronics STM32F446VET100

The project includes:

- low-level MCU bootloader/application firmware images
- host-side protocol logic to communicate with the MCU
- firmware update and app/boot switching flows
- screen-related UI and data handling logic

## Repository layout

```text
Docs/                     Design documentation for the screen and MCU behavior
PIC32_Bootloaders/        PIC32 bootloader related files
mcu/                      MCU firmware sources and generated image bundles
  app_hex/                application hex files
  boot_hex/               bootloader hex files
  intact_hex/             combined bootloader + application images
  mz1024efe100/           Microchip PIC32 firmware project
  stm32f446vet/           STM32F446 firmware project
uhmi/                     Host-side UHMI protocol and control application
  Makefile                Build/install targets
  uhmi_main.c             Main loop, packet handling, mode switching
  uhmi_common.h           Common protocol enums, packet structures, CRC support
  uhmi_bootloader.c       Bootloader communication and update logic
  uhmi_application.c      Application-mode protocol handling and screen events
  uhmi_data_crawler.c     Data acquisition/processing helpers
```

## Core idea

The `uhmi` directory implements a host-side controller that exchanges framed packets with the MCU via a serial interface. The protocol uses a simple start-of-header/end-of-transmission frame with CRC protection.

The MCU can operate in two states:

- `MCU_APP_MODE`: normal application mode
- `MCU_BOOT_MODE`: bootloader mode for firmware update/maintenance

The host software can switch between these modes, prepare packets for transmission, and handle events like update, reboot, or status checks.

## Firmware versioning

The repository stores firmware images with names like:

- `app.1.2.108.hex`
- `boot.1.2.hex`
- `factory.1.2.108.hex`

The versioning scheme is documented in `mcu/README_NOTE`, which explains the MCU family and oscillator configuration by major/minor values.

## Build and usage

The host-side tool is built from the `uhmi` directory:

```bash
cd uhmi
make
```

The `Makefile` also includes installation steps for copying the UHMI binary and firmware bundle into a target runtime layout.

## Notes on the firmware projects

The MCU project tree contains generated and vendor-style project code:

- Microchip firmware projects under `mcu/mz1024efe100/...`
- STM32 firmware projects under `mcu/stm32f446vet/...`
- bootloader/application code in ST project folders and Microchip Harmony-based structures

The repo is primarily an embedded firmware package rather than a general-purpose application, so many files are device-specific and generated from vendor tooling.

## Documentation

For deeper hardware and firmware details, refer to the files under `Docs/` and the notes in:

- `mcu/README`
- `mcu/README_NOTE`

## License

No explicit repository license file is present in the root of this project at the moment. Please check the upstream repository for licensing details before redistribution or commercial use.
