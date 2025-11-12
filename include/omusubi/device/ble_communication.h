#pragma once

#include <cstdint>
#include "omusubi/core/string_view.h"
#include "omusubi/core/fixed_string.hpp"
#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"

namespace omusubi {

// 前方宣言
class BLEService;
class BLECharacteristic;

/**
 * @brief BLE動作モード
 */
enum class BLEMode : uint8_t {
    idle,       ///< 未初期化
    central,    ///< Centralモード（クライアント）
    peripheral  ///< Peripheralモード（サーバー）
};

/**
 * @brief BLE Characteristic プロパティ
 */
enum class BLECharacteristicProperty : uint16_t {
    broadcast = 0x0001,
    read = 0x0002,
    write_without_response = 0x0004,
    write = 0x0008,
    notify = 0x0010,
    indicate = 0x0020,
    authenticated_signed_writes = 0x0040,
    extended_properties = 0x0080
};

/**
 * @brief BLE Characteristic（キャラクタリスティック）
 *
 * BLEサービスに属するデータ単位。読み取り、書き込み、通知などの機能を持ちます。
 * Bluetooth SIGで定義された標準UUIDまたはカスタムUUIDを使用します。
 */
class BLECharacteristic {
public:
    BLECharacteristic() = default;
    virtual ~BLECharacteristic() = default;
    BLECharacteristic(const BLECharacteristic&) = delete;
    BLECharacteristic& operator=(const BLECharacteristic&) = delete;

    virtual FixedString<64> get_uuid() const = 0;
    virtual uint16_t get_properties() const = 0;

    virtual bool can_read() const = 0;
    virtual bool can_write() const = 0;
    virtual bool can_notify() const = 0;
    virtual bool can_indicate() const = 0;

    virtual FixedString<512> read_string() = 0;
    virtual uint32_t read_bytes(uint8_t* buffer, uint32_t max_length) = 0;

    virtual void write_string(StringView value) = 0;
    virtual void write_bytes(const uint8_t* data, uint32_t length) = 0;

    virtual void notify(const uint8_t* data, uint32_t length) = 0;
    virtual void indicate(const uint8_t* data, uint32_t length) = 0;
};

/**
 * @brief BLE Service（サービス）
 *
 * 複数のCharacteristicをグループ化したもの。
 * 例: Heart Rate Service、Battery Service、カスタムサービスなど。
 */
class BLEService {
public:
    BLEService() = default;
    virtual ~BLEService() = default;
    BLEService(const BLEService&) = delete;
    BLEService& operator=(const BLEService&) = delete;

    virtual FixedString<64> get_uuid() const = 0;

    virtual BLECharacteristic* add_characteristic(StringView uuid, uint16_t properties) = 0;
    virtual BLECharacteristic* get_characteristic(StringView uuid) = 0;
    virtual uint8_t get_characteristic_count() const = 0;
    virtual BLECharacteristic* get_characteristic_at(uint8_t index) = 0;
};

/**
 * @brief BLE (Bluetooth Low Energy) 通信デバイス
 *
 * BLE 4.0以降の低電力Bluetooth通信を扱います。
 * Central（クライアント）モードとPeripheral（サーバー）モードの両方をサポートします。
 * Connectable、Scannableインターフェースを実装しています。
 *
 * 使用例（Peripheralモード - サーバー）:
 * @code
 * BLECommunication* ble = ctx.get_ble();
 *
 * // Peripheralモードで開始
 * ble->begin_peripheral("M5Stack-BLE"_sv);
 *
 * // サービスとキャラクタリスティックを追加
 * BLEService* service = ble->add_service("UUID"_sv);
 * BLECharacteristic* ch = service->add_characteristic(
 *     "UUID"_sv,
 *     static_cast<uint16_t>(BLECharacteristicProperty::read) |
 *     static_cast<uint16_t>(BLECharacteristicProperty::notify)
 * );
 *
 * // アドバタイジング開始
 * ble->start_advertising();
 *
 * // 通知を送信
 * if (ble->is_connected()) {
 *     ch->notify(data, length);
 * }
 * @endcode
 *
 * 使用例（Centralモード - クライアント）:
 * @code
 * BLECommunication* ble = ctx.get_ble();
 *
 * // Centralモードで開始
 * ble->begin_central("M5Stack-Central"_sv);
 *
 * // デバイスをスキャン
 * ble->start_scan();
 * ctx.delay(3000);
 * ble->stop_scan();
 *
 * // 見つかったデバイスに接続
 * uint8_t count = ble->get_found_count();
 * for (uint8_t i = 0; i < count; ++i) {
 *     FixedString<64> name = ble->get_found_name(i);
 * }
 * @endcode
 */
class BLECommunication
    : public Connectable
    , public Scannable {
public:
    BLECommunication() = default;
    ~BLECommunication() override = default;
    BLECommunication(const BLECommunication&) = delete;
    BLECommunication& operator=(const BLECommunication&) = delete;
    BLECommunication(BLECommunication&&) = delete;
    BLECommunication& operator=(BLECommunication&&) = delete;
    
    // ========================================
    // モード管理
    // ========================================
    
    /**
     * @brief Centralモードで初期化（クライアント）
     */
    virtual bool begin_central(StringView device_name) = 0;
    
    /**
     * @brief Peripheralモードで初期化（サーバー）
     */
    virtual bool begin_peripheral(StringView device_name) = 0;
    
    /**
     * @brief 現在のモードを取得
     */
    virtual BLEMode get_mode() const = 0;
    
    /**
     * @brief BLEを停止
     */
    virtual void end() = 0;
    
    // ========================================
    // Central（クライアント）モード
    // ========================================
    
    /**
     * @brief デバイス名で接続
     */
    virtual bool connect_to(StringView device_name, uint32_t timeout_ms = 0) = 0;
    
    /**
     * @brief MACアドレスで接続
     */
    virtual bool connect_to_address(StringView address, uint32_t timeout_ms = 0) = 0;
    
    /**
     * @brief スキャンで見つけたデバイスに接続
     */
    virtual bool connect_to_found(uint8_t index) = 0;
    
    /**
     * @brief 接続済みデバイスのサービスを取得
     */
    virtual BLEService* get_service(StringView uuid) = 0;
    
    /**
     * @brief 接続済みデバイスのサービス数を取得
     */
    virtual uint8_t get_service_count() const = 0;
    
    /**
     * @brief インデックスでサービスを取得
     */
    virtual BLEService* get_service_at(uint8_t index) = 0;
    
    // ========================================
    // Peripheral（サーバー）モード
    // ========================================
    
    /**
     * @brief サービスを追加
     */
    virtual BLEService* add_service(StringView uuid) = 0;
    
    /**
     * @brief Advertisingを開始
     */
    virtual bool start_advertising() = 0;
    
    /**
     * @brief Advertisingを停止
     */
    virtual void stop_advertising() = 0;
    
    /**
     * @brief Advertising中か確認
     */
    virtual bool is_advertising() const = 0;
    
    // ========================================
    // 共通情報
    // ========================================
    
    /**
     * @brief ローカルデバイス名を設定
     */
    virtual void set_local_name(StringView name) = 0;
    
    /**
     * @brief ローカルデバイス名を取得
     */
    virtual FixedString<64> get_local_name() const = 0;
    
    /**
     * @brief 接続済みデバイス名を取得
     */
    virtual FixedString<64> get_connected_device_name() const = 0;
    
    /**
     * @brief 接続済みデバイスのMACアドレスを取得
     */
    virtual FixedString<32> get_connected_device_address() const = 0;
    
    /**
     * @brief スキャンで見つけたデバイスのMACアドレスを取得
     */
    virtual FixedString<32> get_found_address(uint8_t index) const = 0;
    
    /**
     * @brief スキャンで見つけたデバイスの信号強度を取得
     */
    virtual int32_t get_found_signal_strength(uint8_t index) const = 0;
    
    /**
     * @brief スキャンで見つけたデバイスがConnectable広告か確認
     */
    virtual bool is_found_connectable(uint8_t index) const = 0;
};

}  // namespace omusubi