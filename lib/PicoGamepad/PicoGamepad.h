/*
 * Copyright (c) 2018-2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PicoGamepad_H
#define PicoGamepad_H

#include "PluggableUSBHID.h"
#include "platform/Stream.h"
#include "PlatformMutex.h"

// values addresses
#define BTN0_7 0
#define BTN8_15 1
#define BTN16_23 2
#define BTN24_31 3
#define BTN32_39 4
#define BTN40_47 5
#define BTN48_55 6
#define BTN56_63 7
#define BTN64_71 8
#define BTN72_79 9
#define BTN80_87 10
#define BTN88_95 11
#define BTN96_103 12
#define BTN104_111 13
#define BTN112_119 14
#define BTN120_127 15


// STANDARD ANALOG CONTROLS

#define X_AXIS_LSB 16
#define X_AXIS_MSB 17
#define Y_AXIS_LSB 18
#define Y_AXIS_MSB 19
#define Z_AXIS_LSB 20
#define Z_AXIS_MSB 21

#define Rx_AXIS_LSB 22
#define Rx_AXIS_MSB 23
#define Ry_AXIS_LSB 24
#define Ry_AXIS_MSB 25
#define Rz_AXIS_LSB 26
#define Rz_AXIS_MSB 27

#define SLIDER_AXIS_LSB 28
#define SLIDER_AXIS_MSB 29
#define DIAL_AXIS_LSB 30
#define DIAL_AXIS_MSB 31

// EXTENDED ANALOG CONTROLS
#define WHEEL_AXIS_LSB 32
#define WHEEL_AXIS_MSB 33

// Axes 9-15: HID usages 0x39..0x3F (Generic Desktop). Usages must stay inside
// the 0x30..0x3F window, otherwise Chromium falls back to assigning free slots
// and the higher axes jump between indices. Inside the window the browser uses
// the fixed rule: axis index = usage - 0x30, so axes 9-15 always land on
// indices 9-15. (0x39 is nominally "Hat switch", 0x3A..0x3F are reserved -
// hats are not used by this fork, see report_desc().)
#define AXIS9_LSB 34
#define AXIS9_MSB 35
#define AXIS10_LSB 36
#define AXIS10_MSB 37
#define AXIS11_LSB 38
#define AXIS11_MSB 39
#define AXIS12_LSB 40
#define AXIS12_MSB 41
#define AXIS13_LSB 42
#define AXIS13_MSB 43
#define AXIS14_LSB 44
#define AXIS14_MSB 45
#define AXIS15_LSB 46
#define AXIS15_MSB 47

namespace arduino
{

    class PicoGamepad : public USBHID
    {
    public:
        // Array of 8 bit values which holds all inputs to be passed via HID

        /**
    * Basic constructor
    *
    * Construct this object optionally connecting and blocking until it is ready.
    *
    * @note Do not use this constructor in derived classes.
    *
    * @param connect_blocking true to perform a blocking connect, false to start in a disconnected state
    * @param vendor_id Your vendor_id
    * @param product_id Your product_id
    * @param product_release Your product_release
    */
        PicoGamepad(bool connect_blocking = true, uint16_t vendor_id = 0x1235, uint16_t product_id = 0x0050, uint16_t product_release = 0x0001);

        /**
    * Fully featured constructor
    *
    * Construct this object with the supplied USBPhy and parameters. The user
    * this object is responsible for calling connect() or init().
    *
    * @note Derived classes must use this constructor and call init() or
    * connect() themselves. Derived classes should also call deinit() in
    * their destructor. This ensures that no interrupts can occur when the
    * object is partially constructed or destroyed.
    *
    * @param phy USB phy to use
    * @param vendor_id Your vendor_id
    * @param product_id Your product_id
    * @param product_release Your product_release
    */
        PicoGamepad(USBPhy *phy, uint16_t vendor_id = 0x1235, uint16_t product_id = 0x0050, uint16_t product_release = 0x0001);

        /**
     * Destroy this object
     *
     * Any classes which inherit from this class must call deinit
     * before this destructor runs.
     */
        virtual ~PicoGamepad();

        /**
    * To send a character defined by a modifier(CTRL, SHIFT, ALT) and the key
    *
    * @code
    * //To send CTRL + s (save)
    *  keyboard.key_code('s', KEY_CTRL);
    * @endcode
    *
    * @param modifier bit 0: KEY_CTRL, bit 1: KEY_SHIFT, bit 2: KEY_ALT (default: 0)
    * @param key character to send
    * @returns true if there is no error, false otherwise
    */

        bool send_inputs(uint8_t *values);

        
        bool randomizeInputs();

        void SetButton(int idx, bool val);
        void SetAxis(int idx, uint16_t val);
        void SetX(uint16_t val);
        void SetY(uint16_t val);
        void SetZ(uint16_t val);
        void SetRx(uint16_t val);
        void SetRy(uint16_t val);
        void SetRz(uint16_t val);
        void SetSlider(uint16_t val);
        void SetDial(uint16_t val);
        void SetWheel(uint16_t val);
        void SetAxis9(uint16_t val);
        void SetAxis10(uint16_t val);
        void SetAxis11(uint16_t val);
        void SetAxis12(uint16_t val);
        void SetAxis13(uint16_t val);
        void SetAxis14(uint16_t val);
        void SetAxis15(uint16_t val);

        bool send_update();

        // Кількість байтів стану (без провідного Report ID у звіті):
        // 16 кнопок + 16 осей = 48 (див. inputArray нижче).
        static const size_t INPUT_BYTES = 48;

        // Доступ лише для читання до поточного стану — щоб викликач міг
        // визначити, чи стан змінився, перш ніж надсилати звіт.
        const uint8_t *getInputs() const { return inputArray; }

        /*
    * To define the report descriptor. Warning: this method has to store the length of the report descriptor in reportLength.
    *
    * @returns pointer to the report descriptor
    */
        virtual const uint8_t *report_desc();

    protected:
        /*
    * Get configuration descriptor
    *
    * @returns pointer to the configuration descriptor
    */
        virtual const uint8_t *configuration_desc(uint8_t index);

    private:
        // 16 buttons (16 B) + 16 axes (32 B) = 48 bytes
        uint8_t inputArray[48];

        uint8_t _configuration_descriptor[41];
        PlatformMutex _mutex;
    };
}

#endif
