#pragma once

#include "omusubi/context/readable_context.h"

namespace omusubi {

// 前方宣言
class SerialContext;
class BluetoothContext;
namespace platform {
namespace m5stack {

/**
 * @brief M5Stack用読み取り可能デバイスコンテキスト
 *
 * Pattern A: 既存のデバイスインスタンスへの参照を保持
 */
class M5StackReadableContext : public ReadableContext {
private:
    SerialContext* serials_[3];
    BluetoothContext* bluetooth_;

public:
    M5StackReadableContext(
        SerialContext* serial0,
        SerialContext* serial1,
        SerialContext* serial2,
        BluetoothContext* bluetooth)
        : serials_{serial0, serial1, serial2}
        , bluetooth_(bluetooth) {}

    ~M5StackReadableContext() override = default;

    SerialContext* get_serial_context(uint8_t port) const override {
        return (port < 3) ? serials_[port] : nullptr;
    }

    uint8_t get_serial_count() const override { return 3; }

    BluetoothContext* get_bluetooth_context() const override { return bluetooth_; }
};

}  // namespace m5stack
}  // namespace platform
}  // namespace omusubi
