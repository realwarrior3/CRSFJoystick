#ifndef CALIBRATION
#define CALIBRATION

// ---------------------------------------------------------------------------
// Калібрування аналогових осей
//
// Значення нижче задають "вікно" сирих значень CRSF-каналу (у мікросекундах),
// яке розтягується на весь хід джойстика -32767..32767. Вікно відповідає
// номінальному діапазону каналів ELRS (≈988..2012 мкс). Значення, що виходять
// за це вікно, обрізаються функцією mapAxis() у main.cpp, щоб 16-бітна вісь не
// "загорталася" на протилежний бік.
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

#define CHANNEL_9_LOW_EP 989
#define CHANNEL_9_HIGH_EP 2011

#define CHANNEL_10_LOW_EP 989
#define CHANNEL_10_HIGH_EP 2011

#define CHANNEL_11_LOW_EP 989
#define CHANNEL_11_HIGH_EP 2011

#define CHANNEL_12_LOW_EP 989
#define CHANNEL_12_HIGH_EP 2011

#define CHANNEL_13_LOW_EP 989
#define CHANNEL_13_HIGH_EP 2011

#define CHANNEL_14_LOW_EP 989
#define CHANNEL_14_HIGH_EP 2011

#define CHANNEL_15_LOW_EP 989
#define CHANNEL_15_HIGH_EP 2011

#define CHANNEL_16_LOW_EP 989
#define CHANNEL_16_HIGH_EP 2011

// ---------------------------------------------------------------------------
// Опт-ін: віддавати службові канали 15/16 (у ELRS — LQ % і RSSI %) осями
// Axis14 / Axis15. За замовчуванням ВИМКНЕНО: це показники якості зв'язку, а не
// органи керування, і вони не повинні впливати на гру. Розкоментуйте рядок
// нижче, щоб увімкнути (LQ/RSSI — це 0..100%, за потреби підстроїте межі).
// ---------------------------------------------------------------------------
// #define MAP_SIGNAL_AXES 1

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

#define SW_HYSTERESIS 20  // гістерезис для кнопок (мкс)

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
// обмеження — скільки записів ви додасте нижче. NUM_BUTTONS виводиться з
// розміру btn_map автоматично, але стежте, щоб кожен id (0..127) був унікальним.
//
// Типова карта дає кожному AUX-перемикачу (канали 5-12) три окремі кнопки —
// по одній на фізичне положення. Оскільки один CRSF-канал може керувати
// багатьма кнопками через вікна, кількість доступних "привʼязок" обмежена
// лише стелею у 128 кнопок HID. Стіки (канали 1-4) лишаються суто
// аналоговими осями і кнопок не натискають.
//
// Щоб використовувати менше кнопок, просто видаліть записи — NUM_BUTTONS
// перерахується автоматично.
// ---------------------------------------------------------------------------
static const btn_config btn_map[] = {
    // Канал 5 — AUX1: трипозиційний
    {  5,  0, SW_LOW_LO,   SW_LOW_HI,   false },
    {  5,  1, SW_MID_LO,   SW_MID_HI,   false },
    {  5,  2, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 6 — AUX2: трипозиційний
    {  6,  3, SW_LOW_LO,   SW_LOW_HI,   false },
    {  6,  4, SW_MID_LO,   SW_MID_HI,   false },
    {  6,  5, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 7 — AUX3: трипозиційний
    {  7,  6, SW_LOW_LO,   SW_LOW_HI,   false },
    {  7,  7, SW_MID_LO,   SW_MID_HI,   false },
    {  7,  8, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 8 — AUX4: трипозиційний
    {  8,  9, SW_LOW_LO,   SW_LOW_HI,   false },
    {  8, 10, SW_MID_LO,   SW_MID_HI,   false },
    {  8, 11, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 9 — AUX5: трипозиційний
    {  9, 12, SW_LOW_LO,   SW_LOW_HI,   false },
    {  9, 13, SW_MID_LO,   SW_MID_HI,   false },
    {  9, 14, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 10 — AUX6: трипозиційний
    { 10, 15, SW_LOW_LO,   SW_LOW_HI,   false },
    { 10, 16, SW_MID_LO,   SW_MID_HI,   false },
    { 10, 17, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 11 — AUX7: трипозиційний
    { 11, 18, SW_LOW_LO,   SW_LOW_HI,   false },
    { 11, 19, SW_MID_LO,   SW_MID_HI,   false },
    { 11, 20, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 12 — AUX8: трипозиційний
    { 12, 21, SW_LOW_LO,   SW_LOW_HI,   false },
    { 12, 22, SW_MID_LO,   SW_MID_HI,   false },
    { 12, 23, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 13 — AUX9: можна залишити на кнопки за потреби (трипозиційний/двопозиційний)
    { 13, 24, SW_LOW_LO,   SW_LOW_HI,   false },
    { 13, 25, SW_MID_LO,   SW_MID_HI,   false },
    { 13, 26, SW_HIGH_LO,  SW_HIGH_HI,  false },

    // Канал 14 — AUX10: трипозиційний (має вісь Axis 13)
    { 14, 27, SW_LOW_LO,   SW_LOW_HI,   false },
    { 14, 28, SW_MID_LO,   SW_MID_HI,   false },
    { 14, 29, SW_HIGH_LO,  SW_HIGH_HI,  false },
};

// Кількість записів визначається автоматично за розміром масиву. Раніше
// NUM_BUTTONS задавався вручну і міг розійтися з фактичною кількістю записів:
// тоді цикли в main.cpp читали неіснуючі записи поза масивом (значення
// каналу поза діапазоном 1..16 → вихід за межі _channels у CrsfSerial) і
// перевизначали кнопки з id = 0.
#define NUM_BUTTONS ((int)(sizeof(btn_map) / sizeof(btn_map[0])))

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
// Канал 13 - Aux 9 (також вісь Axis 12)
// Канал 14 - Aux 10 (також вісь Axis 13)
// Канал 15 - LQ
// Канал 16 - RSSI

#endif
