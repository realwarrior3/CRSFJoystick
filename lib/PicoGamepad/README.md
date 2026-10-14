# Raspberry Pi Pico Gamepad/Joystick Library

**Version 1.0.1**

This library works with the Arduino IDE, it is designed to work with the Raspberry Pi Pico, it may work on other RP2040 boards but hasn't been tested.

NOTE: This library works with the Arduino MBED OS RP2040 core for the Raspberry Pi Pico.
      It doesn't work with the Earle Philhower core. If you're getting an error about "PluggableUSBHID.h", that's why.

> **Note (this fork):** This copy is synced with upstream PicoGamepad (2025-04-28,
> 16 axes) with local bug fixes: hat byte offsets corrected to 48/49, the `Vx`
> HID usage fixed to `0x40`, `SetButton()` bounds fixed (`>= 128`), `inputArray`
> zeroed in the constructors, and the invalid "System Undefined" axis removed.

This library was created for use with the RealRobots [rr_configurator](https://www.gitlab.com/realrobots/rr_configurator) GUI based game controller customization software but is being made available freely for use and modification by others.

[Patreon](https://www.patreon.com/realrobots)

### API

- `send_update()   - Send HID report, values won't be sent until this is called.`

- `SetAxis(int idx, int val)   - Set the value of axis 0-15 with 16 bit precision  -32767 to 32767.`

- `SetButton(int idx, bool state) - Sets the state of any of the buttons, indexed 0-127`

- `SetHat(int idx, int dir) - Set the direction of hats indexed 0-3.` 

  -  `direction is clockwise 0=N 1=NE 2=E 3=SE 4=S 5=SW 6=W 7=NW 8=CENTER`

- `SetX(int val)  - Set the value of the X axis with 16 bit precision	 -32767 to 32767`

- `SetY(int val)  - Set the value of the Y axis with 16 bit precision	 -32767 to 32767`

- `SetZ(int val)  - Set the value of the Z axis with 16 bit precision	 -32767 to 32767`

- `SetRx(int val) - Set the value of the Rx axis with 16 bit precision	 -32767 to 32767`

- `SetRy(int val) - Set the value of the Ry axis with 16 bit precision	 -32767 to 32767`

- `SetRz(int val) - Set the value of the Rz axis with 16 bit precision	 -32767 to 32767`

- `SetSlider(int val) - Set the value of the Slider axis with 16 bit precision	 -32767 to 32767`

- `SetDial(int val) - Set the value of the Dial axis with 16 bit precision	 -32767 to 32767`

- `SetWheel(int val) - Set the value of the Wheel axis with 16 bit precision	 -32767 to 32767`

- `SetVx(int val) - Set the value of the Vx axis with 16 bit precision	 -32767 to 32767`

- `SetVy(int val) - Set the value of the Vy axis with 16 bit precision	 -32767 to 32767`

- `SetVz(int val) - Set the value of the Vz axis with 16 bit precision	 -32767 to 32767`

- `SetVbrx(int val) - Set the value of the Vbrx axis with 16 bit precision	 -32767 to 32767`

- `SetVbry(int val) - Set the value of the Vbry axis with 16 bit precision	 -32767 to 32767`

- `SetVbrz(int val) - Set the value of the Vbrz axis with 16 bit precision	 -32767 to 32767`

- `SetVno(int val) - Set the value of the Vno axis with 16 bit precision	 -32767 to 32767`

[Video instructions](https://www.youtube.com/watch?v=CNF7G-1LniI)

### Example

```c++
#include <PicoGamepad.h>

PicoGamepad gamepad;

// 16 bit integer for holding input values
int val;

void setup() {  
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  // X Potentiometer on pin 26
  pinMode(26, INPUT);
  // Y Potentiometer on pin 27
  pinMode(27, INPUT);

  // Button on pin 
  pinMode(28, INPUT_PULLUP);
}

void loop() {

  // Get input value from Pico analog pin
  val = analogRead(26);

  // Map analog 0-1023 value from pin to max HID range -32767 - 32767
  val = map(val, 0, 1023, -32767, 32767);

  // Send value to HID object
  gamepad.SetX(val);

  // Repeat with Y pin
  val = analogRead(27);
  val = map(val, 0, 1023, -32767, 32767);
  gamepad.SetY(val);

  

  // Other Axis Options
  /* BY INDEX 0-15 */
  // gamepad.SetAxis(0, val);

  /* BY NAME */
  // gamepad.SetZ(val);
  // gamepad.SetRx(val);
  // gamepad.SetRy(val);
  // gamepad.SetRz(val);
  // gamepad.SetSlider(val);
  // gamepad.SetDial(val);
  // gamepad.SetWheel(val);
  // gamepad.SetVx(val);
  // gamepad.SetVy(val);
  // gamepad.SetVz(val);
  // gamepad.SetVbrx(val);
  // gamepad.SetVbry(val);
  // gamepad.SetVbrz(val);
  // gamepad.SetVno(val);

  // Set button 0 of 128 by reading button on digital pin 28
  gamepad.SetButton(0, !digitalRead(28));

  // Set hat direction, 4 hats available. direction is clockwise 0=N 1=NE 2=E 3=SE 4=S 5=SW 6=W 7=NW 8=CENTER
  // gamepad.SetHat(0, 8);


  // Send all inputs via HID
  // Nothing is send to your computer until this is called.
  gamepad.send_update();

  // Flash the LED just for fun
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(100);
}

```

