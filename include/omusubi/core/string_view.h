#pragma once

#include <cstdint>
#include <cstddef>
#include "mcu_config.h"

namespace omusubi {

/**
 * @brief 文字列の読み取り専用ビュー
 */
class StringView {
public:
    /**
     * @brief デフォルトコンストラクタ（空文字列）
 */
    constexpr StringView() noexcept
        : data_(""), byte_length_(0) {}
    
    /**
     * @brief C文字列リテラルから構築（コンパイル時長さ計算）
 */
    template<uint32_t N>
    constexpr StringView(const char (&str)[N]) noexcept
        : data_(str), byte_length_(N - 1) {}  // null終端を除く
    
    /**
     * @brief ポインタと長さから構築
 */
    constexpr StringView(const char* str, uint32_t len) noexcept
        : data_(str), byte_length_(len) {}
    
    /**
     * @brief C文字列から構築（実行時長さ計算）
 */
    static StringView from_c_string(const char* str) {
        if (!str) {
            return StringView();
        }
        
        uint32_t len = 0;
        while (str[len]) {
            ++len;
        }
        
        return StringView(str, len);
    }
    
    /**
     * @brief データへのポインタを取得
 */
    constexpr const char* data() const noexcept {
        return data_;
    }
    
    /**
     * @brief バイト長を取得
 */
    constexpr uint32_t byte_length() const noexcept {
        return byte_length_;
    }
    
    /**
     * @brief 空文字列か判定
 */
    constexpr bool is_empty() const noexcept {
        return byte_length_ == 0;
    }
    
    /**
     * @brief バイト単位のアクセス
 */
    constexpr char operator[](uint32_t index) const noexcept {
        return (index < byte_length_) ? data_[index] : '\0';
    }
    
    /**
     * @brief 部分文字列を取得
 */
    constexpr StringView substring(uint32_t start, uint32_t length) const noexcept {
        if (start >= byte_length_) {
            return StringView();
        }
        
        uint32_t actual_length = length;
        if (start + length > byte_length_) {
            actual_length = byte_length_ - start;
        }
        
        return StringView(data_ + start, actual_length);
    }
    
    /**
     * @brief 文字列が等しいか比較
 */
    bool equals(StringView other) const noexcept {
        if (byte_length_ != other.byte_length_) {
            return false;
        }
        
        for (uint32_t i = 0; i < byte_length_; ++i) {
            if (data_[i] != other.data_[i]) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * @brief 等価演算子
 */
    constexpr bool operator==(StringView other) const noexcept {
        return equals(other);
    }
    
    /**
     * @brief 非等価演算子
 */
    constexpr bool operator!=(StringView other) const noexcept {
        return !equals(other);
    }
    
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
        
        uint32_t start = byte_length_ - suffix.byte_length_;
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
    constexpr const char* begin() const noexcept {
        return data_;
    }
    
    /**
     * @brief イテレータ（終了）
 */
    constexpr const char* end() const noexcept {
        return data_ + byte_length_;
    }

private:
    const char* data_;
    uint32_t byte_length_;
};

namespace literals {

/**
 * @brief 文字列リテラル演算子
 */
constexpr StringView operator""_sv(const char* str, size_t len) noexcept {
    return StringView(str, static_cast<uint32_t>(len));
}

}  // namespace literals

}  // namespace omusubi