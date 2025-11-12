#pragma once

#include "omusubi/interface/connectable.h"
#include "omusubi/interface/scannable.h"
#include "omusubi/core/string_view.h"
#include "omusubi/core/fixed_string.hpp"

namespace omusubi {

/**
 * @brief WiFi通信デバイス
 *
 * WiFi接続、ネットワークスキャン、アクセスポイントモードを扱います。
 * Connectable、Scannableインターフェースを実装しています。
 *
 * 使用例:
 * @code
 * WiFiCommunication* wifi = ctx.get_wifi();
 *
 * // WiFiに接続
 * if (wifi->connect_to("MySSID"_sv, "password"_sv, 10000)) {
 *     FixedString<16> ip = wifi->get_ip_address();
 *     serial->write("IP: "_sv);
 *     serial->write_line(ip);
 * }
 *
 * // ネットワークスキャン
 * wifi->start_scan();
 * ctx.delay(3000);
 * wifi->stop_scan();
 *
 * uint8_t count = wifi->get_found_count();
 * for (uint8_t i = 0; i < count; ++i) {
 *     FixedString<64> ssid = wifi->get_found_name(i);
 * }
 *
 * // アクセスポイントモード
 * wifi->start_ap("M5Stack-AP"_sv, "password123"_sv);
 * @endcode
 */
class WiFiCommunication
    : public Connectable
    , public Scannable {
public:
    WiFiCommunication() = default;
    ~WiFiCommunication() override = default;
    WiFiCommunication(const WiFiCommunication&) = delete;
    WiFiCommunication& operator=(const WiFiCommunication&) = delete;
    WiFiCommunication(WiFiCommunication&&) = delete;
    WiFiCommunication& operator=(WiFiCommunication&&) = delete;

    /** @brief WiFiネットワークに接続 @param ssid SSID @param password パスワード @param timeout_ms タイムアウト(ms) @return 成功ならtrue */
    virtual bool connect_to(StringView ssid, StringView password, uint32_t timeout_ms = 0) = 0;

    /** @brief オープンネットワークに接続 @param ssid SSID @param timeout_ms タイムアウト(ms) @return 成功ならtrue */
    virtual bool connect_to_open(StringView ssid, uint32_t timeout_ms = 0) = 0;

    /** @brief スキャン結果から接続 @param index ネットワークインデックス @param password パスワード @return 成功ならtrue */
    virtual bool connect_to_found(uint8_t index, StringView password) = 0;

    /** @brief IPアドレスを取得 @return IPアドレス */
    virtual FixedString<16> get_ip_address() const = 0;

    /** @brief MACアドレスを取得 @return MACアドレス */
    virtual FixedString<32> get_mac_address() const = 0;

    /** @brief サブネットマスクを取得 @return サブネットマスク */
    virtual FixedString<16> get_subnet_mask() const = 0;

    /** @brief ゲートウェイアドレスを取得 @return ゲートウェイ */
    virtual FixedString<16> get_gateway() const = 0;

    /** @brief DNSサーバーアドレスを取得 @return DNS */
    virtual FixedString<16> get_dns() const = 0;

    /** @brief 接続中のSSIDを取得 @return SSID */
    virtual FixedString<64> get_connected_ssid() const = 0;

    /** @brief アクセスポイントモードを開始 @param ssid SSID @param password パスワード @return 成功ならtrue */
    virtual bool start_ap(StringView ssid, StringView password) = 0;

    /** @brief アクセスポイントモードを停止 */
    virtual void stop_ap() = 0;

    /** @brief アクセスポイントモードか確認 @return APモードならtrue */
    virtual bool is_ap_mode() const = 0;
};

}  // namespace omusubi