#pragma once

#include "omusubi/core/string_view.h"

namespace omusubi {

class Writable {
public:
    Writable() = default;
    virtual ~Writable() = default;
    Writable(const Writable&) = delete;
    Writable& operator=(const Writable&) = delete;
    Writable(Writable&&) = delete;
    Writable& operator=(Writable&&) = delete;
    
    virtual void write(StringView text) = 0;
    virtual void write_bytes(const uint8_t* data, uint32_t length) = 0;
    virtual void write(int32_t value) = 0;
    virtual void write(uint32_t value) = 0;
    virtual void write(float value) = 0;
    
    void write_line(StringView text) {
        write(text);
        write(StringView("\r\n", 2));
    }
    
    void write_line() {
        write(StringView("\r\n", 2));
    }
};

}  // namespace omusubi