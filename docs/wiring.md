# Receiver wiring

How to connect a CRSF receiver (ELRS, TBS Crossfire, TBS Tracer) to each
supported RP2040 board.

## Common notes

* **CRSF is a full-duplex UART.** The wires are crossed:
  **MCU TX → receiver RX** and **MCU RX → receiver TX**. Do not swap them.
* **Power:** connect **5V** and **GND**. ExpressLRS receivers typically accept
  3.3–5 V; 5 V (USB VBUS) is the usual choice. The 3V3 pin also works if your
  receiver is 3.3 V only.
* **Logic level:** RP2040 GPIO is 3.3 V and ELRS/CRSF receivers are 3.3 V logic,
  so the signal lines connect directly — no level shifter is required.
* Only three wires are needed for CRSF: 5V, GND and one signal line in each
  direction. If your receiver has a single combined wire, check its documentation
  (that is usually a different protocol, not CRSF).
* The firmware always uses **115200 baud** on the CRSF UART
  (`CRSF_BAUDRATE`, see `lib/CrsfSerial/crsf_protocol.h`).

Legend: **MCU TX** = board transmit pin, **MCU RX** = board receive pin.

---

## Raspberry Pi Pico — `pico` (BOARD_ID 1)

| Signal | GPIO  | Physical pin | Receiver pin |
| ------ | ----- | ------------ | ------------ |
| MCU TX | GP4   | 6            | RX           |
| MCU RX | GP5   | 7            | TX           |
| 5V     | VBUS  | 40           | 5V           |
| GND    | GND   | 38           | GND          |

Onboard LED: `GP25` (single).

## Pimoroni Tiny 2040 — `tiny2040` (BOARD_ID 2)

| Signal | GPIO | Pad  | Receiver pin |
| ------ | ---- | ---- | ------------ |
| MCU TX | GP4  | 12   | RX           |
| MCU RX | GP5  | 11   | TX           |
| 5V     | VBUS | 1    | 5V           |
| GND    | GND  | 2    | GND          |

Onboard LED: RGB on `GP18`/`GP19`/`GP20` (active low).

## Waveshare RP2040-Zero — `waveshare2040zero` (BOARD_ID 3)

Pads are numbered anticlockwise starting at the USB connector.

| Signal | GPIO | Pad  | Receiver pin |
| ------ | ---- | ---- | ------------ |
| MCU TX | GP4  | 19   | RX           |
| MCU RX | GP5  | 18   | TX           |
| 5V     | 5V   | 1    | 5V           |
| GND    | GND  | 2    | GND          |

Onboard LED: WS2812 (NeoPixel) on `GP16`.

## Seeed XIAO RP2040 — `seeedxaio2040` (BOARD_ID 4)

| Signal | GPIO | XIAO pin | Receiver pin |
| ------ | ---- | -------- | ------------ |
| MCU TX | GP0  | D6 / TX  | RX           |
| MCU RX | GP1  | D7 / RX  | TX           |
| 5V     | 5V   | 5V       | 5V           |
| GND    | GND  | GND      | GND          |

Onboard LED: WS2812 (NeoPixel) on `GP12`, powered via `GP11`.

## Adafruit QT Py RP2040 — `qtpy2040` (BOARD_ID 5)

| Signal | GPIO | Pad / silk | Receiver pin |
| ------ | ---- | ---------- | ------------ |
| MCU TX | GP20 | 7 / TX     | RX           |
| MCU RX | GP5  | 8 / RX     | TX           |
| 5V     | 5V   | 14         | 5V           |
| GND    | GND  | 13         | GND          |

Onboard LED: WS2812 (NeoPixel) on `GP12`, powered via `GP11`.

---

## Adding a new board

1. Add an `[env:...]` section to `platformio.ini` with
   `build_flags = -DBOARD_ID=<n>`.
2. Add a matching block in `include/board_defs.h` defining `CRSF_TX`, `CRSF_RX`
   and the `led_on()` / `led_off()` / `boardSetup()` functions.
3. Document the wiring here.
