#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

SystemContext& ctx = get_system_context();
SerialCommunication* serial = nullptr;
BLECommunication* ble = nullptr;
Pressable* button_a = nullptr;

void setup() {
    ctx.begin();

    serial = ctx.get_serial(0);
    ble = ctx.get_ble();
    button_a = ctx.get_button(0);

    serial->write_line("=== BLE Client ==="_sv);

    ble->begin_central("M5Stack-Central"_sv);
    serial->write_line("Ready"_sv);
    serial->write_line("Press button A to scan"_sv);
}

void loop() {
    ctx.update();

    if (button_a->was_pressed()) {
        serial->write_line("Scanning..."_sv);

        ble->start_scan();
        ctx.delay(3000);
        ble->stop_scan();

        uint8_t count = ble->get_found_count();
        serial->write("Found "_sv);
        serial->write(count);
        serial->write_line(" devices:"_sv);

        for (uint8_t i = 0; i < count && i < 5; ++i) {
            FixedString<64> name = ble->get_found_name(i);
            int32_t rssi = ble->get_found_signal_strength(i);

            serial->write("  "_sv);
            serial->write(i);
            serial->write(": "_sv);
            serial->write(name);
            serial->write(" ("_sv);
            serial->write(rssi);
            serial->write_line(" dBm)"_sv);
        }
    }

    ctx.delay(10);
}
