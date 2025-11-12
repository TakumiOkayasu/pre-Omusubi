#pragma once

#include <cstdint>
#include "writable.h"

namespace omusubi {

class Displayable : public Writable {
public:
    Displayable() = default;
    ~Displayable() override = default;
    Displayable(const Displayable&) = delete;
    Displayable& operator=(const Displayable&) = delete;
    Displayable(Displayable&&) = delete;
    Displayable& operator=(Displayable&&) = delete;
    
    virtual void clear() = 0;
    virtual void set_cursor(int32_t x, int32_t y) = 0;
    virtual void set_text_size(uint8_t size) = 0;
    virtual void set_text_color(uint32_t color) = 0;
    virtual void set_background_color(uint32_t color) = 0;
    virtual uint16_t get_width() const = 0;
    virtual uint16_t get_height() const = 0;
};

}  // namespace omusubi