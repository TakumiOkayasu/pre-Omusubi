#pragma once

#include <cstdint>
#include "omusubi/core/fixed_string.hpp"

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

    /** @brief スキャンを実行 */
    virtual uint8_t scan() = 0;
};

}  // namespace omusubi