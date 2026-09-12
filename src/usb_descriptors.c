#include <string.h>

#include <bsp/board_api.h>
#include <tusb.h>

#define USB_VENDOR_ID 0xCAFE  // Development-only example VID/PID pair.
#define USB_PRODUCT_ID 0x4002

enum StringId {
    STRING_ID_LANGUAGE = 0,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    STRING_ID_SERIAL,
    STRING_ID_DEBUG_CDC,
    STRING_ID_UART_CDC,
};

char const *const STRING_DESCRIPTORS[] = {
    (const char[]) {0x09, 0x04},
    "RP2040 Zero UART Adapter",
    "RP2040 Zero Dual CDC UART Adapter",
    NULL,
    "Debug console",
    "UART bridge",
};

enum InterfaceNumber {
    INTERFACE_DEBUG_CDC = 0,
    INTERFACE_DEBUG_CDC_DATA,
    INTERFACE_UART_CDC,
    INTERFACE_UART_CDC_DATA,
    INTERFACE_COUNT,
};

#define DEBUG_NOTIFICATION_EP 0x81
#define DEBUG_OUT_EP 0x02
#define DEBUG_IN_EP 0x82
#define UART_NOTIFICATION_EP 0x84
#define UART_OUT_EP 0x05
#define UART_IN_EP 0x85

#define CONFIGURATION_LENGTH (TUD_CONFIG_DESC_LEN + CFG_TUD_CDC * TUD_CDC_DESC_LEN)

tusb_desc_device_t const DEVICE_DESCRIPTOR = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VENDOR_ID,
    .idProduct = USB_PRODUCT_ID,
    .bcdDevice = 0x0100,
    .iManufacturer = STRING_ID_MANUFACTURER,
    .iProduct = STRING_ID_PRODUCT,
    .iSerialNumber = STRING_ID_SERIAL,
    .bNumConfigurations = 1,
};

uint8_t const CONFIGURATION_DESCRIPTOR[] = {
    TUD_CONFIG_DESCRIPTOR(1, INTERFACE_COUNT, 0, CONFIGURATION_LENGTH,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_CDC_DESCRIPTOR(INTERFACE_DEBUG_CDC, STRING_ID_DEBUG_CDC,
                       DEBUG_NOTIFICATION_EP, 8, DEBUG_OUT_EP, DEBUG_IN_EP, 64),
    TUD_CDC_DESCRIPTOR(INTERFACE_UART_CDC, STRING_ID_UART_CDC,
                       UART_NOTIFICATION_EP, 8, UART_OUT_EP, UART_IN_EP, 64),
};

uint16_t STRING_BUFFER[32 + 1];

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&DEVICE_DESCRIPTOR;
}

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return CONFIGURATION_DESCRIPTOR;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    if (index == STRING_ID_LANGUAGE) {
        STRING_BUFFER[1] = 0x0409;
        STRING_BUFFER[0] = (TUSB_DESC_STRING << 8) | 4;
        return STRING_BUFFER;
    }

    size_t character_count = 0;
    if (index == STRING_ID_SERIAL) {
        character_count = board_usb_get_serial(STRING_BUFFER + 1, 32);
    } else {
        if (index >= TU_ARRAY_SIZE(STRING_DESCRIPTORS)) {
            return NULL;
        }
        char const *string = STRING_DESCRIPTORS[index];
        if (string == NULL) {
            return NULL;
        }
        character_count = strlen(string);
        if (character_count > 32) {
            character_count = 32;
        }
        for (size_t i = 0; i < character_count; ++i) {
            STRING_BUFFER[1 + i] = string[i];
        }
    }

    STRING_BUFFER[0] = (TUSB_DESC_STRING << 8) | (character_count * 2 + 2);
    return STRING_BUFFER;
}
