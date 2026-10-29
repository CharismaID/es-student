import time
from datetime import datetime

import serial
from serial.tools import list_ports

VENDOR_ID = 0x2E8A
PRODUCT_ID = 0x000A

TASK = "2.1.3"
PROJECT = "211-command-usb"
LOG_NAME = "device-2-1-3.log"
COMMAND = "mem_info"
ANSWER_S = 2.0


def find_board():
    for port in list_ports.comports():
        if port.vid == VENDOR_ID and port.pid == PRODUCT_ID:
            return port
    return None


def ask_memory(board):
    exchange = []
    with serial.Serial(board.device, timeout=0.1) as port:
        time.sleep(0.2)
        port.reset_input_buffer()
        started = time.monotonic()
        port.write((COMMAND + "\n").encode("ascii"))
        exchange.append((time.monotonic() - started, "-->", COMMAND))
        print("--> " + COMMAND, end="\r\n")
        silence_start = time.monotonic()
        while time.monotonic() - silence_start < ANSWER_S:
            line = port.readline().decode("ascii", "replace").strip()
            if line:
                exchange.append((time.monotonic() - started, "<--", line))
                print("<-- " + line, end="\r\n")
                silence_start = time.monotonic()
    return exchange


def write_log(board, exchange):
    with open(LOG_NAME, "w", encoding="utf-8") as log:
        log.write("задание: " + TASK + "\n")
        log.write("проект: " + PROJECT + "\n")
        log.write("устройство: %04x:%04x\n" % (board.vid, board.pid))
        log.write("серийный номер: " + str(board.serial_number) + "\n")
        log.write("порт: " + board.device + "\n")
        log.write("начало: " + datetime.now().isoformat(timespec="seconds") + "\n")
        for moment, direction, text in exchange:
            log.write("%8.3f %s %s\n" % (moment, direction, text))
        received_count = sum(1 for _, d, _ in exchange if d == "<--")
        log.write("итог: принято строк %d\n" % received_count)


board = find_board()

if board is None:
    print("Плата не найдена. Проверьте кабель и запишите на плату прошивку задания.", end="\r\n")
else:
    print("Плата на порту " + board.device + ", спрашиваю память", end="\r\n")
    exchange = ask_memory(board)
    write_log(board, exchange)
    print("Ответ записан в " + LOG_NAME, end="\r\n")
