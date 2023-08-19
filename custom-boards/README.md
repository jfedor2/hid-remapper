# Custom HID Remapper boards

This document describes how to have custom HID Remapper boards manufactured using the provided design files at a service like JLCPCB. If you'd rather make the device using a Raspberry Pi Pico, see [here](../HARDWARE.md) for details on that option.

There are two versions, imaginatively named v1 and v2. Both of them are equivalent to the dual Pico variant when it comes to functionality and device compatibility.

![Custom HID Remapper boards](../images/custom-boards.jpg)

The main advantage of the v1 version is that it can be made with no soldering, you just order the board, flash the firmware, and have a working device. Files for an optional 3D printable enclosure are also available (it uses M2x8 flat head screws).

The v2 version doesn't have USB connectors and is meant to be built into existing devices like trackballs by soldering the appropriate wires directly to the board.

## How to order the boards

The process of ordering the boards is the same for both versions. You go to JLCPCB's [website](https://jlcpcb.com/), upload the Gerber zip file, enable the "PCB Assembly" option and upload the BOM and CPL files. All the options can be left at their default values. Then click through the rest of the process, making sure that all the components are in stock.

If you want to use some other service than JLCPCB, you will likely need to adapt at least the BOM and CPL files to match their requirements.

## How to flash the firmware

There are two RP2040 chips on each board and each chip has to be flashed with appropriate firmware. The procedure to do it goes as follows:

1. Flash A side (the one that connects to your computer) with [remapper\_board.uf2](https://github.com/jfedor2/hid-remapper/releases/latest/download/remapper_board.uf2) file by putting the board in firmware flashing mode and copying the file to the "RPI-RP2" drive that should appear on your computer.
2. Flash B side by going to the [web config tool](https://www.jfedor.org/hid-remapper-config/), opening the device and pressing the "Flash B side" button. When you do this, the A side flashes the B side with the appropriate firmware (there's no separate UF2 file).

To put the v1 board in firmware flashing mode, hold the BOOT\_A button while connecting it to a computer. This works the same as the BOOTSEL button on a Raspberry Pi Pico. The v2 board doesn't have a button, but it has the equivalent pin broken out. Short the BOOT pin on the A side to GND while connecting the board to a computer to put it in firmware flashing mode. Then copy the UF2 file to the drive that appears.

If the board is already flashed with some version of the HID Remapper firmware then you can also put it in firmware flashing mode by going to the web config tool and pressing the "Flash firmware" button. This is useful when you want to upgrade the firmware on a device that is not easily accessible.

When the board is fresh from the factory (has never been flashed), it should enter firmware flashing mode automatically when you connect it to a computer with no additional steps necessary.

In the future, I'd like to remove the need for step 2 above, it should be possible to make it happen automatically.

v1 PCB design uses [Type-C.pretty](https://github.com/ai03-2725/Type-C.pretty) library by [ai03-2725](https://github.com/ai03-2725).
