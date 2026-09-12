#!/usr/bin/env python3
"""Record a boot log from the RP2040-Zero UART bridge on Linux.

Creates two files from the same byte stream:
  <prefix>.raw  Exact bytes, useful for binary data or later decoding.
  <prefix>.log  Human-readable chunks with local timestamps.
"""

import argparse
import datetime as dt
import os
from pathlib import Path
import select
import sys
import termios
import time


BAUD_RATES = {
    9600: termios.B9600,
    19200: termios.B19200,
    38400: termios.B38400,
    57600: termios.B57600,
    115200: termios.B115200,
}


def configure_port(port: str, baudrate: int) -> int:
    """Open port in raw 8N1 mode without changing flow-control lines."""
    if baudrate not in BAUD_RATES:
        supported = ", ".join(str(rate) for rate in BAUD_RATES)
        raise ValueError(f"Unsupported baud rate {baudrate}; use: {supported}")

    descriptor = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    settings = termios.tcgetattr(descriptor)
    settings[0] = 0  # input flags
    settings[1] = 0  # output flags
    settings[2] = termios.CLOCAL | termios.CREAD | termios.CS8
    settings[3] = 0  # local flags
    settings[4] = BAUD_RATES[baudrate]
    settings[5] = BAUD_RATES[baudrate]
    settings[6][termios.VMIN] = 0
    settings[6][termios.VTIME] = 0
    termios.tcsetattr(descriptor, termios.TCSANOW, settings)
    return descriptor


def timestamp() -> str:
    return dt.datetime.now().astimezone().isoformat(timespec="milliseconds")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Record data arriving through the RP2040-Zero UART bridge."
    )
    parser.add_argument(
        "port",
        help="UART bridge port; prefer a stable /dev/serial/by-id/ path on Linux",
    )
    parser.add_argument(
        "--baud", type=int, default=115200, help="UART baud rate (default: 115200)"
    )
    parser.add_argument(
        "--prefix",
        type=Path,
        default=Path("captures/uart-boot"),
        help="Output path without extension (default: captures/uart-boot)",
    )
    parser.add_argument(
        "--duration",
        type=float,
        default=0,
        help="Stop automatically after this many seconds; 0 waits for Ctrl-C",
    )
    args = parser.parse_args()

    raw_path = args.prefix.with_suffix(".raw")
    log_path = args.prefix.with_suffix(".log")
    raw_path.parent.mkdir(parents=True, exist_ok=True)

    try:
        descriptor = configure_port(args.port, args.baud)
    except (OSError, ValueError) as error:
        print(f"Cannot open {args.port}: {error}", file=sys.stderr)
        return 2

    started_at = time.monotonic()
    header = (
        f"# RP2040-Zero UART boot capture\n"
        f"# Started: {timestamp()}\n"
        f"# Port: {args.port}; UART: {args.baud} baud, 8N1\n\n"
    )

    print(f"Recording {args.port} at {args.baud} baud.")
    print(f"Raw:  {raw_path}")
    print(f"Log:  {log_path}")
    print("Power-cycle the target now. Press Ctrl-C to stop recording.")

    try:
        with raw_path.open("wb") as raw_file, log_path.open(
            "w", encoding="utf-8", newline=""
        ) as log_file:
            log_file.write(header)
            pending_text = b""
            while args.duration <= 0 or time.monotonic() - started_at < args.duration:
                ready, _, _ = select.select([descriptor], [], [], 0.2)
                if not ready:
                    continue
                chunk = os.read(descriptor, 4096)
                if not chunk:
                    continue
                raw_file.write(chunk)
                raw_file.flush()
                # USB CDC delivers arbitrary-sized chunks. Timestamp complete
                # lines instead of each chunk, otherwise one boot line becomes
                # visually fragmented in the text log.
                pending_text += chunk
                while b"\n" in pending_text:
                    line, pending_text = pending_text.split(b"\n", 1)
                    rendered_line = line.rstrip(b"\r").decode(
                        "utf-8", errors="backslashreplace"
                    )
                    log_file.write(f"[{timestamp()}] {rendered_line}\n")
                log_file.flush()
                sys.stdout.write(chunk.decode("utf-8", errors="backslashreplace"))
                sys.stdout.flush()
            if pending_text:
                rendered_line = pending_text.rstrip(b"\r").decode(
                    "utf-8", errors="backslashreplace"
                )
                log_file.write(f"[{timestamp()}] {rendered_line}\n")
    except KeyboardInterrupt:
        print("\nCapture stopped by user.")
    finally:
        os.close(descriptor)

    print(f"Saved raw data to {raw_path} and readable log to {log_path}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
