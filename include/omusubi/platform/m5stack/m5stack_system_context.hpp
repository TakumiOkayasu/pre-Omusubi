#pragma once

#include "omusubi/system_context.h"

namespace omusubi {
namespace platform {
namespace m5stack {

class M5StackSystemContext : public SystemContext {
private:
    class Impl;

    // ヒープを使わずに静的バッファで確保（placement newを使用）
    static constexpr size_t ImplBufferSize = 8192;  // Implのサイズに十分な容量
    alignas(std::max_align_t) char impl_buffer_[ImplBufferSize];

    Impl* get_impl() { return reinterpret_cast<Impl*>(impl_buffer_); }
    const Impl* get_impl() const { return reinterpret_cast<const Impl*>(impl_buffer_); }

    M5StackSystemContext();
    ~M5StackSystemContext() override;

public:
    // Singleton pattern
    static M5StackSystemContext& get_instance();

    // Disable copy and assignment
    M5StackSystemContext(const M5StackSystemContext&) = delete;
    M5StackSystemContext& operator=(const M5StackSystemContext&) = delete;

    // SystemContext interface implementation
    const char* get_device_name() const override;
    const char* get_firmware_version() const override;
    FixedString<32> get_chip_id() const override;
    uint32_t get_uptime_ms() const override;
    uint32_t get_free_memory() const override;
    PowerState get_power_state() const override;
    uint8_t get_battery_level() const override;

    SerialContext* get_serial(uint8_t port = 0) override;
    BluetoothContext* get_bluetooth() override;
    WiFiContext* get_wifi() override;
    BLEContext* get_ble() override;

    Pressable* get_button(uint8_t index) override;
    uint8_t get_button_count() const override;

    Measurable3D* get_accelerometer() override;
    Measurable3D* get_gyroscope() override;

    Displayable* get_display() override;

    void begin() override;
    void update() override;
    void delay(uint32_t ms) override;
    void reset() override;
};

}  // namespace m5stack
}  // namespace platform
}  // namespace omusubi
