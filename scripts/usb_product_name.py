# -*- coding: utf-8 -*-
"""Опційна зміна назви USB-пристрою (iProduct) під час збірки.

Ядро arduino-mbed формує назву USB-пристрою з макроса BOARD_NAME у
заголовку варіанта спільного фреймворку:

    <framework-arduino-mbed>/variants/RASPBERRY_PI_PICO/pins_arduino.h

У цього #define немає охоронця #ifndef, тому перекрити його прапорцем
-DBOARD_NAME з platformio.ini неможливо. Щоб змінити назву, не чіпаючи
спільний фреймворк (він один на всі проєкти одразу), цей скрипт під час
збірки:

  1. робить локальну копію каталогу варіанта у .pio/usb_name_variant;
  2. замінює в копії рядок BOARD_NAME на потрібну назву;
  3. вказує збірці використовувати цю копію (build.variant).

Скрипт вимкнено за замовчуванням. Щоб увімкнути його, розкоментуйте рядок
extra_scripts у секції [env] файлу platformio.ini і впишіть назву нижче.
Якщо назва порожня — скрипт нічого не робить.

УВАГА: назву бажано писати лише ASCII (латиниця, цифри, пробіли). Кирилиця
в USB-дескрипторі може відображатися некоректно.
"""

import re
import shutil
from os.path import abspath, isdir, join
from pathlib import Path

Import("env")

# ===========================================================================
# ВПИШІТЬ БАЖАНУ НАЗВУ ПРИСТРОЮ ТУТ.
# Порожній рядок ("") — нічого не змінювати (залишиться "RaspberryPi Pico").
USB_PRODUCT_NAME = ""
# ===========================================================================


def _log(msg):
    # Навмисно ASCII/англійською: консоль PlatformIO на Windows не завжди
    # коректно друкує кирилицю.
    print("[usb-name] %s" % msg)


def main():
    name = USB_PRODUCT_NAME.strip()
    if not name:
        _log("USB_PRODUCT_NAME is empty -> keeping the default USB name")
        return

    board = env.BoardConfig()
    framework_dir = env.PioPlatform().get_package_dir("framework-arduino-mbed")
    variant_name = board.get("build.variant", "")
    if not variant_name:
        _log("board has no build.variant -> nothing to do")
        return

    src = join(framework_dir, "variants", variant_name)
    if not isdir(src):
        _log("variant dir not found: %s" % src)
        return

    project_dir = env.subst("$PROJECT_DIR")
    dst = join(project_dir, ".pio", "usb_name_variant")

    # 1. Локальна копія варіанта (повторні збірки безпечні).
    shutil.copytree(src, dst, dirs_exist_ok=True)

    # 2. Підміна назви в pins_arduino.h.
    pins = Path(dst) / "pins_arduino.h"
    if not pins.is_file():
        _log("pins_arduino.h not found in the copied variant -> abort")
        return

    text = pins.read_bytes().decode("utf-8")
    escaped = name.replace("\\", "\\\\").replace('"', '\\"')
    new_text, replaced = re.subn(
        r'#define\s+BOARD_NAME\s+"[^"]*"',
        '#define BOARD_NAME "%s"' % escaped,
        text,
        count=1,
    )
    if not replaced:
        _log("BOARD_NAME not found in pins_arduino.h -> abort (framework changed?)")
        return
    pins.write_bytes(new_text.encode("utf-8"))

    # 3. Збірка використовує нашу копію: абсолютний шлях перекриває
    #    os.path.join(FRAMEWORK_DIR, "variants", build.variant).
    board.update("build.variant", abspath(dst))

    _log('USB product name set to "%s"' % name)


main()
