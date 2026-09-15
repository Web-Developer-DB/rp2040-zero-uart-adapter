# CircuitPython release checklist

Use this short checklist before publishing a new tested CircuitPython image.

## Board test

- [ ] The CircuitPython UF2 boots on a Waveshare RP2040-Zero.
- [ ] `boot.py` and `code.py` are copied to the root of `CIRCUITPY`.
- [ ] Both USB CDC interfaces appear: console/REPL and data.
- [ ] The LED is solid blue while idle and pulses during traffic.
- [ ] UART communication works at 115200 baud, 8N1.
- [ ] The Linux or Windows terminal instructions work with a real target.
- [ ] The capture tool creates a timestamped `.raw` and `.log` pair.

## Privacy and documentation

- [ ] No files from `captures/`, `docs/private/`, or device-specific profiles
  are staged for commit.
- [ ] Logs have been checked for credentials, serial numbers, MAC addresses,
  and other identifying data.
- [ ] The UF2 filename, version, SHA-256, pinout, and README links are current.

## Publish

1. Update the firmware filename and SHA-256 in `firmware/README.md`.
2. Test the exact UF2 and Python files that will be published.
3. Create a version tag such as `v1.0.0` and attach the tested UF2 to the
   GitHub release.
