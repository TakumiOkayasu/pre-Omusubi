#pragma once

#include "omusubi/core/fixed_buffer.hpp"

namespace omusubi {

/**
 * @brief データ読み取りインターフェース
 */
class Readable {
public:
    Readable() = default;
    virtual ~Readable() = default;
    Readable(const Readable&) = delete;
    Readable& operator=(const Readable&) = delete;
    Readable(Readable&&) = delete;
    Readable& operator=(Readable&&) = delete;

    /** @brief データを読み取る */
    virtual FixedBuffer<256> read() = 0;
};

}  // namespace omusubi