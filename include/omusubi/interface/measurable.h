#pragma once

#include "omusubi/core/types.h"

namespace omusubi {

class Measurable1D {
public:
    Measurable1D() = default;
    virtual ~Measurable1D() = default;
    Measurable1D(const Measurable1D&) = delete;
    Measurable1D& operator=(const Measurable1D&) = delete;
    Measurable1D(Measurable1D&&) = delete;
    Measurable1D& operator=(Measurable1D&&) = delete;
    
    virtual float get_value() const = 0;
};

class Measurable3D {
public:
    Measurable3D() = default;
    virtual ~Measurable3D() = default;
    Measurable3D(const Measurable3D&) = delete;
    Measurable3D& operator=(const Measurable3D&) = delete;
    Measurable3D(Measurable3D&&) = delete;
    Measurable3D& operator=(Measurable3D&&) = delete;
    
    virtual Vector3 get_values() const = 0;
    virtual float get_x() const = 0;
    virtual float get_y() const = 0;
    virtual float get_z() const = 0;
};

}  // namespace omusubi