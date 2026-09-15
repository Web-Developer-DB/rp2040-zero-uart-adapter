# Fehlerhilfe

## Es erscheint nur ein serieller Port

Nach einem Firmwarewechsel das Board einmal ab- und wieder anstecken. Prüfe
anschließend die USB-Interfaces statt nur die Portnummer:

```sh
ls -l /dev/serial/by-id/
dmesg | tail -n 30
```

Bei CircuitPython müssen `boot.py` und `code.py` im Wurzelverzeichnis von
`CIRCUITPY` liegen; `boot.py` wird nur beim Start ausgeführt. Nach Änderungen
an `boot.py` deshalb neu starten oder USB trennen.

## Ubuntu: `Permission denied` beim Öffnen von `/dev/ttyACM*`

Ubuntu weist USB-Seriellgeräte üblicherweise der Gruppe `dialout` zu. Deshalb
einmalig das eigene Konto hinzufügen:

```sh
sudo usermod -aG dialout "$USER"
```

Danach vollständig ab- und wieder anmelden oder den Rechner neu starten. Ein
neu geöffnetes Terminal allein reicht nicht aus. Mit diesem Befehl prüfen, ob
die neue Sitzung die Gruppe kennt:

```sh
groups
```

`dialout` muss in der Ausgabe stehen. Danach kann der Recorder ohne `sudo`
gestartet werden. Für eine sofortige einzelne Aufnahme ist auch ein
`sudo python3 tools/capture_uart.py ...` möglich: Der Recorder übergibt die
neu erzeugten `.raw`- und `.log`-Dateien **sowie den Ordner `captures/`**
danach automatisch wieder an den Desktop-Nutzer, der `sudo` aufgerufen hat.
Der Ordner erhält normale Rechte `755`, die Dateien `644`.

Falls eine alte Aufzeichnung aus einer früheren Programmversion durch einen
`sudo`-Aufruf root-eigen ist, lasse ihre Eigentümerschaft nicht unbesehen
rekursiv ändern. Verschiebe oder sichere die betreffende Datei gezielt und
prüfe sie vor dem weiteren Teilen.

## Richtigen CircuitPython-Port auswählen

CircuitPython stellt eine REPL-Konsole und eine Daten-Schnittstelle bereit.
Für die Aufzeichnung ist ausschließlich die Daten-Schnittstelle vorgesehen.
Nach Einstecken oder Neustart helfen diese Befehle beim Erkennen der Ports:

```sh
ls -l /dev/serial/by-id/
dmesg | tail -n 30
```

Die Nummern `ttyACM0` und `ttyACM1` können sich auf anderen Rechnern oder nach
einem Neustart ändern. Daher möglichst den passenden Namen unter
`/dev/serial/by-id/` verwenden und nicht dauerhaft eine feste `ttyACM`-Nummer
in Skripten speichern.

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
4. Sicherstellen, dass der geöffnete USB-Port die CircuitPython-Datenschnittstelle
   `usb_cdc.data` ist, nicht die REPL-Konsole.
5. Prüfen, ob das Zielgerät tatsächlich 3,3-V-TTL und nicht RS-232 oder 5-V-TTL
   verwendet.

## LED zeigt keine Aktivität

Beim Waveshare RP2040-Zero liegt die WS2812-Datenleitung auf GP16. Bei einem
anderen Board muss `LED_PIN` in `circuitpython/code.py` angepasst werden. Die
LED zeigt den tatsächlich übertragenen Verkehr an, nicht bloß das Öffnen eines
Ports. Ohne Verkehr leuchtet sie dauerhaft blau: Die Firmware läuft und wartet
auf UART-Daten. Bei Verkehr pulsiert die zugehörige Farbe viermal pro Sekunde.

## CircuitPython startet im Safe Mode

Zu viele gleichzeitig aktivierte USB-Funktionen können die verfügbaren
Endpunkte überschreiten. Diese Projektdatei aktiviert genau Konsole und
Daten-CDC. Falls die CircuitPython-Version oder ein angepasstes `boot.py`
weitere USB-Funktionen einschaltet, diese zunächst deaktivieren.
