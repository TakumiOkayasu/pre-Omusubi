// ========================================
// BLE Server Example
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
        serial->write_line("=== BLE Server ==="_sv);
    }

    if (display) {
        display->clear();
        display->set_cursor(0, 0);
        display->set_text_size(2);
        display->write_line("BLE Server"_sv);
        display->set_text_size(1);
    }

    if (ble) {
        // Peripheralモードで初期化
        ble->begin_peripheral("M5Stack-BLE"_sv);

        if (serial) {
            serial->write("Device: "_sv);
            serial->write_line(ble->get_local_name());
        }

        // サービスを追加
        BLEService* service = ble->add_service(
            StringView::from_c_string(SERVICE_UUID)
        );

        if (service) {
            if (serial) {
                serial->write("Service: "_sv);
                serial->write_line(service->get_uuid());
            }

            // Characteristicを追加
            BLECharacteristic* characteristic = service->add_characteristic(
                StringView::from_c_string(CHAR_UUID),
                static_cast<uint16_t>(BLECharacteristicProperty::read) |
                static_cast<uint16_t>(BLECharacteristicProperty::write) |
                static_cast<uint16_t>(BLECharacteristicProperty::notify)
            );

            if (characteristic) {
                // 初期値を設定
                characteristic->write_string("Hello!"_sv);

                if (serial) {
                    serial->write("Char: "_sv);
                    serial->write_line(characteristic->get_uuid());
                }
            }

            // Advertisingを開始
            if (ble->start_advertising()) {
                if (serial) {
                    serial->write_line("Advertising..."_sv);
                }

                if (display) {
                    display->write_line();
                    display->write_line("Advertising"_sv);
                    display->write(ble->get_local_name().view());
                    display->write_line();
                    display->write_line("A: Notify"_sv);
                }
            }
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

    if (!ble) {
        ctx.delay(100);
        return;
    }

    // 接続状態を監視
    static bool last_connected = false;
    bool current_connected = ble->is_connected();

    if (current_connected != last_connected) {
        if (current_connected) {
            if (serial) {
                serial->write_line("Connected!"_sv);
                serial->write("Device: "_sv);
                serial->write_line(ble->get_connected_device_name());
            }

            if (display) {
                display->clear();
                display->set_cursor(0, 0);
                display->write_line("Connected"_sv);
                display->write(ble->get_connected_device_name().view());
            }
        } else {
            if (serial) {
                serial->write_line("Disconnected"_sv);
            }

            if (display) {
                display->clear();
                display->set_cursor(0, 0);
                display->write_line("Disconnected"_sv);
            }
        }

        last_connected = current_connected;
    }

    // ボタンA: 通知送信
    if (button_a && button_a->was_pressed()) {
        if (ble->is_connected()) {
            BLEService* service = ble->get_service(
                StringView::from_c_string(SERVICE_UUID)
            );

            if (service) {
                BLECharacteristic* ch = service->get_characteristic(
                    StringView::from_c_string(CHAR_UUID)
                );

                if (ch && ch->can_notify()) {
                    const char* msg = "Button pressed!";
                    ch->notify(
                        reinterpret_cast<const uint8_t*>(msg),
                        15
                    );

                    if (serial) {
                        serial->write_line("Notified!"_sv);
                    }

                    if (display) {
                        display->write_line("Sent!"_sv);
                    }
                }
            }
        } else {
            if (serial) {
                serial->write_line("Not connected"_sv);
            }
        }
    }

    ctx.delay(10);
}
