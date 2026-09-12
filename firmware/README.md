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

## Optional C++ firmware

The alternative Pico SDK C++ implementation is built by GitHub Actions.
Pushing a version tag such as `v1.0.0` creates a GitHub Release and attaches
`rp2040_zero_uart_adapter.uf2`. Do not create a tag until that firmware has
been tested on a real board; use the [release checklist](../docs/release-checklist.md).
