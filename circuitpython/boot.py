"""Configure the USB functions before CircuitPython connects to the host."""

import usb_cdc

# CDC console: REPL, tracebacks and diagnostics.
# CDC data:    the raw UART bridge, kept separate from the console.
usb_cdc.enable(console=True, data=True)
