#pragma once

#include "omusubi/core/string_view.h"

namespace omusubi {

/**
 * @brief データ書き込みインターフェース
 */
class Writable {
public:
    Writable() = default;
    virtual ~Writable() = default;
    Writable(const Writable&) = delete;
    Writable& operator=(const Writable&) = delete;
    Writable(Writable&&) = delete;
    Writable& operator=(Writable&&) = delete;

    /** @brief 文字列を出力 */
    virtual void write(StringView text) = 0;
};

}  // namespace omusubi