#ifndef CALIBRATION
#define CALIBRATION

// ---------------------------------------------------------------------------
// Analog axis calibration
//
// The values below define the raw CRSF channel window (in microseconds) that
// is stretched to the full -32767..32767 joystick travel.  Typical CRSF
// channels idle around 1000..2000us but every receiver/transmitter is a little
// different, so these are intentionally a touch wider than the nominal range.
// ---------------------------------------------------------------------------
#define CHANNEL_1_LOW_EP 989
#define CHANNEL_1_HIGH_EP 2011

#define CHANNEL_2_LOW_EP 989
#define CHANNEL_2_HIGH_EP 2011

#define CHANNEL_3_LOW_EP 989
#define CHANNEL_3_HIGH_EP 2011

#define CHANNEL_4_LOW_EP 989
#define CHANNEL_4_HIGH_EP 2011

#define CHANNEL_5_LOW_EP 989
#define CHANNEL_5_HIGH_EP 2011

#define CHANNEL_6_LOW_EP 989
#define CHANNEL_6_HIGH_EP 2011

#define CHANNEL_7_LOW_EP 989
#define CHANNEL_7_HIGH_EP 2011

#define CHANNEL_8_LOW_EP 989
#define CHANNEL_8_HIGH_EP 2011

#define JOYSTICK_LOW (-32767)
#define JOYSTICK_HIGH 32767

// ---------------------------------------------------------------------------
// Switch position detection windows
//
// A three position switch reports discrete values around 1000 / 1500 / 2000.
// Each window is deliberately generous so a slightly off-centre switch still
// registers.  Two position switches simply never enter the MID window.
// ---------------------------------------------------------------------------
#define SW_LOW_LO 800
#define SW_LOW_HI 1250
#define SW_MID_LO 1350
#define SW_MID_HI 1650
#define SW_HIGH_LO 1750
#define SW_HIGH_HI 2200

// lower_bound / upper_bound constrain where a button reports HIGH (inclusive).
// When invert is true the button instead reports HIGH *outside* the window.
typedef struct btn_config {
    byte channel;
    byte id;
    int lower_bound;
    int upper_bound;
    bool invert;
} btn_config;

// ---------------------------------------------------------------------------
// Button map
//
// The USB HID descriptor already exposes 128 buttons (see PicoGamepad.cpp), so
// the only limit is how many entries you add below - just keep NUM_BUTTONS in
// sync and make sure every id (0..127) is unique.
//
// The default map gives every AUX switch (channels 5-12) three independent
// buttons, one per physical position, plus optional stick deflection buttons
// on channels 1-4.  Since one CRSF channel can drive many buttons via windows,
// the number of usable "binds" is limited only by the 128 button HID ceiling.
//
// To use fewer buttons simply delete entries and lower NUM_BUTTONS.
// ---------------------------------------------------------------------------
#define NUM_BUTTONS 32

btn_config btn_map[NUM_BUTTONS] = {
    // --- AUX 1..8 - LOW position (channels 5-12) ---------------------------
    {  5,  0, SW_LOW_LO,  SW_LOW_HI,  false },
    {  6,  1, SW_LOW_LO,  SW_LOW_HI,  false },
    {  7,  2, SW_LOW_LO,  SW_LOW_HI,  false },
    {  8,  3, SW_LOW_LO,  SW_LOW_HI,  false },
    {  9,  4, SW_LOW_LO,  SW_LOW_HI,  false },
    { 10,  5, SW_LOW_LO,  SW_LOW_HI,  false },
    { 11,  6, SW_LOW_LO,  SW_LOW_HI,  false },
    { 12,  7, SW_LOW_LO,  SW_LOW_HI,  false },

    // --- AUX 1..8 - MIDDLE position ---------------------------------------
    {  5,  8, SW_MID_LO,  SW_MID_HI,  false },
    {  6,  9, SW_MID_LO,  SW_MID_HI,  false },
    {  7, 10, SW_MID_LO,  SW_MID_HI,  false },
    {  8, 11, SW_MID_LO,  SW_MID_HI,  false },
    {  9, 12, SW_MID_LO,  SW_MID_HI,  false },
    { 10, 13, SW_MID_LO,  SW_MID_HI,  false },
    { 11, 14, SW_MID_LO,  SW_MID_HI,  false },
    { 12, 15, SW_MID_LO,  SW_MID_HI,  false },

    // --- AUX 1..8 - HIGH position -----------------------------------------
    {  5, 16, SW_HIGH_LO, SW_HIGH_HI, false },
    {  6, 17, SW_HIGH_LO, SW_HIGH_HI, false },
    {  7, 18, SW_HIGH_LO, SW_HIGH_HI, false },
    {  8, 19, SW_HIGH_LO, SW_HIGH_HI, false },
    {  9, 20, SW_HIGH_LO, SW_HIGH_HI, false },
    { 10, 21, SW_HIGH_LO, SW_HIGH_HI, false },
    { 11, 22, SW_HIGH_LO, SW_HIGH_HI, false },
    { 12, 23, SW_HIGH_LO, SW_HIGH_HI, false },

    // --- Optional: stick deflection as buttons (channels 1-4) --------------
    // Delete these (and lower NUM_BUTTONS) if your sticks are used purely as
    // analog axes and you do not want a full deflection to fire a button.
    {  1, 24, SW_HIGH_LO, SW_HIGH_HI, false },
    {  2, 25, SW_HIGH_LO, SW_HIGH_HI, false },
    {  3, 26, SW_HIGH_LO, SW_HIGH_HI, false },
    {  4, 27, SW_HIGH_LO, SW_HIGH_HI, false },
    {  1, 28, SW_LOW_LO,  SW_LOW_HI,  false },
    {  2, 29, SW_LOW_LO,  SW_LOW_HI,  false },
    {  3, 30, SW_LOW_LO,  SW_LOW_HI,  false },
    {  4, 31, SW_LOW_LO,  SW_LOW_HI,  false },
};

// Channel1 - A
// Channel2 - E
// Channel3 - T
// Channel4 - R
// Channel5 - Aux 1
// Channel6 - Aux 2
// Channel7 - Aux 3
// Channel8 - Aux 4
// Channel9 - Aux 5
// Channel10 - Aux 6
// Channel11 - Aux 7
// Channel12 - Aux 8
// Channel13
// Channel14
// Channel15 - LQ
// Channel16 - RSSI

#endif
