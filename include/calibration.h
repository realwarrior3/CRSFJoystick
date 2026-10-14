#ifndef CALIBRATION
#define CALIBRATION

// ---------------------------------------------------------------------------
// Калібрування аналогових осей
//
// Значення нижче задають "вікно" сирих значень CRSF-каналу (у мікросекундах),
// яке розтягується на весь хід джойстика -32767..32767. Зазвичай канали CRSF
// у спокої лежать у діапазоні 1000..2000 мкс, але кожен приймач/пульт трохи
// відрізняється, тому ці межі навмисно трохи ширші за номінальний діапазон.
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
// Вікна визначення положення перемикачів
//
// Трипозиційний перемикач видає дискретні значення близько 1000 / 1500 / 2000.
// Кожне вікно навмисно зроблене із запасом, щоб трохи зміщений перемикач усе
// одно спрацьовував. Двопозиційний перемикач просто ніколи не потрапляє у
// вікно MID.
// ---------------------------------------------------------------------------
#define SW_LOW_LO 800
#define SW_LOW_HI 1250
#define SW_MID_LO 1350
#define SW_MID_HI 1650
#define SW_HIGH_LO 1750
#define SW_HIGH_HI 2200

// lower_bound / upper_bound задають межі, у яких кнопка звітує HIGH (включно).
// Якщо invert = true, кнопка натомість звітує HIGH *поза* цим вікном.
typedef struct btn_config {
    byte channel;
    byte id;
    int lower_bound;
    int upper_bound;
    bool invert;
} btn_config;

// ---------------------------------------------------------------------------
// Карта кнопок
//
// USB HID-дескриптор уже надає 128 кнопок (див. PicoGamepad.cpp), тому єдине
// обмеження — скільки записів ви додасте нижче. Просто тримайте NUM_BUTTONS
// у відповідності та стежте, щоб кожен id (0..127) був унікальним.
//
// Типова карта дає кожному AUX-перемикачу (канали 5-12) три окремі кнопки —
// по одній на фізичне положення, плюс додаткові кнопки на відхилення стіків
// для каналів 1-4. Оскільки один CRSF-канал може керувати багатьма кнопками
// через вікна, кількість доступних "привʼязок" обмежена лише стелею у 128
// кнопок HID.
//
// Щоб використовувати менше кнопок, просто видаліть записи та зменште
// NUM_BUTTONS.
// ---------------------------------------------------------------------------
#define NUM_BUTTONS 32

btn_config btn_map[NUM_BUTTONS] = {
    // --- AUX 1..8 — положення LOW (канали 5-12) ----------------------------
    {  5,  0, SW_LOW_LO,  SW_LOW_HI,  false },
    {  6,  1, SW_LOW_LO,  SW_LOW_HI,  false },
    {  7,  2, SW_LOW_LO,  SW_LOW_HI,  false },
    {  8,  3, SW_LOW_LO,  SW_LOW_HI,  false },
    {  9,  4, SW_LOW_LO,  SW_LOW_HI,  false },
    { 10,  5, SW_LOW_LO,  SW_LOW_HI,  false },
    { 11,  6, SW_LOW_LO,  SW_LOW_HI,  false },
    { 12,  7, SW_LOW_LO,  SW_LOW_HI,  false },

    // --- AUX 1..8 — середнє положення --------------------------------------
    {  5,  8, SW_MID_LO,  SW_MID_HI,  false },
    {  6,  9, SW_MID_LO,  SW_MID_HI,  false },
    {  7, 10, SW_MID_LO,  SW_MID_HI,  false },
    {  8, 11, SW_MID_LO,  SW_MID_HI,  false },
    {  9, 12, SW_MID_LO,  SW_MID_HI,  false },
    { 10, 13, SW_MID_LO,  SW_MID_HI,  false },
    { 11, 14, SW_MID_LO,  SW_MID_HI,  false },
    { 12, 15, SW_MID_LO,  SW_MID_HI,  false },

    // --- AUX 1..8 — положення HIGH -----------------------------------------
    {  5, 16, SW_HIGH_LO, SW_HIGH_HI, false },
    {  6, 17, SW_HIGH_LO, SW_HIGH_HI, false },
    {  7, 18, SW_HIGH_LO, SW_HIGH_HI, false },
    {  8, 19, SW_HIGH_LO, SW_HIGH_HI, false },
    {  9, 20, SW_HIGH_LO, SW_HIGH_HI, false },
    { 10, 21, SW_HIGH_LO, SW_HIGH_HI, false },
    { 11, 22, SW_HIGH_LO, SW_HIGH_HI, false },
    { 12, 23, SW_HIGH_LO, SW_HIGH_HI, false },

    // --- Опційно: відхилення стіків як кнопки (канали 1-4) ------------------
    // Видаліть ці записи (і зменште NUM_BUTTONS), якщо стіки використовуються
    // суто як аналогові осі і ви не хочете, щоб повне відхилення натискало
    // кнопку.
    {  1, 24, SW_HIGH_LO, SW_HIGH_HI, false },
    {  2, 25, SW_HIGH_LO, SW_HIGH_HI, false },
    {  3, 26, SW_HIGH_LO, SW_HIGH_HI, false },
    {  4, 27, SW_HIGH_LO, SW_HIGH_HI, false },
    {  1, 28, SW_LOW_LO,  SW_LOW_HI,  false },
    {  2, 29, SW_LOW_LO,  SW_LOW_HI,  false },
    {  3, 30, SW_LOW_LO,  SW_LOW_HI,  false },
    {  4, 31, SW_LOW_LO,  SW_LOW_HI,  false },
};

// Канал 1  - A
// Канал 2  - E
// Канал 3  - T
// Канал 4  - R
// Канал 5  - Aux 1
// Канал 6  - Aux 2
// Канал 7  - Aux 3
// Канал 8  - Aux 4
// Канал 9  - Aux 5
// Канал 10 - Aux 6
// Канал 11 - Aux 7
// Канал 12 - Aux 8
// Канал 13
// Канал 14
// Канал 15 - LQ
// Канал 16 - RSSI

#endif
