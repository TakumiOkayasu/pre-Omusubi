#pragma once

#include "omusubi/interface/readable.h"
#include "omusubi/interface/writable.h"
#include "omusubi/interface/connectable.h"

namespace omusubi {

/**
 * @brief シリアル通信デバイス
 *
 * UART/USBシリアル通信を扱います。
 * Readable、Writable、Connectableインターフェースを実装しています。
 *
 * 使用例:
 * @code
 * SerialCommunication* serial = ctx.get_serial(0);
 *
 * // ボーレートを設定
 * serial->set_baud_rate(115200);
 *
 * // 接続
 * serial->connect();
 *
 * // データ送信
 * serial->write_line("Hello, Serial!"_sv);
 * serial->write(42);
 *
 * // データ受信
 * if (serial->has_data()) {
 *     FixedString<256> line = serial->read_line();
 * }
 *
 * // バッファをクリア
 * serial->clear();
 * @endcode
 */
class SerialCommunication
    : public Readable
    , public Writable
    , public Connectable {
public:
    SerialCommunication() = default;
    ~SerialCommunication() override = default;
    SerialCommunication(const SerialCommunication&) = delete;
    SerialCommunication& operator=(const SerialCommunication&) = delete;
    SerialCommunication(SerialCommunication&&) = delete;
    SerialCommunication& operator=(SerialCommunication&&) = delete;

    /** @brief ボーレートを設定 @param baud_rate ボーレート（例: 9600, 115200） */
    virtual void set_baud_rate(uint32_t baud_rate) = 0;

    /** @brief 現在のボーレートを取得 @return ボーレート */
    virtual uint32_t get_baud_rate() const = 0;

    /** @brief 送信バッファをフラッシュ（送信完了まで待機） */
    virtual void flush() = 0;

    /** @brief 受信バッファをクリア */
    virtual void clear() = 0;
};

}  // namespace omusubi