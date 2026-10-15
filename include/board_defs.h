#ifndef BOARD_DEFS
#define BOARD_DEFS

// Raspberry Pi Pico
#if BOARD_ID == 1
    #define CRSF_TX 4  // фізичний пін 6
    #define CRSF_RX 5  // фізичний пін 7

    #define LED_PIN 25

    void boardSetup() {
      pinMode(LED_PIN, OUTPUT);
    }

    void led_off() {
      digitalWrite(LED_PIN, LOW);
    }

    void led_on() {
      digitalWrite(LED_PIN, HIGH);
    }
#endif

// Pimoroni Tiny 2040
#if BOARD_ID == 2
    #define CRSF_TX 4
    #define CRSF_RX 5

    #define LED_PIN_R 18
    #define LED_PIN_G 19
    #define LED_PIN_B 20

    void boardSetup() {
      pinMode(LED_PIN_R, OUTPUT);
      pinMode(LED_PIN_G, OUTPUT);
      pinMode(LED_PIN_B, OUTPUT);
    }

    void led_off() {
      digitalWrite(LED_PIN_R, HIGH);
      digitalWrite(LED_PIN_G, HIGH);
      digitalWrite(LED_PIN_B, HIGH);
    }

    void led_on() {
      digitalWrite(LED_PIN_R, HIGH);
      digitalWrite(LED_PIN_G, HIGH);
      digitalWrite(LED_PIN_B, LOW);
    }
#endif

// WaveShare RP2040 Zero
#if BOARD_ID == 3
    #define CRSF_TX 4
    #define CRSF_RX 5

    #define LED_PIN 16

    #include <NeoPixelConnect.h>
    NeoPixelConnect p(LED_PIN, 1, pio0, 0);

    void boardSetup() {
      // нічого не робимо
    }

    void led_off() {
        p.neoPixelFill(0, 0, 0, true);
    }

    void led_on() {
        p.neoPixelFill(0, 0, 255, true);
    }
#endif

// Seeed XIAO RP2040
#if BOARD_ID == 4
    #define CRSF_TX 0
    #define CRSF_RX 1

    // 1 NeoPixel — використовується як індикатор звʼязку
    #define LED_NEO_PWR 11
    #define LED_PIN 12

    #include <NeoPixelConnect.h>
    NeoPixelConnect p(LED_PIN, 1, pio0, 0);

    // Вбудований RGB-світлодіод XIAO (активний низький рівень) для
    // індикації не використовується — лише гаситься під час старту.
    #define LED_PIN_R 17
    #define LED_PIN_G 16
    #define LED_PIN_B 25

    void boardSetup() {
        // Увімкнути NeoPixel LED
        pinMode(LED_NEO_PWR, OUTPUT);
        digitalWrite(LED_NEO_PWR, HIGH);
        // Вимкнути користувацький RGB-світлодіод
        pinMode(LED_PIN_R, OUTPUT);
        pinMode(LED_PIN_G, OUTPUT);
        pinMode(LED_PIN_B, OUTPUT);
        digitalWrite(LED_PIN_R, HIGH);
        digitalWrite(LED_PIN_G, HIGH);
        digitalWrite(LED_PIN_B, HIGH);
    }

    void led_off() {
        p.neoPixelFill(0, 0, 0, true);
    }

    void led_on() {
        p.neoPixelFill(0, 0, 255, true);
    }
#endif

// Adafruit QT Py RP2040
#if BOARD_ID == 5
    #define CRSF_TX 20
    #define CRSF_RX 5

    // 1 NeoPixel
    #define LED_NEO_PWR 11
    #define LED_PIN 12

    #include <NeoPixelConnect.h>
    NeoPixelConnect p(LED_PIN, 1, pio0, 0);

    void boardSetup() {
        pinMode(LED_NEO_PWR, OUTPUT);
        digitalWrite(LED_NEO_PWR, HIGH);
    }

    void led_off() {
        p.neoPixelFill(0, 0, 0, true);
    }

    void led_on() {
        p.neoPixelFill(0, 0, 255, true);
    }
#endif

#endif
