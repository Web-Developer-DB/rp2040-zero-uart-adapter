"""Dual-CDC USB-to-UART bridge for a Waveshare RP2040-Zero.

GP0 is the adapter TX output and GP1 is the adapter RX input. The built-in
WS2812 LED is green for RX, orange for TX and yellow for simultaneous traffic.
"""

import time

import board
import busio
import digitalio
import neopixel_write
import usb_cdc

UART_BAUDRATE = 115200
LED_PIN = board.GP16
# Treat transfers occurring within this interval as one continuous activity.
TRAFFIC_HOLD_SECONDS = 0.35
# Four pulses per second: 125 ms traffic colour followed by 125 ms blue.
TRAFFIC_BLINK_PERIOD_SECONDS = 0.25
TRAFFIC_BLINK_ON_SECONDS = 0.125

uart = busio.UART(board.GP0, board.GP1, baudrate=UART_BAUDRATE, timeout=0)
usb_data = usb_cdc.data
# Keep the bridge loop responsive while no host data is waiting. Without this,
# a blocking read can prevent status and UART processing from running.
usb_data.timeout = 0

led_pin = digitalio.DigitalInOut(LED_PIN)
led_pin.direction = digitalio.Direction.OUTPUT
led_buffer = bytearray(3)  # WS2812 byte order: green, red, blue.

last_tx = 0.0
last_rx = 0.0
last_color = None


def set_led(red, green, blue):
    """Set the one on-board WS2812. Avoid duplicate transfers."""
    global last_color
    color = (red, green, blue)
    if color == last_color:
        return
    led_buffer[0] = green
    led_buffer[1] = red
    led_buffer[2] = blue
    neopixel_write.neopixel_write(led_pin, led_buffer)
    last_color = color


def update_activity_led(now):
    """Pulse the traffic direction four times per second; blue means idle."""
    tx_active = now - last_tx < TRAFFIC_HOLD_SECONDS
    rx_active = now - last_rx < TRAFFIC_HOLD_SECONDS
    if tx_active or rx_active:
        pulse_on = (now % TRAFFIC_BLINK_PERIOD_SECONDS) < TRAFFIC_BLINK_ON_SECONDS
        if not pulse_on:
            set_led(0, 0, 32)  # Brief blue gap makes activity visibly blink.
        elif tx_active and rx_active:
            set_led(85, 85, 0)  # Yellow
        elif tx_active:
            set_led(127, 35, 0)  # Orange
        else:
            set_led(0, 127, 0)  # Green
    else:
        set_led(0, 0, 32)  # Solid blue: firmware is alive and waiting.


set_led(0, 0, 0)

while True:
    from_host = usb_data.read(64)
    if from_host:
        uart.write(from_host)
        last_tx = time.monotonic()

    from_target = uart.read(64)
    if from_target:
        usb_data.write(from_target)
        last_rx = time.monotonic()

    update_activity_led(time.monotonic())
