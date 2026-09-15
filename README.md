# RP2040-Zero CircuitPython UART Adapter

> 🇬🇧 **English** · [🇩🇪 Deutsch](#deutsch)

![Board](https://img.shields.io/badge/board-Waveshare%20RP2040--Zero-5a2d82)
![UART](https://img.shields.io/badge/UART-3.3%20V%20TTL-important)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A small USB-to-TTL-UART adapter for the Waveshare RP2040-Zero. CircuitPython
is the only board firmware in this project: flash one UF2, copy two files, and
use the adapter as a transparent 3.3 V serial console.

Made for makers, embedded developers, repair technicians, and anyone who needs
a quick UART console for a router, microcontroller, single-board computer, or
other embedded device.

## Why the RP2040-Zero instead of a classic Arduino?

The RP2040's GPIO and UART use **3.3 V logic**, which matches the 3.3 V TTL
levels used by many modern embedded targets. Classic Arduino boards such as an
Uno or Nano commonly use 5 V logic; connecting their UART directly can damage
a 3.3 V target or the RP2040. A 5 V Arduino therefore needs a level shifter.
Some Arduino boards are 3.3 V versions, so always check the exact board and
its voltage levels before wiring it.

The RP2040-Zero also provides native USB and runs the complete bridge from
CircuitPython, so no compiled firmware toolchain is needed for this project.

> [!WARNING]
> This is **3.3 V TTL only**. Never connect it directly to 5 V TTL or classic
> RS-232 (often ±12 V). Use a suitable level shifter or RS-232 transceiver.

## 🚀 Quick start

1. Hold **BOOT** on the RP2040-Zero and connect it by USB. The `RPI-RP2` drive
   appears.
2. Copy [`firmware/adafruit-circuitpython-waveshare_rp2040_zero-en_US-10.3.0.uf2`](firmware/adafruit-circuitpython-waveshare_rp2040_zero-en_US-10.3.0.uf2)
   to `RPI-RP2`.
3. Wait for the `CIRCUITPY` drive to appear.
4. Copy [`circuitpython/boot.py`](circuitpython/boot.py) and
   [`circuitpython/code.py`](circuitpython/code.py) to the root of `CIRCUITPY`.
5. Reconnect the board. Solid blue means the bridge is running and waiting.

```text
CIRCUITPY/
├── boot.py
└── code.py
```

The board exposes two USB CDC ports: `usb_cdc.console` for the REPL and
diagnostics, and `usb_cdc.data` for UART traffic. Use the data port with your
terminal program.

## 🧰 Wiring

| RP2040-Zero | Function | Target device |
| --- | --- | --- |
| `GP0` | UART0 TX — adapter sends | RX |
| `GP1` | UART0 RX — adapter receives | TX |
| `GND` | Signal reference | GND |

TX and RX are crossed. GND is required. See the [wiring guide](docs/wiring.md)
and the [wiring diagram](docs/wiring.svg).

![RP2040-Zero UART wiring](docs/wiring.svg)

### LED status

| LED | Meaning |
| --- | --- |
| Solid blue | Firmware is running and idle |
| Orange pulse | USB → target traffic |
| Green pulse | Target → USB traffic |
| Yellow pulse | Recent traffic in both directions |

## 🖥️ Use it as a terminal

Use the **data** port only. Text received from the target appears in the
terminal; keys you type are sent to the target through `GP0`. For a safe
receive-only console, leave `GP0` disconnected and connect only target TX →
`GP1` plus GND.

### Linux

Install `picocom` once and open the usual CircuitPython data port:

~~~sh
sudo apt install picocom
sudo picocom -b 115200 /dev/ttyACM1
~~~

Exit picocom with <kbd>Ctrl</kbd>+<kbd>A</kbd>, then <kbd>Ctrl</kbd>+<kbd>X</kbd>.
If the device is not `ttyACM1`, list available ports with
`ls -l /dev/serial/by-id/` and choose the `usb_cdc.data` interface.

### Windows

1. Open **Device Manager → Ports (COM & LPT)** and identify the board's data
   COM port by unplugging and reconnecting it.
2. Open [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/), select
   **Serial**, enter `COMx`, and set the speed to `115200`.
3. Under **Connection → Serial**, use 8 data bits, 1 stop bit, no parity, and
   no flow control.
4. Choose **Open**. Close the window to end the terminal session.

## 📟 Record a boot log (Linux)

Use the separate [capture tool](tools/capture_uart.py). It writes exact bytes
and a readable log; date and time including seconds are added to every name.
Start it before powering the target:

~~~sh
sudo python3 tools/capture_uart.py /dev/ttyACM1
~~~

Wait for `Power-cycle the target now`, power-cycle the target, then press
<kbd>Ctrl</kbd>+<kbd>C</kbd>. The files are saved under `captures/` and are not
overwritten by the next capture. See [troubleshooting](docs/troubleshooting.md)
for Ubuntu permissions and port selection.

## ⚙️ Adjust the CircuitPython program

Edit `circuitpython/code.py` before copying it to the board:

| Setting | Default | Purpose |
| --- | --- | --- |
| `UART_BAUDRATE` | `115200` | UART speed |
| `LED_PIN` | `board.GP16` | On-board WS2812 data pin |

`circuitpython/boot.py` enables the console and data USB CDC interfaces.

## 🔒 Privacy

`captures/` and `docs/private/` are ignored by Git. UART logs may contain
serial numbers, MAC addresses, credentials, or other private data. Review a
log before sharing it.

## 🛟 Help and license

See [docs/troubleshooting.md](docs/troubleshooting.md) for permission, USB,
port, wiring, and CircuitPython Safe Mode problems. The project is released
under the [MIT License](LICENSE).

---

<a id='deutsch'></a>

# RP2040-Zero CircuitPython-UART-Adapter

> [🇬🇧 English](#rp2040-zero-circuitpython-uart-adapter) · **🇩🇪 Deutsch**

Ein kleiner USB-zu-TTL-UART-Adapter für den Waveshare RP2040-Zero. Dieses
Projekt verwendet ausschließlich CircuitPython als Board-Firmware: eine UF2
flashen, zwei Dateien kopieren und sofort eine transparente 3,3-V-UART-Konsole
verwenden.

Geeignet für Maker, Embedded-Entwickler, Reparaturtechniker und alle, die
schnell eine UART-Konsole für Router, Mikrocontroller, Single-Board-Computer
oder andere Embedded-Geräte benötigen.

## Warum RP2040-Zero statt eines klassischen Arduino?

Die GPIOs und UARTs des RP2040 arbeiten mit **3,3-V-Logikpegeln**. Das passt
zu den 3,3-V-TTL-Pegeln vieler moderner Embedded-Zielgeräte. Klassische
Arduino-Boards wie Uno oder Nano verwenden dagegen häufig 5-V-Logik. Eine
direkte UART-Verbindung kann dann ein 3,3-V-Zielgerät oder den RP2040
beschädigen; bei einem 5-V-Arduino ist ein Pegelwandler erforderlich.
Es gibt auch Arduino-Boards mit 3,3 V. Deshalb vor dem Verdrahten immer das
genaue Board und seine Pegel prüfen.

Der RP2040-Zero bietet außerdem natives USB und führt die komplette Bridge mit
CircuitPython aus. Für dieses Projekt ist daher keine kompilierte
Firmware-Toolchain nötig.

> [!WARNING]
> Nur **3,3-V-TTL** anschließen. Niemals direkt 5-V-TTL oder klassisches
> RS-232 (oft ±12 V) verbinden. Dafür einen Pegelwandler verwenden.

## 🚀 Schnellstart

1. **BOOT** am RP2040-Zero gedrückt halten und das Board per USB verbinden.
   Das Laufwerk `RPI-RP2` erscheint.
2. [`firmware/adafruit-circuitpython-waveshare_rp2040_zero-en_US-10.3.0.uf2`](firmware/adafruit-circuitpython-waveshare_rp2040_zero-en_US-10.3.0.uf2)
   auf `RPI-RP2` kopieren.
3. Warten, bis das Laufwerk `CIRCUITPY` erscheint.
4. [`circuitpython/boot.py`](circuitpython/boot.py) und
   [`circuitpython/code.py`](circuitpython/code.py) in das Wurzelverzeichnis
   von `CIRCUITPY` kopieren.
5. Das Board neu verbinden. Dauerhaftes Blau bedeutet: Die Bridge läuft und
   wartet auf Daten.

`usb_cdc.console` ist REPL und Diagnose; `usb_cdc.data` ist ausschließlich die
UART-Daten-Schnittstelle. Für das Terminal immer `usb_cdc.data` verwenden.

## 🧰 Verdrahtung

| RP2040-Zero | Funktion | Zielgerät |
| --- | --- | --- |
| `GP0` | UART0 TX — Adapter sendet | RX |
| `GP1` | UART0 RX — Adapter empfängt | TX |
| `GND` | Bezugsmasse | GND |

TX und RX werden gekreuzt verbunden; GND ist erforderlich. Siehe die
[Verdrahtungsanleitung](docs/wiring.md) und das [Verdrahtungsbild](docs/wiring.svg).

![UART-Verdrahtung](docs/wiring.svg)

### LED-Status

| LED | Bedeutung |
| --- | --- |
| Dauerhaft blau | Firmware läuft und wartet |
| Orange, Puls | USB → Zielgerät |
| Grün, Puls | Zielgerät → USB |
| Gelb, Puls | Datenverkehr in beide Richtungen |

## 🖥️ Als Terminal verwenden

Den **Datenport** verwenden. Empfangener Text erscheint im Terminal; getippte
Zeichen werden über `GP0` zum Zielgerät gesendet. Für reinen Empfang `GP0`
offen lassen und nur Ziel-TX → `GP1` sowie GND verbinden.

### Linux

`picocom` einmal installieren und den üblichen CircuitPython-Datenport öffnen:

~~~sh
sudo apt install picocom
sudo picocom -b 115200 /dev/ttyACM1
~~~

Picocom mit <kbd>Ctrl</kbd>+<kbd>A</kbd>, anschließend <kbd>Ctrl</kbd>+<kbd>X</kbd>
beenden. Falls der Port nicht `ttyACM1` ist, mit
`ls -l /dev/serial/by-id/` den `usb_cdc.data`-Port suchen.

### Windows

1. **Geräte-Manager → Anschlüsse (COM & LPT)** öffnen und den Daten-COM-Port
   durch Ab- und Anstecken des Boards erkennen.
2. [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/) öffnen,
   **Serial** wählen, `COMx` eintragen und **Speed** auf `115200` setzen.
3. Unter **Connection → Serial** 8 Datenbits, 1 Stoppbit, keine Parität und
   keine Flusskontrolle wählen.
4. **Open** wählen. Zum Beenden das Fenster schließen.

## 📟 Boot-Log aufzeichnen (Linux)

Das separate [Aufzeichnungswerkzeug](tools/capture_uart.py) schreibt exakte
Bytes und ein lesbares Log. Datum, Uhrzeit und Sekunden werden jedem Namen
automatisch angehängt. Vor dem Einschalten des Zielgeräts starten:

~~~sh
sudo python3 tools/capture_uart.py /dev/ttyACM1
~~~

Auf `Power-cycle the target now` warten, Zielgerät aus- und einschalten und
danach <kbd>Ctrl</kbd>+<kbd>C</kbd> drücken. Die Dateien liegen unter
`captures/` und werden bei der nächsten Aufnahme nicht überschrieben. Die
[Fehlerhilfe](docs/troubleshooting.md) erklärt Ubuntu-Berechtigungen und
Portauswahl.

## ⚙️ CircuitPython-Programm anpassen

Vor dem Kopieren auf das Board kann `circuitpython/code.py` geändert werden:

| Einstellung | Standard | Zweck |
| --- | --- | --- |
| `UART_BAUDRATE` | `115200` | UART-Geschwindigkeit |
| `LED_PIN` | `board.GP16` | Datenpin der WS2812 auf dem Board |

`circuitpython/boot.py` aktiviert die USB-CDC-Konsole und den Datenport.

## 🔒 Datenschutz

`captures/` und `docs/private/` sind in Git ignoriert. UART-Logs können
Seriennummern, MAC-Adressen, Zugangsdaten oder andere private Daten enthalten.
Vor dem Teilen jedes Log prüfen.

## 🛟 Hilfe und Lizenz

Bei Problemen mit Berechtigungen, USB, Ports, Verdrahtung oder CircuitPython
Safe Mode hilft [docs/troubleshooting.md](docs/troubleshooting.md). Das Projekt
steht unter der [MIT-Lizenz](LICENSE).
