# CRSFJoystick

Firmware for **RP2040** boards that turns an **ExpressLRS / CRSF / Tracer**
receiver into a **USB HID gamepad** for simulators.

The board speaks the CRSF wire protocol to the receiver and exposes the incoming
channels as joystick axes and buttons over native USB. Named *CRSFJoystick* after
that protocol.

* 8 analog channels → X, Y, Rx, Ry, Z, Rz, Throttle, S0
* Up to 128 buttons (AUX switches and optional stick deflection)
* Failsafe: when the link drops, every axis is centered and every button
  released (re-sent every 100 ms)
* ExpressLRS is the primary target; TBS Crossfire and TBS Tracer also work

## Supported boards

Every board is a separate PlatformIO environment (see `platformio.ini`). The
`BOARD_ID` build flag selects the pin/LED map in `include/board_defs.h`.

| PlatformIO env    | Board                    | BOARD_ID | MCU TX | MCU RX |
| ----------------- | ------------------------ | -------- | ------ | ------ |
| `pico`            | Raspberry Pi Pico        | 1        | GP4    | GP5    |
| `tiny2040`        | Pimoroni Tiny 2040       | 2        | GP4    | GP5    |
| `waveshare2040zero` | Waveshare RP2040-Zero  | 3        | GP4    | GP5    |
| `seeedxaio2040`   | Seeed XIAO RP2040        | 4        | GP0    | GP1    |
| `qtpy2040`        | Adafruit QT Py RP2040    | 5        | GP20   | GP5    |

**Wiring diagrams for each board — how to connect the receiver — are in
[`docs/wiring.md`](docs/wiring.md).**

## Install VSCode + PlatformIO

1. Install Visual Studio Code: <https://code.visualstudio.com/>
2. In VSCode open the Extensions view (`Ctrl+Shift+X`), search for **PlatformIO
   IDE** and install it.
   (Full instructions: <https://platformio.org/install/ide?install=vscode>)
3. Restart VSCode and open this folder: `File → Open Folder…`

On the first build PlatformIO downloads the RP2040 toolchain automatically.

## Build and flash

The PlatformIO toolbar is at the bottom of the VSCode window.

1. **Select the board.** Click the environment name (initially
   `Default (CRSFJoystick)`) in the toolbar and choose your board
   (`pico`, `tiny2040`, `waveshare2040zero`, `seeedxaio2040`, `qtpy2040`).
2. **Build.** Click the **✓** (checkmark) button.
3. **Upload.** Connect the board over USB and click the **→** (arrow) button.

### Alternative upload (BOOTSEL / UF2)

This is the standard Raspberry Pi method and needs no tools:

1. Hold the **BOOTSEL** button while plugging the board in (or press
   BOOTSEL + RESET if already powered).
2. A mass-storage device (for example `RPI-RP2`) appears.
3. Drag `firmware.uf2` from `.pio/build/<env>/` onto that drive
   (replace `<env>` with your board's environment name).
4. The board reboots and enumerates as a gamepad.

### Drivers (Windows)

The RP2040 USB driver can be a hit or miss with PlatformIO. If upload cannot
find the board you may need Zadig:
<https://github.com/platformio/platform-raspberrypi/issues/2#issuecomment-828586398>

Driver mapping that is known to work on Windows 11:

| Mode        | Interface | Zadig driver          | Device Manager                       |
| ----------- | --------- | --------------------- | ------------------------------------ |
| Normal run  | 0         | HidUsb                | Human Interface Devices → HID-compliant game controller |
| Normal run  | 1         | usbser                | Ports (COM & LPT) → USB Serial Device |
| RP2 Boot    | 0         | USBSTOR               | Disk Drives → RPI RP2 USB Device      |
| RP2 Boot    | 1         | WinUSB                | Universal Serial Bus Devices → RP2 Boot |

To change a driver: right-click the device → Properties → Update Driver →
Browse my computer → Let me pick from a list of available drivers.

## After flashing

* Bind the receiver to your radio. The onboard LED shows link status
  (on = linked, blink = no link).
* Verify the device in a gamepad tester such as
  <https://hardwaretester.com/gamepad>.

## Calibration and mapping

Channel endpoints and the button map live in [`include/calibration.h`](include/calibration.h);
the comments there explain how to add or remove binds. Per-board pins and LED
handling are in [`include/board_defs.h`](include/board_defs.h).

## Project layout

```
src/main.cpp        CRSF → USB HID logic
include/            board_defs.h (pins/LED), calibration.h (axes/buttons)
lib/CrsfSerial/     CRSF protocol parser
lib/PicoGamepad/    USB HID gamepad
lib/crc8/           CRC8 used by CrsfSerial
docs/wiring.md      receiver wiring for each RP2040 board
platformio.ini      build environments (one per board)
```

## Flashing the receiver (optional)

The firmware includes a CRSF passthrough, so a receiver can be updated through
the joystick using the Betaflight passthrough method:
<https://www.expresslrs.org/software/updating/betaflight-passthrough/>

If your receiver is WiFi-enabled it can also be updated over WiFi:
<https://www.expresslrs.org/software/updating/wifi-updating/>

## Credits

* CRSF protocol: CapnBry — <https://github.com/CapnBry/CRServoF>
* USB gamepad: RealRobots — <https://gitlab.com/realrobots/PicoGamepad>

## License

GPLv3 — see [`LICENSE`](LICENSE).
Originally based on <https://github.com/mikeneiderhauser/CRSFJoystick>.
