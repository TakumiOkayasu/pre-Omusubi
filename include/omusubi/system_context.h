#pragma once

#include <cstdint>
#include "core/string_view.h"
#include "core/fixed_string.hpp"
#include "core/types.h"

namespace omusubi {

// 前方宣言
class SerialContext;
class BluetoothContext;
class WiFiContext;
class BLEContext;
class Pressable;
class Measurable1D;
class Measurable3D;
class Writable;
class Displayable;

/**
 * @brief システムコンテキスト
 * 
 * デバイス全体のシステム情報とハードウェアへのアクセスを提供する。
 * すべてのハードウェアアクセスはこのクラスを経由する。
 */
class SystemContext {
public:
    virtual ~SystemContext() = default;
    
    SystemContext(const SystemContext&) = delete;
    SystemContext& operator=(const SystemContext&) = delete;
    SystemContext(SystemContext&&) = delete;
    SystemContext& operator=(SystemContext&&) = delete;
    
    // ========================================
    // システム情報
    // ========================================
    
    /**
     * @brief デバイス名を取得
     */
    virtual const char* get_device_name() const = 0;
    
    /**
     * @brief ファームウェアバージョンを取得
     */
    virtual const char* get_firmware_version() const = 0;
    
    /**
     * @brief チップIDを取得
     */
    virtual FixedString<32> get_chip_id() const = 0;
    
    /**
     * @brief 起動からの経過時間（ミリ秒）
     */
    virtual uint32_t get_uptime_ms() const = 0;
    
    /**
     * @brief 空きメモリ（バイト）
     */
    virtual uint32_t get_free_memory() const = 0;
    
    // ========================================
    // 電源管理
    // ========================================
    
    /**
     * @brief 電源状態を取得
     */
    virtual PowerState get_power_state() const = 0;
    
    /**
     * @brief バッテリー残量（0-100%）
     */
    virtual uint8_t get_battery_level() const = 0;
    
    // ========================================
    // 通信デバイス
    // ========================================

    /**
     * @brief シリアル通信を取得
     */
    virtual SerialContext* get_serial(uint8_t port = 0) = 0;

    /**
     * @brief Bluetooth Classic通信を取得
     */
    virtual BluetoothContext* get_bluetooth() = 0;

    /**
     * @brief Wi-Fi通信を取得
     */
    virtual WiFiContext* get_wifi() = 0;

    /**
     * @brief BLE (Bluetooth Low Energy) 通信を取得
     */
    virtual BLEContext* get_ble() = 0;

    // ========================================
    // 入力デバイス
    // ========================================
    
    /**
     * @brief ボタンを取得
     */
    virtual Pressable* get_button(uint8_t index) = 0;
    
    /**
     * @brief ボタン数を取得
     */
    virtual uint8_t get_button_count() const = 0;
    
    // ========================================
    // センサー
    // ========================================
    
    /**
     * @brief 加速度センサーを取得
     */
    virtual Measurable3D* get_accelerometer() = 0;
    
    /**
     * @brief ジャイロスコープを取得
     */
    virtual Measurable3D* get_gyroscope() = 0;
    
    // ========================================
    // ディスプレイ
    // ========================================
    
    /**
     * @brief ディスプレイを取得
     */
    virtual Displayable* get_display() = 0;

    // ========================================
    // システム制御
    // ========================================
    
    /**
     * @brief システムを初期化
     */
    virtual void begin() = 0;
    
    /**
     * @brief システム状態を更新
     */
    virtual void update() = 0;
    
    /**
     * @brief 待機（ミリ秒）
     */
    virtual void delay(uint32_t ms) = 0;
    
    /**
     * @brief システムをリセット
     */
    virtual void reset() = 0;

protected:
    SystemContext() = default;
};

/**
 * @brief グローバルSystemContextを取得
 */
SystemContext& get_system_context();

}  // namespace omusubi