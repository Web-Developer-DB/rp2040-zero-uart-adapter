# Release firmware

This directory documents release artifacts. The C++ UF2 is built by the GitHub
Actions workflow; pushing a version tag such as `v1.0.0` creates a GitHub
Release and attaches `rp2040_zero_uart_adapter.uf2`.

Do not create a version tag until the generated UF2 has been tested on a real
Waveshare RP2040-Zero. See [the release checklist](../docs/release-checklist.md).
