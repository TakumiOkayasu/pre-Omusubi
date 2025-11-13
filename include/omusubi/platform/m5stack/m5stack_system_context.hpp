#pragma once

#include "omusubi/system_context.h"
#include "m5stack_connectable_context.hpp"
#include "m5stack_readable_context.hpp"
#include "m5stack_writable_context.hpp"
#include "m5stack_scannable_context.hpp"
#include "m5stack_sensor_context.hpp"
#include "m5stack_input_context.hpp"
#include "m5stack_output_context.hpp"
#include "m5stack_system_info_context.hpp"
#include "m5stack_power_context.hpp"

namespace omusubi {
namespace platform {
namespace m5stack {

/**
 * @brief M5Stack用システムコンテキスト
 *
 * Pattern A: デバイスの所有構造
 * - ConnectableContextがSerial, Bluetooth, WiFi, BLEの実体を所有
 * - 他のContextはデバイスへの参照を保持
 */
class M5StackSystemContext : public SystemContext {
private:
    // デバイスを所有するContext（先に初期化）
    mutable M5StackConnectableContext connectable_;

    // デバイスへの参照を持つContext（後に初期化）
    mutable M5StackReadableContext readable_;
    mutable M5StackWritableContext writable_;
    mutable M5StackScannableContext scannable_;

    // 独立したContext
    mutable M5StackSensorContext sensor_;
    mutable M5StackInputContext input_;
    mutable M5StackOutputContext output_;
    mutable M5StackSystemInfoContext system_info_;
    mutable M5StackPowerContext power_;

public:
    // コンストラクタ（get_system_context()内でのみ使用される）
    M5StackSystemContext();
    ~M5StackSystemContext() override = default;

    // コピー・ムーブ禁止
    M5StackSystemContext(const M5StackSystemContext&) = delete;
    M5StackSystemContext& operator=(const M5StackSystemContext&) = delete;

    // ========================================
    // システム制御
    // ========================================

    void begin() override;
    void update() override;
    void delay(uint32_t ms) override;
    void reset() override;

    // ========================================
    // カテゴリ別コンテキストアクセス
    // ========================================

    ConnectableContext* get_connectable_context() const override { return &connectable_; }
    ReadableContext* get_readable_context() const override { return &readable_; }
    WritableContext* get_writable_context() const override { return &writable_; }
    ScannableContext* get_scannable_context() const override { return &scannable_; }
    SensorContext* get_sensor_context() const override { return &sensor_; }
    InputContext* get_input_context() const override { return &input_; }
    OutputContext* get_output_context() const override { return &output_; }
    SystemInfoContext* get_system_info_context() const override { return &system_info_; }
    PowerContext* get_power_context() const override { return &power_; }
};

}  // namespace m5stack
}  // namespace platform
}  // namespace omusubi
