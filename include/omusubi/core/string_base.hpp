#pragma once

#include <cstdint>

namespace omusubi {

namespace utf8 {

inline uint8_t get_char_byte_length(uint8_t first_byte) noexcept;
inline uint32_t count_chars(const char* str, uint32_t byte_length) noexcept;
inline uint32_t get_char_position(const char* str, uint32_t byte_length, uint32_t char_index) noexcept;

} // namespace utf8

/**
 * @brief CRTP文字列基底クラス
 *
 * StringViewとFixedStringの共通実装を提供。
 * 派生クラスはdata()とbyte_length()を実装する必要がある。
 */
template <typename Derived>
class String {
protected:
    constexpr const Derived& derived() const noexcept { return static_cast<const Derived&>(*this); }

public:
    uint32_t char_length() const noexcept { return utf8::count_chars(derived().data(), derived().byte_length()); }

    uint32_t get_char_position(uint32_t char_index) const noexcept { return utf8::get_char_position(derived().data(), derived().byte_length(), char_index); }

    constexpr bool is_empty() const noexcept { return derived().byte_length() == 0; }

    constexpr bool equals(const char* str, uint32_t len) const noexcept {
        if (derived().byte_length() != len) {
            return false;
        }

        for (uint32_t i = 0; i < len; ++i) {
            if (derived().data()[i] != str[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename Other>
    constexpr bool equals(const String<Other>& other) const noexcept {
        const auto& other_derived = static_cast<const Other&>(other);
        return equals(other_derived.data(), other_derived.byte_length());
    }
};

} // namespace omusubi
