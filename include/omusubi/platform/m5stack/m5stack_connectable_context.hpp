#pragma once

#include "omusubi/context/connectable_context.h"
#include "m5stack_serial_context.hpp"
#include "m5stack_bluetooth_context.hpp"
#include "m5stack_wifi_context.hpp"
#include "m5stack_ble_context.hpp"

namespace omusubi {
namespace platform {
namespace m5stack {

/**
 * @brief M5Stack用接続可能デバイスコンテキスト
 *
 * Pattern A: Context owns devices (CLAUDE.mdに準拠)
 * メンバ変数としてデバイスを所有し、それらへのポインタを返す
 */
class M5StackConnectableContext : public ConnectableContext {
private:
    mutable M5StackSerialContext serials_[3]{
        M5StackSerialContext(0),
        M5StackSerialContext(1),
        M5StackSerialContext(2)
    };
    mutable M5StackBluetoothContext bluetooth_;
    mutable M5StackWiFiContext wifi_;
    mutable M5StackBLEContext ble_;

public:
    M5StackConnectableContext() = default;
    ~M5StackConnectableContext() override = default;

    SerialContext* get_serial_context(uint8_t port) const override {
        return (port < 3) ? &serials_[port] : nullptr;
    }

    uint8_t get_serial_count() const override { return 3; }

    BluetoothContext* get_bluetooth_context() const override { return &bluetooth_; }
    WiFiContext* get_wifi_context() const override { return &wifi_; }
    BLEContext* get_ble_context() const override { return &ble_; }
};

}  // namespace m5stack
}  // namespace platform
}  // namespace omusubi
