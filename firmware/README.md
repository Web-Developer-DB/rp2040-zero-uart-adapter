# Firmware

## Primary installation: CircuitPython

`adafruit-circuitpython-waveshare_rp2040_zero-en_US-10.3.0.uf2` is the tested
CircuitPython image used by this project for the **Waveshare RP2040-Zero**.
Flash this file first, then copy `boot.py` and `code.py` from the repository's
`circuitpython/` directory to the `CIRCUITPY` drive. The complete procedure is
at the start of the [project README](../README.md).

SHA-256:

```text
b268f11ca9ce94660c27beaea0b9e71782b60a97dce5cbeecac8702ac6af92a3
```

## Files installed on the board

After flashing the UF2, copy these two project files to the root of the
`CIRCUITPY` drive:

```text
CIRCUITPY/
├── boot.py
└── code.py
```

The [project README](../README.md) explains the complete installation and the
terminal connection. Use the [release checklist](../docs/release-checklist.md)
when publishing a new tested CircuitPython image.
