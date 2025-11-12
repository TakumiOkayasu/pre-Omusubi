#pragma once

#include "omusubi/core/fixed_string.hpp"
#include "omusubi/core/fixed_buffer.hpp"

namespace omusubi {

class Readable {
public:
    Readable() = default;
    virtual ~Readable() = default;
    Readable(const Readable&) = delete;
    Readable& operator=(const Readable&) = delete;
    Readable(Readable&&) = delete;
    Readable& operator=(Readable&&) = delete;
    
    virtual FixedString<256> read_string() = 0;
    virtual FixedString<256> read_line() = 0;
    virtual FixedString<256> read_string_wait() = 0;
    virtual FixedString<256> read_line_wait() = 0;
    virtual FixedBuffer<256> read_bytes() = 0;
    virtual bool has_data() const = 0;
};

}  // namespace omusubi