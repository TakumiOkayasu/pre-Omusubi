#pragma once

#include <cstdint>
#include "string_view.h"

namespace omusubi {

/**
 * @brief UTF-8ヘルパー関数
 */
namespace utf8 {

/**
 * @brief UTF-8の最初のバイトから文字のバイト数を取得
 */
inline uint8_t get_char_byte_length(uint8_t first_byte) noexcept {
    if ((first_byte & 0x80) == 0x00) return 1;  // 0xxxxxxx (ASCII)
    if ((first_byte & 0xE0) == 0xC0) return 2;  // 110xxxxx
    if ((first_byte & 0xF0) == 0xE0) return 3;  // 1110xxxx
    if ((first_byte & 0xF8) == 0xF0) return 4;  // 11110xxx
    return 1;  // 不正なバイト
}

/**
 * @brief 文字数をカウント（バイト数ではなく）
 */
inline uint32_t count_chars(const char* str, uint32_t byte_length) noexcept {
    uint32_t char_count = 0;
    uint32_t i = 0;
    
    while (i < byte_length) {
        uint8_t char_len = get_char_byte_length(static_cast<uint8_t>(str[i]));
        i += char_len;
        ++char_count;
    }
    
    return char_count;
}

/**
 * @brief n番目の文字の開始位置を取得（バイト単位）
 */
inline uint32_t get_char_position(const char* str, uint32_t byte_length, 
                                  uint32_t char_index) noexcept {
    uint32_t current_char = 0;
    uint32_t i = 0;
    
    while (i < byte_length && current_char < char_index) {
        uint8_t char_len = get_char_byte_length(static_cast<uint8_t>(str[i]));
        i += char_len;
        ++current_char;
    }
    
    return i;
}

}  // namespace utf8

/**
 * @brief 固定長UTF-8文字列
 * 
 * - UTF-8エンコーディング
 * - バイト長と文字数を両方管理
 * - null終端を保証
 * - 動的メモリ確保なし
 */
template<uint32_t Capacity>
class FixedString {
public:
    /**
     * @brief デフォルトコンストラクタ
     */
    FixedString() noexcept : byte_length_(0) {
        buffer_[0] = '\0';
    }
    
    /**
     * @brief C文字列から構築
     */
    explicit FixedString(const char* str) noexcept : byte_length_(0) {
        buffer_[0] = '\0';
        if (str) {
            append(str);
        }
    }
    
    /**
     * @brief StringViewから構築
     */
    explicit FixedString(StringView view) noexcept : byte_length_(0) {
        buffer_[0] = '\0';
        append(view);
    }
    
    /**
     * @brief 容量を取得
     */
    constexpr uint32_t capacity() const noexcept {
        return Capacity;
    }
    
    /**
     * @brief バイト長を取得
     */
    uint32_t byte_length() const noexcept {
        return byte_length_;
    }
    
    /**
     * @brief 文字数を取得（UTF-8）
     */
    uint32_t char_length() const noexcept {
        return utf8::count_chars(buffer_, byte_length_);
    }
    
    /**
     * @brief 空か判定
     */
    bool is_empty() const noexcept {
        return byte_length_ == 0;
    }
    
    /**
     * @brief データへのポインタを取得
     */
    const char* data() const noexcept {
        return buffer_;
    }
    
    /**
     * @brief C文字列として取得（null終端保証）
     */
    const char* c_str() const noexcept {
        return buffer_;
    }
    
    /**
     * @brief StringViewに変換
     */
    StringView view() const noexcept {
        return StringView(buffer_, byte_length_);
    }
    
    /**
     * @brief 文字列を追加
     */
    bool append(StringView view) noexcept {
        if (byte_length_ + view.byte_length() > Capacity) {
            return false;
        }
        
        for (uint32_t i = 0; i < view.byte_length(); ++i) {
            buffer_[byte_length_++] = view[i];
        }
        
        buffer_[byte_length_] = '\0';
        return true;
    }
    
    /**
     * @brief C文字列を追加
     */
    bool append(const char* str) noexcept {
        if (!str) {
            return false;
        }
        return append(StringView::from_c_string(str));
    }
    
    /**
     * @brief 1文字追加
     */
    bool append(char c) noexcept {
        if (byte_length_ >= Capacity) {
            return false;
        }
        
        buffer_[byte_length_++] = c;
        buffer_[byte_length_] = '\0';
        return true;
    }
    
    /**
     * @brief クリア
     */
    void clear() noexcept {
        byte_length_ = 0;
        buffer_[0] = '\0';
    }
    
    /**
     * @brief 文字列が等しいか比較
     */
    bool equals(StringView other) const noexcept {
        return view().equals(other);
    }
    
    /**
     * @brief 等価演算子
     */
    bool operator==(StringView other) const noexcept {
        return equals(other);
    }
    
    /**
     * @brief 非等価演算子
     */
    bool operator!=(StringView other) const noexcept {
        return !equals(other);
    }
    
    /**
     * @brief 文字単位のアクセス（UTF-8）
     */
    StringView get_char(uint32_t char_index) const noexcept {
        uint32_t byte_pos = utf8::get_char_position(buffer_, byte_length_, char_index);
        
        if (byte_pos >= byte_length_) {
            return StringView();
        }
        
        uint8_t char_len = utf8::get_char_byte_length(
            static_cast<uint8_t>(buffer_[byte_pos])
        );
        return StringView(buffer_ + byte_pos, char_len);
    }
    
    /**
     * @brief イテレータ（開始）
     */
    const char* begin() const noexcept {
        return buffer_;
    }
    
    /**
     * @brief イテレータ（終了）
     */
    const char* end() const noexcept {
        return buffer_ + byte_length_;
    }

private:
    char buffer_[Capacity + 1];  // +1 for null terminator
    uint32_t byte_length_;
};

}  // namespace omusubi