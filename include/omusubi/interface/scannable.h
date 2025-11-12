#pragma once

#include <cstdint>
#include "omusubi/core/fixed_string.hpp"

namespace omusubi {

class Scannable {
public:
    Scannable() = default;
    virtual ~Scannable() = default;
    Scannable(const Scannable&) = delete;
    Scannable& operator=(const Scannable&) = delete;
    Scannable(Scannable&&) = delete;
    Scannable& operator=(Scannable&&) = delete;
    
    virtual void start_scan() = 0;
    virtual void stop_scan() = 0;
    virtual bool is_scanning() const = 0;
    virtual uint8_t get_found_count() const = 0;
    virtual FixedString<64> get_found_name(uint8_t index) const = 0;
};

}  // namespace omusubi