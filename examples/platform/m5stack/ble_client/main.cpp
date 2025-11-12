// ========================================
// BLE Client Example
// ========================================
#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

// カスタムサービスUUID
constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char* CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

void setup() {
    SystemContext& ctx = get_system_context();
    ctx.begin();

    SerialCommunication* serial = ctx.get_serial(0);
    BLECommunication* ble = ctx.get_ble();
    Displayable* display = ctx.get_display();

    if (serial) {
        serial->write_line("=== BLE Client ==="_sv);
    }

    if (display) {
        display->clear();
        display->set_cursor(0, 0);
        display->set_text_size(2);
        display->write_line("BLE Client"_sv);
        display->set_text_size(1);
    }

    if (ble) {
        // Centralモードで初期化
        ble->begin_central("M5Stack-Central"_sv);

        if (serial) {
            serial->write_line("Central mode"_sv);
        }

        if (display) {
            display->write_line();
            display->write_line("Ready"_sv);
            display->write_line("A: Scan"_sv);
            display->write_line("B: Connect"_sv);
        }
    } else {
        if (serial) {
            serial->write_line("BLE not available"_sv);
        }
        if (display) {
            display->write_line("BLE N/A"_sv);
        }
    }
}

void loop() {
    SystemContext& ctx = get_system_context();
    ctx.update();

    SerialCommunication* serial = ctx.get_serial(0);
    BLECommunication* ble = ctx.get_ble();
    Displayable* display = ctx.get_display();
    Pressable* button_a = ctx.get_button(0);
    Pressable* button_b = ctx.get_button(1);

    if (!ble) {
        ctx.delay(100);
        return;
    }

    // ボタンA: スキャン
    if (button_a && button_a->was_pressed()) {
        if (display) {
            display->clear();
            display->set_cursor(0, 0);
            display->write_line("Scanning..."_sv);
        }

        if (serial) {
            serial->write_line("Scanning BLE..."_sv);
        }

        if (ble) {
            ble->start_scan();
            ctx.delay(3000);
            ble->stop_scan();

            uint8_t count = ble->get_found_count();

            if (display) {
                display->clear();
                display->set_cursor(0, 0);
                display->write("Found: "_sv);
                display->write_line(count);
                display->write_line();
            }

            if (serial) {
                serial->write("Found "_sv);
                serial->write(count);
                serial->write_line(" devices:"_sv);
            }

            for (uint8_t i = 0; i < count && i < 5; ++i) {
                FixedString<64> name = ble->get_found_name(i);
                int32_t rssi = ble->get_found_signal_strength(i);

                if (display && i < 3) {
                    display->write(i);
                    display->write(": "_sv);
                    display->write(name.view().substring(0, 12));
                    display->write_line();
                }

                if (serial) {
                    serial->write("  "_sv);
                    serial->write(i);
                    serial->write(": "_sv);
                    serial->write(name);
                    serial->write(" ("_sv);
                    serial->write(rssi);
                    serial->write_line(" dBm)"_sv);
                }
            }
        }
    }

    // ボタンB: 最初のデバイスに接続
    if (button_b && button_b->was_pressed()) {
        if (ble->get_found_count() > 0) {
            if (serial) {
                serial->write_line("Connecting..."_sv);
            }

            if (display) {
                display->clear();
                display->set_cursor(0, 0);
                display->write_line("Connecting..."_sv);
            }

            if (ble->connect_to_found(0)) {
                if (serial) {
                    serial->write_line("Connected!"_sv);
                    serial->write("Device: "_sv);
                    serial->write_line(ble->get_connected_device_name());
                }

                if (display) {
                    display->write_line("Connected!"_sv);
                    display->write(ble->get_connected_device_name().view());
                }
            } else {
                if (serial) {
                    serial->write_line("Failed"_sv);
                }

                if (display) {
                    display->write_line("Failed"_sv);
                }
            }
        } else {
            if (serial) {
                serial->write_line("Scan first"_sv);
            }
        }
    }

    ctx.delay(10);
}
