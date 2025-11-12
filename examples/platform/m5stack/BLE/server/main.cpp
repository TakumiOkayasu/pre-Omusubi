#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

constexpr const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char* CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

SystemContext& ctx = get_system_context();
SerialCommunication* serial = nullptr;
BLECommunication* ble = nullptr;
Pressable* button_a = nullptr;
BLEService* service = nullptr;
BLECharacteristic* ch = nullptr;

void setup() {
    ctx.begin();

    serial = ctx.get_serial(0);
    ble = ctx.get_ble();
    button_a = ctx.get_button(0);

    serial->write_line("=== BLE Server ==="_sv);

    ble->begin_peripheral("M5Stack-BLE"_sv);
    serial->write("Device: "_sv);
    serial->write_line(ble->get_local_name());

    service = ble->add_service(
        StringView::from_c_string(SERVICE_UUID)
    );

    ch = service->add_characteristic(
        StringView::from_c_string(CHAR_UUID),
        static_cast<uint16_t>(BLECharacteristicProperty::read) |
        static_cast<uint16_t>(BLECharacteristicProperty::write) |
        static_cast<uint16_t>(BLECharacteristicProperty::notify)
    );
    ch->write_string("Hello!"_sv);

    ble->start_advertising();
    serial->write_line("Advertising..."_sv);
    serial->write_line("Press button A to notify"_sv);
}

void loop() {
    ctx.update();

    if (button_a->was_pressed()) {
        if (ble->is_connected()) {
            const char* msg = "Button pressed!";
            ch->notify(
                reinterpret_cast<const uint8_t*>(msg),
                15
            );

            serial->write_line("Notified!"_sv);
        } else {
            serial->write_line("Not connected"_sv);
        }
    }

    ctx.delay(10);
}
