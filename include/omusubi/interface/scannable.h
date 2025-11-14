#pragma once

#include "omusubi/core/fixed_string.hpp"

#include <cstdint>

namespace omusubi {

/**
 * @brief スキャン機能インターフェース
 */
class Scannable {
public:
    Scannable() = default;
    virtual ~Scannable() = default;
    Scannable(const Scannable&) = delete;
    Scannable& operator=(const Scannable&) = delete;
    Scannable(Scannable&&) = delete;
    Scannable& operator=(Scannable&&) = delete;

    /** @brief スキャンを開始 */
    virtual void start_scan() = 0;

    /** @brief スキャンを停止 */
    virtual void stop_scan() = 0;

    /** @brief 発見されたデバイス数を取得 */
    virtual uint8_t get_found_count() const = 0;

    /** @brief 発見されたデバイス名を取得 */
    virtual FixedString<64> get_found_name(uint8_t index) const = 0;

    /** @brief 発見されたデバイスの信号強度を取得 */
    virtual int32_t get_found_signal_strength(uint8_t index) const = 0;
};

} // namespace omusubi
