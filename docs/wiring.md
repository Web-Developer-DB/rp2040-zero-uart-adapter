# Verdrahtung

![Platinenansicht und UART-Verdrahtung](wiring.svg)

## Anschlüsse

| RP2040-Zero | UART-Funktion | Verbindung am Zielgerät |
| --- | --- | --- |
| `GP0` | UART0 `TX` | `RX` |
| `GP1` | UART0 `RX` | `TX` |
| `GND` | Bezugsmasse | `GND` |

Die Signalleitungen werden über Kreuz angeschlossen: **TX zu RX** und **RX zu
TX**. Ohne die gemeinsame Masse kann UART nicht zuverlässig funktionieren.

Die Firmware verwendet `UART0` auf GP0/GP1 mit **115200 Baud, 8 Datenbits,
keiner Parität und einem Stoppbit (8N1)**. Die auf der Platine integrierte
WS2812-RGB-LED verwendet `GP16`; sie muss nicht verdrahtet werden.

> [!WARNING]
> Die Pins des RP2040-Zero arbeiten mit 3,3-V-Logik. Dieses Projekt ist nicht
> direkt mit klassischem RS-232 (oft ±12 V) und auch nicht mit einer 5-V-TTL-
> UART kompatibel. Verwende in diesen Fällen einen passenden Pegelwandler.

## Quellen zur Pinbelegung

Die Grafik ist eine eigenständige technische Zeichnung, erstellt anhand der
offiziellen Waveshare-Unterlagen:

- [RP2040-Zero Produktseite](https://www.waveshare.com/product/rp2040-zero.htm)
- [RP2040-Zero Wiki und Ressourcen](https://www.waveshare.com/wiki/RP2040-Zero)
- [offizieller Schaltplan (PDF)](https://files.waveshare.com/upload/4/4c/RP2040_Zero.pdf)

Dadurch kann die Grafik im Repository ohne ein fremdes Produktfoto verwendet
werden und zeigt nur die für diesen Adapter relevanten Verbindungen.
