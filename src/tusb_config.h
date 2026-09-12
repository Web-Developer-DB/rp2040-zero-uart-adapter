#ifndef RP2040_ZERO_UART_ADAPTER_TUSB_CONFIG_H
#define RP2040_ZERO_UART_ADAPTER_TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_TUD_ENABLED 1
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT 0
#endif

// Two independent virtual serial ports: CDC 0 for diagnostics and CDC 1 for
// the UART bridge.
#define CFG_TUD_CDC 2
#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256
#define CFG_TUD_CDC_EP_BUFSIZE 64

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#endif

#ifdef __cplusplus
}
#endif

#endif
