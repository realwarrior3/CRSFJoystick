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

#include "stdint.h"
#include "PluggableUSBHID.h"
#include "PicoGamepad.h"
#include "usb_phy_api.h"

using namespace arduino;

PicoGamepad::PicoGamepad(bool connect, uint16_t vendor_id, uint16_t product_id, uint16_t product_release) : USBHID(get_usb_phy(), 0, 0, vendor_id, product_id, product_release)
{
    // Fix vs upstream: zero every input so unused axes/buttons read neutral
    // instead of random RAM contents.
    memset(inputArray, 0, sizeof(inputArray));
}

PicoGamepad::PicoGamepad(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release) : USBHID(phy, 0, 0, vendor_id, product_id, product_release)
{
    // Fix vs upstream: zero every input so unused axes/buttons read neutral
    // instead of random RAM contents.
    memset(inputArray, 0, sizeof(inputArray));
    // User or child responsible for calling connect or init
}

PicoGamepad::~PicoGamepad()
{
}

const uint8_t *PicoGamepad::report_desc()
{
    static const uint8_t reportDescriptor[] = {
        // HEADER
        0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
        0x09, 0x04,         // USAGE (Gamepad)
        0xA1, 0x01,         // COLLECTION (Application)
        0x85, 0x01,         // REPORT_ID (1)

        // BUTTONS (128 buttons)
        0x05, 0x09,         // USAGE_PAGE (Button)
        0x19, 0x01,         // USAGE_MINIMUM (Button 1)
        0x29, 0x80,         // USAGE_MAXIMUM (Button 128)
        0x15, 0x00,         // LOGICAL_MINIMUM (0)
        0x25, 0x01,         // LOGICAL_MAXIMUM (1)
        0x75, 0x01,         // REPORT_SIZE (1)
        0x95, 0x80,         // REPORT_COUNT (128)
        0x81, 0x02,         // INPUT (Data,Var,Abs)

        // 16 ANALOG AXES (16-bit signed)
        0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
        // First 8 axes (Standard game controls)
        0x09, 0x30,         // USAGE (X)
        0x09, 0x31,         // USAGE (Y)
        0x09, 0x32,         // USAGE (Z)
        0x09, 0x33,         // USAGE (Rx)
        0x09, 0x34,         // USAGE (Ry)
        0x09, 0x35,         // USAGE (Rz)
        0x09, 0x36,         // USAGE (Slider)
        0x09, 0x37,         // USAGE (Dial)
        // Next 8 axes (Extended analog controls). Usages 0x39..0x3F keep every
        // axis inside the 0x30..0x3F window, so Chromium uses the fixed rule
        // axis index = usage - 0x30 and axes never jump between slots.
        // (0x39 is nominally "Hat switch", 0x3A..0x3F are reserved; the browser
        // treats them as plain Generic Desktop usages. Hats are unused here.)
        0x09, 0x38,         // USAGE (Wheel - axis 8)
        0x09, 0x39,         // USAGE (axis 9)
        0x09, 0x3A,         // USAGE (axis 10)
        0x09, 0x3B,         // USAGE (axis 11)
        0x09, 0x3C,         // USAGE (axis 12)
        0x09, 0x3D,         // USAGE (axis 13)
        0x09, 0x3E,         // USAGE (axis 14)
        0x09, 0x3F,         // USAGE (axis 15)

        0x16, 0x01, 0x80,   // LOGICAL_MINIMUM (-32767)
        0x26, 0xFF, 0x7F,   // LOGICAL_MAXIMUM (32767)
        0x75, 0x10,         // REPORT_SIZE (16)
        0x95, 0x10,         // REPORT_COUNT (16)  // 16 axes!
        0x81, 0x02,         // INPUT (Data,Var,Abs)

        0xC0                // END_COLLECTION
    };
    reportLength = sizeof(reportDescriptor);
    return reportDescriptor;
}

bool PicoGamepad::randomizeInputs()
{
    _mutex.lock();

    HID_REPORT report;
    report.data[0] = 0x01;
    for (size_t i = 1; i <= INPUT_BYTES; i++)
    {
        report.data[i] = random();
    }
    report.length = INPUT_BYTES + 1;

    if (!send(&report))
    {
        _mutex.unlock();
        return false;
    }

    _mutex.unlock();
    return true;
}

void PicoGamepad::SetButton(int idx, bool val)
{
    if (idx >= 128 || idx < 0)
    {
        return;
    }
    bitWrite(inputArray[idx / 8], idx % 8, val);
}

void PicoGamepad::SetAxis(int idx, uint16_t val)
{
    if (idx < 0 || idx > 15)
    {
        return;
    }
    if (idx > 9 && idx < 16){
        idx = 25 - idx;
    }

    inputArray[16 + (idx * 2)] = LSB(val);
    inputArray[16 + (idx * 2)+1] = MSB(val);
}

void PicoGamepad::SetX(uint16_t val)
{
    inputArray[X_AXIS_LSB] = LSB(val);
    inputArray[X_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetY(uint16_t val)
{
    inputArray[Y_AXIS_LSB] = LSB(val);
    inputArray[Y_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetZ(uint16_t val)
{
    inputArray[Z_AXIS_LSB] = LSB(val);
    inputArray[Z_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetRx(uint16_t val)
{
    inputArray[Rx_AXIS_LSB] = LSB(val);
    inputArray[Rx_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetRy(uint16_t val)
{
    inputArray[Ry_AXIS_LSB] = LSB(val);
    inputArray[Ry_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetRz(uint16_t val)
{
    inputArray[Rz_AXIS_LSB] = LSB(val);
    inputArray[Rz_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetSlider(uint16_t val)
{
    inputArray[SLIDER_AXIS_LSB] = LSB(val);
    inputArray[SLIDER_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetDial(uint16_t val)
{
    inputArray[DIAL_AXIS_LSB] = LSB(val);
    inputArray[DIAL_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetWheel(uint16_t val)
{
    inputArray[WHEEL_AXIS_LSB] = LSB(val);
    inputArray[WHEEL_AXIS_MSB] = MSB(val);
}

void PicoGamepad::SetAxis9(uint16_t val)
{
    inputArray[AXIS9_LSB] = LSB(val);
    inputArray[AXIS9_MSB] = MSB(val);
}

void PicoGamepad::SetAxis10(uint16_t val)
{
    inputArray[AXIS10_LSB] = LSB(val);
    inputArray[AXIS10_MSB] = MSB(val);
}

void PicoGamepad::SetAxis11(uint16_t val)
{
    inputArray[AXIS11_LSB] = LSB(val);
    inputArray[AXIS11_MSB] = MSB(val);
}

void PicoGamepad::SetAxis12(uint16_t val)
{
    inputArray[AXIS12_LSB] = LSB(val);
    inputArray[AXIS12_MSB] = MSB(val);
}

void PicoGamepad::SetAxis13(uint16_t val)
{
    inputArray[AXIS13_LSB] = LSB(val);
    inputArray[AXIS13_MSB] = MSB(val);
}

void PicoGamepad::SetAxis14(uint16_t val)
{
    inputArray[AXIS14_LSB] = LSB(val);
    inputArray[AXIS14_MSB] = MSB(val);
}

void PicoGamepad::SetAxis15(uint16_t val)
{
    inputArray[AXIS15_LSB] = LSB(val);
    inputArray[AXIS15_MSB] = MSB(val);
}

bool PicoGamepad::send_update()
{
    _mutex.lock();

    HID_REPORT report;
    report.data[0] = 0x01;
    for (size_t i = 1; i <= INPUT_BYTES; i++)
    {
        report.data[i] = inputArray[i - 1];
    }

    report.length = INPUT_BYTES + 1;

    if (!send(&report))
    {
        _mutex.unlock();
        return false;
    }

    _mutex.unlock();
    return true;
}

bool PicoGamepad::send_inputs(uint8_t *values)
{
    _mutex.lock();

    HID_REPORT report;
    report.data[0] = 0x01;
    for (size_t i = 1; i <= INPUT_BYTES; i++)
    {
        report.data[i] = values[i - 1];
    }

    report.length = INPUT_BYTES + 1;

    if (!send(&report))
    {
        _mutex.unlock();
        return false;
    }

    _mutex.unlock();
    return true;
}



#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) + (1 * INTERFACE_DESCRIPTOR_LENGTH) + (1 * HID_DESCRIPTOR_LENGTH) + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

const uint8_t *PicoGamepad::configuration_desc(uint8_t index)
{
    if (index != 0)
    {
        return NULL;
    }
    uint8_t configuration_descriptor_temp[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH, // bLength
        CONFIGURATION_DESCRIPTOR,        // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),    // wTotalLength (MSB)
        0x01,                            // bNumInterfaces
        DEFAULT_CONFIGURATION,           // bConfigurationValue
        0x00,                            // iConfiguration
        C_RESERVED | C_SELF_POWERED,     // bmAttributes
        C_POWER(0),                      // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH, // bLength
        INTERFACE_DESCRIPTOR,        // bDescriptorType
        0x00,                        // bInterfaceNumber
        0x00,                        // bAlternateSetting
        0x02,                        // bNumEndpoints
        HID_CLASS,                   // bInterfaceClass
        HID_SUBCLASS_NONE,           // bInterfaceSubClass
        HID_PROTOCOL_NONE,           // bInterfaceProtocol
        0x00,                        // iInterface

        HID_DESCRIPTOR_LENGTH,                // bLength
        HID_DESCRIPTOR,                       // bDescriptorType
        LSB(HID_VERSION_1_11),                // bcdHID (LSB)
        MSB(HID_VERSION_1_11),                // bcdHID (MSB)
        0x00,                                 // bCountryCode
        0x01,                                 // bNumDescriptors
        REPORT_DESCRIPTOR,                    // bDescriptorType
        (uint8_t)(LSB(report_desc_length())), // wDescriptorLength (LSB)
        (uint8_t)(MSB(report_desc_length())), // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH, // bLength
        ENDPOINT_DESCRIPTOR,        // bDescriptorType
        _int_in,                    // bEndpointAddress
        E_INTERRUPT,                // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
        1,                          // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH, // bLength
        ENDPOINT_DESCRIPTOR,        // bDescriptorType
        _int_out,                   // bEndpointAddress
        E_INTERRUPT,                // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),   // wMaxPacketSize (MSB)
        1,                          // bInterval (milliseconds)
    };
    MBED_ASSERT(sizeof(configuration_descriptor_temp) == sizeof(_configuration_descriptor));
    memcpy(_configuration_descriptor, configuration_descriptor_temp, sizeof(_configuration_descriptor));
    return _configuration_descriptor;
}
