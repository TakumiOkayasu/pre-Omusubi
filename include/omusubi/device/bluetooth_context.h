#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"
#include "omusubi/core/string_view.h"
#include "omusubi/core/fixed_string.hpp"

namespace omusubi {

/**
 * @brief Bluetooth Classic通信デバイス
 *
 * Bluetooth 2.x/3.x (SPP: Serial Port Profile) による通信を扱います。
 * Readable、Writable、Connectable、Scannableインターフェースを実装しています。
 *
 * 使用例:
 * @code
 * BluetoothContext* bt = ctx.get_bluetooth();
 *
 * // ローカルデバイス名を設定
 * bt->set_local_name("M5Stack-BT"_sv);
 *
 * // デバイスをスキャン
 * bt->start_scan();
 * ctx.delay(3000);
 * bt->stop_scan();
 *
 * // スキャン結果を表示
 * uint8_t count = bt->get_found_count();
 * for (uint8_t i = 0; i < count; ++i) {
 *     FixedString<64> name = bt->get_found_name(i);
 *     int32_t rssi = bt->get_found_signal_strength(i);
 * }
 *
 * // デバイスに接続
 * if (bt->connect_to("RemoteDevice"_sv, 5000)) {
 *     bt->write_line("Hello Bluetooth!"_sv);
 * }
 * @endcode
 */
class BluetoothContext
    : public Readable
    , public Writable
    , public Connectable
    , public Scannable {
public:
    BluetoothContext() = default;
    ~BluetoothContext() override = default;
    BluetoothContext(const BluetoothContext&) = delete;
    BluetoothContext& operator=(const BluetoothContext&) = delete;
    BluetoothContext(BluetoothContext&&) = delete;
    BluetoothContext& operator=(BluetoothContext&&) = delete;

    /** @brief デバイス名で接続 @param device_name デバイス名 @param timeout_ms タイムアウト(ms) @return 成功ならtrue */
    virtual bool connect_to(StringView device_name, uint32_t timeout_ms = 0) = 0;

    /** @brief MACアドレスで接続 @param address アドレス @param timeout_ms タイムアウト(ms) @return 成功ならtrue */
    virtual bool connect_to_address(StringView address, uint32_t timeout_ms = 0) = 0;

    /** @brief スキャン結果から接続 @param index デバイスインデックス @return 成功ならtrue */
    virtual bool connect_to_found(uint8_t index) = 0;

    /** @brief ローカルデバイス名を設定 @param name デバイス名 */
    virtual void set_local_name(StringView name) = 0;

    /** @brief ローカルデバイス名を取得 @return デバイス名 */
    virtual FixedString<64> get_local_name() const = 0;

    /** @brief 接続中のデバイス名を取得 @return デバイス名 */
    virtual FixedString<64> get_connected_device_name() const = 0;

    /** @brief 接続中のデバイスアドレスを取得 @return MACアドレス */
    virtual FixedString<32> get_connected_device_address() const = 0;

    /** @brief スキャンで見つかったデバイスのアドレス @param index インデックス @return MACアドレス */
    virtual FixedString<32> get_found_address(uint8_t index) const = 0;

    /** @brief スキャンで見つかったデバイスの信号強度 @param index インデックス @return RSSI (dBm) */
    virtual int32_t get_found_signal_strength(uint8_t index) const = 0;
};

}  // namespace omusubi