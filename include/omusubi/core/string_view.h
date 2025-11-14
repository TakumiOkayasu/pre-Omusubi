#pragma once

#include <cstddef>
#include <cstdint>

#include "mcu_config.h"
#include "string_base.hpp"

namespace omusubi {

namespace utf8 {

inline uint8_t get_char_byte_length(uint8_t first_byte) noexcept {
    if ((first_byte & 0x80) == 0x00) {
        return 1;
    }
    if ((first_byte & 0xE0) == 0xC0) {
        return 2;
    }
    if ((first_byte & 0xF0) == 0xE0) {
        return 3;
    }
    if ((first_byte & 0xF8) == 0xF0) {
        return 4;
    }
    return 1;
}

inline uint32_t count_chars(const char* str, uint32_t byte_length) noexcept {
    uint32_t char_count = 0;
    uint32_t i = 0;

    while (i < byte_length) {
        const uint8_t char_len = get_char_byte_length(static_cast<uint8_t>(str[i]));
        i += char_len;
        ++char_count;
    }

    return char_count;
}

inline uint32_t get_char_position(const char* str, uint32_t byte_length, uint32_t char_index) noexcept {
    uint32_t current_char = 0;
    uint32_t i = 0;

    while (i < byte_length && current_char < char_index) {
        const uint8_t char_len = get_char_byte_length(static_cast<uint8_t>(str[i]));
        i += char_len;
        ++current_char;
    }

    return i;
}

} // namespace utf8

class StringView : public String<StringView> {
public:
    /**
     * @brief デフォルトコンストラクタ（空文字列）
     */
    constexpr StringView() noexcept : data_(""), byte_length_(0) {}

    /**
     * @brief C文字列リテラルから構築（コンパイル時長さ計算）
     */
    template <uint32_t N>
    constexpr StringView(const char (&str)[N]) noexcept : data_(str), byte_length_(N - 1) {} // null終端を除く

    /**
     * @brief ポインタと長さから構築
     */
    constexpr StringView(const char* str, uint32_t len) noexcept : data_(str), byte_length_(len) {}

    /**
     * @brief C文字列から構築（実行時長さ計算）
     */
    static StringView from_c_string(const char* str) {
        if (str == nullptr) {
            return StringView {};
        }

        uint32_t len = 0;

        while (str[len] != 0U) {
            ++len;
        }

        return {str, len};
    }

    /**
     * @brief データへのポインタを取得
     */
    constexpr const char* data() const noexcept { return data_; }

    /**
     * @brief バイト長を取得
     */
    constexpr uint32_t byte_length() const noexcept { return byte_length_; }

    /**
     * @brief バイト単位のアクセス
     */
    constexpr char operator[](uint32_t index) const noexcept { return (index < byte_length_) ? data_[index] : '\0'; }

    /**
     * @brief 部分文字列を取得
     */
    constexpr StringView substring(uint32_t start, uint32_t length) const noexcept {
        if (start >= byte_length_) {
            return StringView {};
        }

        uint32_t actual_length = length;

        if (start + length > byte_length_) {
            actual_length = byte_length_ - start;
        }

        return {data_ + start, actual_length};
    }

    bool operator==(StringView other) const noexcept { return String<StringView>::equals(other); }

    bool operator!=(StringView other) const noexcept { return !String<StringView>::equals(other); }

    /**
     * @brief 指定された接頭辞で始まるか判定
     */
    bool starts_with(StringView prefix) const noexcept {
        if (prefix.byte_length_ > byte_length_) {
            return false;
        }

        return substring(0, prefix.byte_length_).equals(prefix);
    }

    /**
     * @brief 指定された接尾辞で終わるか判定
     */
    bool ends_with(StringView suffix) const noexcept {
        if (suffix.byte_length_ > byte_length_) {
            return false;
        }

        const uint32_t start = byte_length_ - suffix.byte_length_;
        return substring(start, suffix.byte_length_).equals(suffix);
    }

    /**
     * @brief 部分文字列を含むか判定
     */
    bool contains(StringView needle) const noexcept {
        if (needle.byte_length_ > byte_length_) {
            return false;
        }

        for (uint32_t i = 0; i <= byte_length_ - needle.byte_length_; ++i) {
            if (substring(i, needle.byte_length_).equals(needle)) {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief イテレータ（開始）
     */
    constexpr const char* begin() const noexcept { return data_; }

    /**
     * @brief イテレータ（終了）
     */
    constexpr const char* end() const noexcept { return data_ + byte_length_; }

private:
    const char* data_;
    uint32_t byte_length_;
};

namespace literals {

/**
 * @brief 文字列リテラル演算子
 */
constexpr StringView operator""_sv(const char* str, size_t len) noexcept {
    return {str, static_cast<uint32_t>(len)};
}

} // namespace literals

} // namespace omusubi
