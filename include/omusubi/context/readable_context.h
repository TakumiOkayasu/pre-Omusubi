#pragma once

namespace omusubi {

// 前方宣言
class SerialContext;
class BluetoothContext;

/**
 * @brief 読み取り可能なデバイスのコンテキスト
 */
class ReadableContext {
public:
    ReadableContext() = default;
    virtual ~ReadableContext() = default;
    ReadableContext(const ReadableContext&) = delete;
    ReadableContext& operator=(const ReadableContext&) = delete;
    ReadableContext(ReadableContext&&) = delete;
    ReadableContext& operator=(ReadableContext&&) = delete;

    /** @brief シリアルポートコンテキストを取得（実行時） */
    virtual SerialContext* get_serial_context(uint8_t port) const = 0;

    /** @brief シリアルポートコンテキストを取得（コンパイル時） */
    template<uint8_t Port>
    SerialContext* get_serial_context() const {
        static_assert(Port <= 2, "Serial port must be 0, 1, or 2");
        return get_serial_context(Port);
    }

    /** @brief シリアルポート数を取得 */
    virtual uint8_t get_serial_count() const = 0;

    /** @brief Bluetoothコンテキストを取得 */
    virtual BluetoothContext* get_bluetooth_context() const = 0;
};

}  // namespace omusubi
