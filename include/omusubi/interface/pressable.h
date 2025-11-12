#pragma once

namespace omusubi {

class Pressable {
public:
    Pressable() = default;
    virtual ~Pressable() = default;
    Pressable(const Pressable&) = delete;
    Pressable& operator=(const Pressable&) = delete;
    Pressable(Pressable&&) = delete;
    Pressable& operator=(Pressable&&) = delete;
    
    virtual bool is_pressed() const = 0;
    virtual bool was_pressed() = 0;
    virtual bool was_released() = 0;
};

}  // namespace omusubi