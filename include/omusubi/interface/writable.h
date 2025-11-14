#pragma once

#include "omusubi/core/span.hpp"

#include <cstddef>
#include <cstdint>

namespace omusubi {

/**
 * @brief バイト列書き込みインターフェース
 */
class ByteWritable {
public:
    ByteWritable() = default;
    virtual ~ByteWritable() = default;
    ByteWritable(const ByteWritable&) = delete;
    ByteWritable& operator=(const ByteWritable&) = delete;
    ByteWritable(ByteWritable&&) = delete;
    ByteWritable& operator=(ByteWritable&&) = delete;

    /** @brief バイト列を書き込む */
    virtual size_t write(span<const uint8_t> data) = 0;
};

/**
 * @brief テキスト書き込みインターフェース
 */
class TextWritable : public ByteWritable {
public:
    /** @brief テキストを書き込む */
    virtual size_t write_text(span<const char> text) = 0;
};

} // namespace omusubi
