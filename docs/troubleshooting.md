# Fehlerhilfe

## Es erscheint nur ein serieller Port

Nach einem Firmwarewechsel das Board einmal ab- und wieder anstecken. Prüfe
anschließend die USB-Interfaces statt nur die Portnummer:

```sh
ls -l /dev/serial/by-id/
dmesg | tail -n 30
```

Die C++-Firmware soll zwei Interfaces mit den Namen `Debug console` und
`UART bridge` bereitstellen. Bei CircuitPython müssen `boot.py` und `code.py`
im Wurzelverzeichnis von `CIRCUITPY` liegen; `boot.py` wird nur beim Start
ausgeführt. Nach Änderungen an `boot.py` deshalb neu starten oder USB trennen.

## Die REPL ist nicht erreichbar

Nur CircuitPython besitzt eine REPL. Verbinde dich mit dem Port
`usb_cdc.console`, nicht mit dem Datenport. Ein Fehler in `code.py` wird als
Traceback über die Konsole ausgegeben und blockiert die Diagnose-Schnittstelle
nicht.

## Keine UART-Daten

1. Masse verbinden: `GND` des RP2040-Zero mit `GND` des Zielgeräts.
2. Datenleitungen kreuzen: `GP0 → RX` und `GP1 ← TX`.
3. Bei beiden Seiten dieselben UART-Einstellungen setzen, standardmäßig
   115200 Baud, 8 Datenbits, keine Parität, 1 Stoppbit.
4. Sicherstellen, dass der geöffnete USB-Port `UART bridge` ist.
5. Prüfen, ob das Zielgerät tatsächlich 3,3-V-TTL und nicht RS-232 oder 5-V-TTL
   verwendet.

## LED zeigt keine Aktivität

Beim Waveshare RP2040-Zero liegt die WS2812-Datenleitung auf GP16. Bei einem
anderen Board muss `RGB_LED_PIN` in `src/main.cpp` beziehungsweise `LED_PIN`
in `circuitpython/code.py` angepasst werden. Die LED zeigt den tatsächlich
übertragenen Verkehr an, nicht bloß das Öffnen eines Ports. Ohne Verkehr
leuchtet sie dauerhaft blau: Die Firmware läuft und wartet auf UART-Daten.
Bei Verkehr pulsiert die zugehörige Farbe viermal pro Sekunde.

## CircuitPython startet im Safe Mode

Zu viele gleichzeitig aktivierte USB-Funktionen können die verfügbaren
Endpunkte überschreiten. Diese Projektdatei aktiviert genau Konsole und
Daten-CDC. Falls die CircuitPython-Version oder ein angepasstes `boot.py`
weitere USB-Funktionen einschaltet, diese zunächst deaktivieren.
