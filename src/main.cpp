#include <Arduino.h>
#include <CrsfSerial.h>  // https://github.com/CapnBry/CRServoF/
#include <PicoGamepad.h> // https://gitlab.com/realrobots/PicoGamepad/-/tree/master

#include "calibration.h"

// ВИБІР BOARD_ID ВІДБУВАЄТЬСЯ ПРИ ВИБОРІ СЕРЕДОВИЩА PLATFORMIO

// Змінні та відстеження стану рутини блимання
#define BLINK_ENABLED                    // закоментуйте цей рядок, щоб вимкнути блимання LED
#define BLINK_TIME 60000                 // вікно рутини блимання (мс)
#define BLINK_DELAY 500                  // затримка між змінами стану LED (мс)
#define FAILSAFE_RESEND_MS 100           // як часто повторно надсилати центральний failsafe-стан, поки немає звʼязку (мс)

static bool serialEcho;
static char serialInBuff[64];
static uint8_t serialInBuffLen;

bool led_state = false;                  // поточний стан LED (увімк./вимк.)
unsigned long ms_curr = 0;               // поточний час
unsigned long ms_last_link_changed = 0;  // час останньої зміни стану CRSF-звʼязку
unsigned long ms_last_led_changed = 0;   // час останньої зміни стану LED у рутині блимання
unsigned long ms_last_failsafe_sent = 0; // час останнього надсилання центрального failsafe-стану

#include "board_defs.h"

UART Serial2(CRSF_TX, CRSF_RX, NC, NC);

CrsfSerial crsf(Serial2, CRSF_BAUDRATE); // передайте будь-який HardwareSerial-порт
int channel_data = 0;
int map_data = 0;

PicoGamepad gamepad;

btn_config *c;

// ---------------------------------------------------------------------------
// Надсилання HID-звіту лише за потреби
//
// USB HID interrupt-ендпоінт має bInterval = 1 мс, тож надсилати звіт частіше
// немає сенсу. Якщо стан не змінився — надсилання нічого не дає, тож
// пропускаємо його: менше USB-трафіку й прокидань CPU. Для failsafe-стану
// надсилання форсується (force = true), щоб гарантовано повторювати його,
// поки немає звʼязку.
// ---------------------------------------------------------------------------
static uint8_t hid_last_sent[PicoGamepad::INPUT_BYTES];
static bool hid_has_sent = false;
static unsigned long ms_last_hid_send = 0;

static void sendGamepadReport(bool force)
{
    const uint8_t *cur = gamepad.getInputs();

    if (!force)
    {
        // стан не змінився — надсилати нічого
        if (hid_has_sent && memcmp(cur, hid_last_sent, PicoGamepad::INPUT_BYTES) == 0)
            return;

        // не частіше одного звіту на мілісекунду (bInterval ендпоінта = 1)
        if (hid_has_sent && (millis() - ms_last_hid_send) < 1)
            return;
    }

    memcpy(hid_last_sent, cur, PicoGamepad::INPUT_BYTES);
    hid_has_sent = true;
    ms_last_hid_send = millis();

    // якщо USB-передавання не вдалося — дозволяємо повторну спробу наступного разу
    if (!gamepad.send_update())
        hid_has_sent = false;
}

/***
 * @brief Привести геймпад у безпечний стан: усі аналогові осі центруються,
 *        усі кнопки відпускаються. Використовується, коли CRSF-звʼязок
 *        відсутній, щоб втрата радіозвʼязку не залишила стіки/кнопки в
 *        останньому положенні (це спричинило б некероване керування в симі).
 ***/
void setFailsafeState()
{
    gamepad.SetX(0);
    gamepad.SetY(0);
    gamepad.SetZ(0);
    gamepad.SetRx(0);
    gamepad.SetRy(0);
    gamepad.SetRz(0);
    gamepad.SetDial(0);
    gamepad.SetSlider(0);

    for (uint8_t i = 0; i < NUM_BUTTONS; i++)
        gamepad.SetButton(btn_map[i].id, LOW);

    for (uint8_t h = 0; h < 4; h++)
        gamepad.SetHat(h, HAT_DIR_C);

    sendGamepadReport(true);
}

// Arduino map() НЕ обмежує результат: якщо сире значення каналу виходить за
// межі вікна (LOW_EP..HIGH_EP), воно екстраполюється далі. Для 16-бітної осі
// HID це означає, що значення може перевищити 32767 і «загорнутися» у
// відʼємне — тоді стік на краю миттєво стрибає на протилежний бік. Тому
// обмежуємо результат діапазоном осі.
static int mapAxis(int us, int lowEp, int highEp)
{
    return constrain(map(us, lowEp, highEp, JOYSTICK_LOW, JOYSTICK_HIGH),
                     JOYSTICK_LOW, JOYSTICK_HIGH);
}

/***
 * Цей колбек викликається щоразу, коли зʼявляються нові значення каналів.
 * Використовуйте crsf.getChannel(x), щоб отримати значення каналів (1-16).
 ***/
void packetChannels()
{
    // Розписуємо вручну, а не циклом, щоб за потреби міняти параметри

    // X — Канал 1 — A
    channel_data = crsf.getChannel(1);
    map_data = mapAxis(channel_data, CHANNEL_1_LOW_EP, CHANNEL_1_HIGH_EP);
    gamepad.SetX(map_data);

    // Y — Канал 2 — E
    channel_data = crsf.getChannel(2);
    map_data = mapAxis(channel_data, CHANNEL_2_LOW_EP, CHANNEL_2_HIGH_EP);
    gamepad.SetY(map_data);

    // Z — Канал 3 — T (Throttle)
    channel_data = crsf.getChannel(3);
    map_data = mapAxis(channel_data, CHANNEL_3_LOW_EP, CHANNEL_3_HIGH_EP);
    gamepad.SetZ(map_data);

    // Rz — Канал 4 — R (Rudder / Yaw)
    channel_data = crsf.getChannel(4);
    map_data = mapAxis(channel_data, CHANNEL_4_LOW_EP, CHANNEL_4_HIGH_EP);
    gamepad.SetRz(map_data);

    // Rx — Канал 5 — AUX1
    channel_data = crsf.getChannel(5);
    map_data = mapAxis(channel_data, CHANNEL_5_LOW_EP, CHANNEL_5_HIGH_EP);
    gamepad.SetRx(map_data);

    // Ry — Канал 6 — AUX2
    channel_data = crsf.getChannel(6);
    map_data = mapAxis(channel_data, CHANNEL_6_LOW_EP, CHANNEL_6_HIGH_EP);
    gamepad.SetRy(map_data);

    // Dial — Канал 7
    channel_data = crsf.getChannel(7);
    map_data = mapAxis(channel_data, CHANNEL_7_LOW_EP, CHANNEL_7_HIGH_EP);
    gamepad.SetDial(map_data);

    // Slider — Канал 8
    channel_data = crsf.getChannel(8);
    map_data = mapAxis(channel_data, CHANNEL_8_LOW_EP, CHANNEL_8_HIGH_EP);
    gamepad.SetSlider(map_data);

    // Решта осей (Wheel, Vx..Vno) не використовується: HID-дескриптор їх
    // експортує, але ми залишаємо їх у нулі (див. PicoGamepad).

    // Багатопозиційні перемикачі налаштовуються у calibrations.h
    // Кнопка звітує HIGH, коли канал перебуває в межах нижньої/верхньої межі
    // (включно). Типово HIGH (1510, 2011), інакше LOW.

    for(uint8_t i = 0; i < NUM_BUTTONS; i++){
      c = &btn_map[i];
      channel_data = crsf.getChannel(c->channel);
      // перевірка меж включно
      if(channel_data >= c->lower_bound && channel_data <= c->upper_bound) {
        map_data = c->invert ? LOW : HIGH;
      }
      else {
        map_data = c->invert ? HIGH : LOW;
      }
      gamepad.SetButton(c->id, map_data);

      #ifdef BTN_PRINT
      Serial.print("b: "); Serial.print(c->id);
      Serial.print(" c: "); Serial.print(channel_data);
      Serial.print(" m: "); Serial.println(map_data);
      #endif
    }
    // TODO що робити з каналами 13,14,15,16 (NA,NA,LQ,RSSI)

    // Задати напрямок хатів, доступно 4 хати. напрямок за годинниковою стрілкою, 0=N 1=NE 2=E 3=SE 4=S 5=SW 6=W 7=NW 8=CENTER
    // gamepad.SetHat(0, 8);

    sendGamepadReport(false);
}

void crsfLinkUp() {
  ms_last_link_changed = millis();
  ms_last_led_changed = ms_last_link_changed;
  led_state = true;
  led_on();
}

void crsfLinkDown() {
  ms_last_link_changed = millis();
  ms_last_led_changed = ms_last_link_changed;
  ms_last_failsafe_sent = ms_last_link_changed;
  led_state = false;
  led_off();

  // Радіозвʼязок втрачено — центруємо всі органи керування, щоб хост не зберіг останній ввід
  setFailsafeState();
}

static void passthroughBegin(uint32_t baud)
{
    if (baud != crsf.getBaud())
    {
        // Надсилаємо команду перезавантаження, бо хочемо передати
        // reboot на 420000, а потім перемкнутися на потрібну користувачу швидкість
        const uint8_t rebootpayload[] = { 'b', 'l' };
        crsf.queuePacket(CRSF_FRAMETYPE_COMMAND, &rebootpayload, sizeof(rebootpayload), CRSF_ADDRESS_CRSF_RECEIVER);
    }
    crsf.setPassthroughMode(true, baud);

    serialEcho = false;
}

static void crsfOobData(uint8_t b)
{
    Serial.write(b);
}

/***
 * @brief Обробляє текстову команду, наче це якийсь CLI
 * @return true, якщо CrsfSerial було переведено в режим паскрізу, інакше false
*/
static bool handleSerialCommand(char *cmd)
{
    // Імітуємо CRSF-приймач на UART6
    bool prompt = true;
    if (strcmp(cmd, "#") == 0)
    {
        Serial.println("Fake CLI Mode, type 'exit' or 'help' to do nothing\r\n");
        serialEcho = true;
    }

    else if (strcmp(cmd, "serial") == 0)
        Serial.println("serial 5 64 0 0 0 0\r\n");

    else if (strcmp(cmd, "get serialrx_provider") == 0)
        Serial.println("serialrx_provider = CRSF\r\n");

    else if (strcmp(cmd, "get serialrx_inverted") == 0)
        Serial.println("serialrx_inverted = OFF\r\n");

    else if (strcmp(cmd, "get serialrx_halfduplex") == 0)
        Serial.println("serialrx_halfduplex = OFF\r\n");

    else if (strncmp(cmd, "serialpassthrough 5 ", 20) == 0)
    {
        Serial.println(cmd);

        unsigned int baud = atoi(&cmd[20]);
        passthroughBegin(baud);

        return true;
    }

    else
        prompt = false;

    if (prompt)
        Serial.print("# ");

    return false;
}

static void checkSerialInNormal()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (serialEcho && c != '\n')
            Serial.write(c);

        if (c == '\r' || c == '\n')
        {
            if (serialInBuffLen != 0)
            {
                Serial.write('\n');
                serialInBuff[serialInBuffLen] = '\0';
                serialInBuffLen = 0;

                bool goToPassthrough = handleSerialCommand(serialInBuff);
                // Якщо потрібно перейти в паскріз — виходимо, поки не вибрали жодного байта з черги
                if (goToPassthrough)
                    return;
            }
        }
        // У Betaflight CLI входиш просто натиснувши #, без Enter
        else if (c == '#')
        {
            serialInBuffLen = 0;
            handleSerialCommand("#");
        }
        else
        {
            serialInBuff[serialInBuffLen++] = c;
            // якщо буфер заповнився без символу нового рядка — просто скидаємо його
            if (serialInBuffLen >= sizeof(serialInBuff))
                serialInBuffLen = 0;
        }
    }  /* while Serial */
}

static void checkSerialInPassthrough()
{
    static uint32_t lastData = 0;
    static bool LED = false;
    bool gotData = false;
    unsigned int avail;
    while ((avail = Serial.available()) != 0)
    {
        uint8_t buf[16];
        avail = Serial.readBytes((char *)buf, min(sizeof(buf), avail));
        crsf.write(buf, avail);
        LED ? led_on() : led_off();
        LED = !LED;
        gotData = true;
    }
    // Якщо минуло більше X секунд без даних — виходимо з режиму паскрізу
    if (gotData || !lastData)
        lastData = millis();

    // Вимкнути LED через 1 с після останніх даних
    else if (LED && (millis() - lastData > 1000))
    {
        LED = false;
        led_off();
    }

    // Коротке блимання LED після тайм-ауту
    else if (millis() - lastData > 5000)
    {
        lastData = 0;
        led_on();
        delay(200);
        led_off();
        crsf.setPassthroughMode(false);
    }
}

static void checkSerialIn()
{
    if (crsf.getPassthroughMode())
        checkSerialInPassthrough();
    else
        checkSerialInNormal();
}

#ifdef BLINK_ENABLED
void led_loop() {
  ms_curr = millis();
  // звʼязок відсутній
  if(!crsf.isLinkUp()) {
    // у межах вікна рутини блимання (BLINK_TIME)
    if(ms_curr < (ms_last_link_changed + BLINK_TIME)) {
      // обробка затримки перемикання LED
      if(ms_curr > (ms_last_led_changed + BLINK_DELAY)) {
        ms_last_led_changed = ms_curr;
        led_state ? led_on() : led_off();
        led_state = !led_state;  // перемкнути стан LED
      }
    }
    else
    {
      // якщо вікно блимання минуло, а звʼязку досі немає — вимкнути LED
      led_off();
    }
  }
}
#endif

void setup()
{
    Serial.begin(115200);
    boardSetup();
    crsfLinkDown();

    sendGamepadReport(true);

    // Привʼязати колбек каналів
    crsf.onPacketChannels = &packetChannels;
    crsf.onLinkUp = &crsfLinkUp;
    crsf.onLinkDown = &crsfLinkDown;
    crsf.onOobData = &crsfOobData;
    crsf.begin();
    serialEcho = true;
}

void loop()
{
    // Для обробки даних обовʼязково викликати CrsfSerial.loop() у loop()
    crsf.loop();
    checkSerialIn();

    // Поки звʼязку немає, повторно надсилаємо центральний failsafe-стан, щоб
    // хост не зберіг застарілий ввід навіть після скидання свого стану пристрою.
    if (!crsf.isLinkUp() && (millis() - ms_last_failsafe_sent > FAILSAFE_RESEND_MS))
    {
        setFailsafeState();
        ms_last_failsafe_sent = millis();
    }

    #ifdef BLINK_ENABLED
    led_loop();
    #endif
}
