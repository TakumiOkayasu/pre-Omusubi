#pragma once

#include "omusubi/core/fixed_string.hpp"

#include <cstdint>

namespace omusubi {

/**
 * @brief システム情報コンテキスト
 */
class SystemInfoContext {
public:
    SystemInfoContext() = default;
    virtual ~SystemInfoContext() = default;
    SystemInfoContext(const SystemInfoContext&) = delete;
    SystemInfoContext& operator=(const SystemInfoContext&) = delete;
    SystemInfoContext(SystemInfoContext&&) = delete;
    SystemInfoContext& operator=(SystemInfoContext&&) = delete;

    /** @brief デバイス名を取得 */
    virtual FixedString<64> get_device_name() const = 0;

    /** @brief ファームウェアバージョンを取得 */
    virtual FixedString<32> get_firmware_version() const = 0;

    /** @brief チップIDを取得 */
    virtual uint64_t get_chip_id() const = 0;

    /** @brief 稼働時間を取得（ミリ秒） */
    virtual uint32_t get_uptime_ms() const = 0;

    /** @brief 空きメモリを取得（バイト） */
    virtual uint32_t get_free_memory() const = 0;
};

} // namespace omusubi
