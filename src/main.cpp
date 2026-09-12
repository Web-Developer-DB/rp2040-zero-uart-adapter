#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <bsp/board_api.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <tusb.h>

#include "ws2812.pio.h"

namespace {

constexpr uint8_t DEBUG_CDC = 0;
constexpr uint8_t UART_CDC = 1;

constexpr uint UART_TX_PIN = 0;
constexpr uint UART_RX_PIN = 1;
constexpr uint UART_BAUD_RATE = 115200;

// Waveshare's RP2040-Zero routes its on-board WS2812 to GP16.
constexpr uint RGB_LED_PIN = 16;
constexpr uint32_t TRAFFIC_HOLD_MS = 350;
constexpr uint32_t TRAFFIC_BLINK_PERIOD_MS = 250;
constexpr uint32_t TRAFFIC_BLINK_ON_MS = 125;

constexpr uint32_t COLOR_OFF = 0x000000;
constexpr uint32_t COLOR_TX = 0x7F2300;  // Orange: USB -> target UART.
constexpr uint32_t COLOR_RX = 0x007F00;  // Green: target UART -> USB.
constexpr uint32_t COLOR_BOTH = 0x555500; // Yellow: both directions recently active.
constexpr uint32_t COLOR_IDLE = 0x000020; // Blue: bridge is alive and waiting.

template <size_t Capacity>
class ByteRing {
  public:
    bool push(uint8_t value) {
        if (size_ == Capacity) {
            return false;
        }
        data_[head_] = value;
        head_ = (head_ + 1) % Capacity;
        ++size_;
        return true;
    }

    bool pop(uint8_t *value) {
        if (size_ == 0) {
            return false;
        }
        *value = data_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --size_;
        return true;
    }

    [[nodiscard]] bool empty() const { return size_ == 0; }
    [[nodiscard]] size_t size() const { return size_; }
    [[nodiscard]] size_t free() const { return Capacity - size_; }

  private:
    std::array<uint8_t, Capacity> data_{};
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t size_ = 0;
};

ByteRing<2048> usb_to_uart;
ByteRing<2048> uart_to_usb;

PIO led_pio = pio0;
uint led_state_machine;
uint32_t last_tx_activity = 0;
uint32_t last_rx_activity = 0;
bool has_tx_activity = false;
bool has_rx_activity = false;
uint32_t last_led_color = COLOR_OFF;

void put_pixel(uint32_t rgb) {
    // The PIO program transmits GRB, MSB first.
    uint32_t grb = ((rgb & 0x00FF00) << 8) | ((rgb & 0xFF0000) >> 8) |
                   (rgb & 0x0000FF);
    pio_sm_put_blocking(led_pio, led_state_machine, grb << 8u);
}

void initialise_ws2812_program(PIO pio, uint state_machine, uint offset,
                               uint data_pin, uint32_t bit_rate) {
    pio_sm_config config = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&config, data_pin);
    sm_config_set_out_shift(&config, false, true, 24);
    sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);

    const float clock_divider =
        static_cast<float>(clock_get_hz(clk_sys)) /
        (static_cast<float>(bit_rate) * (ws2812_T1 + ws2812_T2 + ws2812_T3));
    sm_config_set_clkdiv(&config, clock_divider);

    pio_gpio_init(pio, data_pin);
    pio_sm_set_consecutive_pindirs(pio, state_machine, data_pin, 1, true);
    pio_sm_init(pio, state_machine, offset, &config);
    pio_sm_set_enabled(pio, state_machine, true);
}

void initialise_led() {
    uint offset = pio_add_program(led_pio, &ws2812_program);
    led_state_machine = pio_claim_unused_sm(led_pio, true);
    initialise_ws2812_program(led_pio, led_state_machine, offset, RGB_LED_PIN,
                              800000);
    // Blue immediately confirms that the firmware is alive, even before the
    // first USB or UART byte has been transferred.
    put_pixel(COLOR_IDLE);
    last_led_color = COLOR_IDLE;
}

void update_activity_led() {
    const uint32_t now = to_ms_since_boot(get_absolute_time());
    // The explicit flags prevent a false traffic indication during the first
    // 350 ms after power-up, when the timestamp values are still zero.
    const bool tx_active =
        has_tx_activity && (now - last_tx_activity) < TRAFFIC_HOLD_MS;
    const bool rx_active =
        has_rx_activity && (now - last_rx_activity) < TRAFFIC_HOLD_MS;
    const bool traffic_active = tx_active || rx_active;
    const bool pulse_on =
        (now % TRAFFIC_BLINK_PERIOD_MS) < TRAFFIC_BLINK_ON_MS;
    const uint32_t color = !traffic_active ? COLOR_IDLE
                         : !pulse_on       ? COLOR_IDLE
                         : tx_active && rx_active ? COLOR_BOTH
                         : tx_active              ? COLOR_TX
                                                  : COLOR_RX;
    if (color != last_led_color) {
        put_pixel(color);
        last_led_color = color;
    }
}

void initialise_uart() {
    uart_init(uart0, UART_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    uart_set_hw_flow(uart0, false, false);
    uart_set_fifo_enabled(uart0, true);
}

void drain_debug_input() {
    // pico_stdio_usb needs CDC 0 RX to be drained. This firmware intentionally
    // does not interpret debug-console input, so discard it safely.
    std::array<uint8_t, 64> discard{};
    while (tud_cdc_n_available(DEBUG_CDC) > 0) {
        tud_cdc_n_read(DEBUG_CDC, discard.data(), discard.size());
    }
}

void receive_usb_data() {
    std::array<uint8_t, 64> buffer{};
    while (tud_cdc_n_available(UART_CDC) > 0) {
        if (usb_to_uart.free() == 0) {
            return;
        }
        const size_t read_limit = std::min(buffer.size(), usb_to_uart.free());
        const uint32_t received =
            tud_cdc_n_read(UART_CDC, buffer.data(), read_limit);
        for (uint32_t i = 0; i < received; ++i) {
            // read_limit never exceeds the free capacity of the ring.
            usb_to_uart.push(buffer[i]);
        }
    }
}

void move_usb_to_uart() {
    bool transferred = false;
    uint8_t byte = 0;
    while (uart_is_writable(uart0) && usb_to_uart.pop(&byte)) {
        uart_putc_raw(uart0, byte);
        transferred = true;
    }
    if (transferred) {
        last_tx_activity = to_ms_since_boot(get_absolute_time());
        has_tx_activity = true;
    }
}

void receive_uart_data() {
    while (uart_is_readable(uart0)) {
        if (!uart_to_usb.push(uart_getc(uart0))) {
            return;
        }
    }
}

void move_uart_to_usb() {
    if (uart_to_usb.empty() || tud_cdc_n_write_available(UART_CDC) == 0) {
        return;
    }

    std::array<uint8_t, 64> buffer{};
    const size_t limit =
        std::min<size_t>(buffer.size(), tud_cdc_n_write_available(UART_CDC));
    size_t count = 0;
    while (count < limit && uart_to_usb.pop(&buffer[count])) {
        ++count;
    }
    if (count > 0) {
        tud_cdc_n_write(UART_CDC, buffer.data(), count);
        tud_cdc_n_write_flush(UART_CDC);
        last_rx_activity = to_ms_since_boot(get_absolute_time());
        has_rx_activity = true;
    }
}

}  // namespace

int main() {
    board_init();
    tusb_init();
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }
    stdio_init_all();

    initialise_uart();
    initialise_led();
    printf("RP2040 Zero dual-CDC UART adapter ready (GP0/GP1, %u baud)\r\n",
           UART_BAUD_RATE);

    while (true) {
        tud_task();
        drain_debug_input();
        receive_usb_data();
        move_usb_to_uart();
        receive_uart_data();
        move_uart_to_usb();
        update_activity_led();
    }
}
