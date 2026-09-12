# Release checklist

Use this short checklist before creating a version tag. It keeps a release
reproducible, testable, and free of private target-device data.

## Firmware checks

- [ ] The GitHub Actions build completes without warnings that affect the UF2.
- [ ] The UF2 starts on a Waveshare RP2040-Zero.
- [ ] Both USB serial interfaces enumerate with the labels **Debug console**
  and **UART bridge**.
- [ ] Blue is solid while idle; orange, green, and yellow activity pulses work
  in the expected directions.
- [ ] A 115200-baud loopback or known-good target test passes in both
  directions.

## Documentation and privacy checks

- [ ] Pinout, voltage warning, installation instructions, and README links are
  current.
- [ ] No contents from `captures/`, `docs/private/`, or device-specific
  profiles are staged for commit.
- [ ] Logs have been reviewed for credentials, serial numbers, MAC addresses,
  and other identifying data.
- [ ] The release notes name tested hardware and any known limitations.

## Publish

1. Update the version notes as appropriate.
2. Create and push a tag in the form `vMAJOR.MINOR.PATCH`, for example
   `v1.0.0`.
3. The workflow builds the UF2 and publishes it to the corresponding GitHub
   Release. Verify the release asset after the workflow completes.
