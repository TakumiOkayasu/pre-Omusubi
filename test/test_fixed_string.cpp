// FixedString<N> の単体テスト

#include <omusubi/core/fixed_string.hpp>

#include "test_framework.hpp"

namespace fixed_string_test {

using namespace omusubi;
using namespace omusubi::literals;
using namespace test;

void test_fixed_string_basic() {
    test_section("基本機能");

    // デフォルトコンストラクタ
    FixedString<32> s1;
    TEST_ASSERT_EQ(s1.byte_length(), 0U, "デフォルト構築の文字列は空");
    TEST_ASSERT_STR_EQ(s1.c_str(), "", "空文字列のc_str()");

    // C文字列からの構築
    FixedString<32> s2("Hello");
    TEST_ASSERT_EQ(s2.byte_length(), 5U, "C文字列からのバイト長");
    TEST_ASSERT_STR_EQ(s2.c_str(), "Hello", "C文字列からの内容");

    // StringViewからの構築
    FixedString<32> s3("World"_sv);
    TEST_ASSERT_EQ(s3.byte_length(), 5U, "StringViewからのバイト長");
    TEST_ASSERT_STR_EQ(s3.c_str(), "World", "StringViewからの内容");
}

void test_fixed_string_append() {
    test_section("追加操作");

    FixedString<32> s;

    // StringView追加
    TEST_ASSERT(s.append("Hello"_sv), "StringView追加成功");
    TEST_ASSERT_EQ(s.byte_length(), 5U, "追加後のバイト長");

    // C文字列追加
    TEST_ASSERT(s.append(" World"), "C文字列追加成功");
    TEST_ASSERT_STR_EQ(s.c_str(), "Hello World", "連続追加後の内容");

    // 1文字追加
    TEST_ASSERT(s.append('!'), "1文字追加成功");
    TEST_ASSERT_STR_EQ(s.c_str(), "Hello World!", "1文字追加後の内容");
}

void test_fixed_string_capacity() {
    test_section("容量制限");

    FixedString<10> s;

    // 容量内の追加
    TEST_ASSERT(s.append("12345"), "容量内追加成功");
    TEST_ASSERT(s.append("67890"), "容量いっぱいまで追加成功");
    TEST_ASSERT_EQ(s.byte_length(), 10U, "容量いっぱいのバイト長");

    // 容量超過
    TEST_ASSERT(!s.append("X"), "容量超過の追加は失敗");
    TEST_ASSERT_EQ(s.byte_length(), 10U, "容量超過後もバイト長は変わらない");
}

void test_fixed_string_utf8() {
    test_section("UTF-8処理");

    FixedString<64> s;

    // 日本語追加
    s.append("こんにちは"_sv);
    TEST_ASSERT_EQ(s.byte_length(), 15U, "日本語のバイト長（5文字×3バイト）");
    TEST_ASSERT_EQ(s.char_length(), 5U, "日本語の文字数");

    // 混合文字列
    FixedString<64> s2("Hello世界"_sv);
    TEST_ASSERT_EQ(s2.byte_length(), 11U, "混合文字列のバイト長");
    TEST_ASSERT_EQ(s2.char_length(), 7U, "混合文字列の文字数");
}

void test_fixed_string_clear() {
    test_section("クリア操作");

    FixedString<32> s("Hello World");
    TEST_ASSERT_EQ(s.byte_length(), 11U, "クリア前のバイト長");

    s.clear();
    TEST_ASSERT_EQ(s.byte_length(), 0U, "クリア後のバイト長は0");
    TEST_ASSERT_STR_EQ(s.c_str(), "", "クリア後の内容は空");

    // クリア後の再利用
    TEST_ASSERT(s.append("New"), "クリア後に再度追加可能");
    TEST_ASSERT_STR_EQ(s.c_str(), "New", "クリア後の再追加の内容");
}

void test_fixed_string_comparison() {
    test_section("比較操作");

    FixedString<32> s1("Hello");
    FixedString<32> s2("Hello");
    FixedString<32> s3("World");

    TEST_ASSERT(s1 == "Hello"_sv, "等価比較: 一致");
    TEST_ASSERT(s1 != "World"_sv, "非等価比較: 不一致");
    TEST_ASSERT(s1.view() == s2.view(), "FixedString同士の比較");
}

void test_fixed_string_view_conversion() {
    test_section("StringView変換");

    FixedString<32> s("Test String");
    StringView view = s.view();

    TEST_ASSERT_EQ(view.byte_length(), 11U, "view()のバイト長");
    TEST_ASSERT(view == "Test String"_sv, "view()の内容");

    // viewは元の文字列を参照
    TEST_ASSERT(view.data() == s.data(), "viewは元のデータを参照");
}

void test_fixed_string_iteration() {
    test_section("イテレータ");

    FixedString<32> s("ABC");

    int count = 0;
    for (char c : s) {
        TEST_ASSERT(c == 'A' || c == 'B' || c == 'C', "イテレータで各文字にアクセス");
        count++;
    }
    TEST_ASSERT_EQ(count, 3, "イテレータで全要素を走査");
}

void test_fixed_string_null_termination() {
    test_section("null終端保証");

    FixedString<32> s;
    s.append("Hello");

    // null終端が保証されている
    TEST_ASSERT_EQ(s.data()[5], '\0', "null終端が存在");
    TEST_ASSERT_EQ(s.c_str()[5], '\0', "c_str()でnull終端が存在");
}

void test_fixed_string_constexpr() {
    test_section("constexpr対応");

    // コンパイル時文字列構築
    {
        constexpr auto str1 = fixed_string("Hello");
        static_assert(str1.byte_length() == 5, "constexpr byte_length()");
        static_assert(str1.capacity() == 5, "constexpr capacity()");
        TEST_ASSERT_EQ(str1.byte_length(), 5U, "constexpr構築のバイト長");
        TEST_ASSERT_STR_EQ(str1.c_str(), "Hello", "constexpr構築の内容");
    }

    // コンパイル時UTF-8処理
    {
        constexpr auto utf8_str = fixed_string("こんにちは");
        static_assert(utf8_str.byte_length() == 15, "constexpr UTF-8バイト長");
        static_assert(utf8_str.char_length() == 5, "constexpr UTF-8文字数");
        TEST_ASSERT_EQ(utf8_str.byte_length(), 15U, "constexpr UTF-8バイト長");
        TEST_ASSERT_EQ(utf8_str.char_length(), 5U, "constexpr UTF-8文字数");
    }

    // コンパイル時StringView
    {
        constexpr StringView view("Test");
        static_assert(view.byte_length() == 4, "constexpr StringView");
        TEST_ASSERT_EQ(view.byte_length(), 4U, "constexpr StringViewのバイト長");
    }

    // 実行時にconstexpr関数を使用
    FixedString<32> s1;
    s1.append("Hello");
    s1.append(" World");
    TEST_ASSERT_EQ(s1.byte_length(), 11U, "constexpr関数の実行時使用");
    TEST_ASSERT_STR_EQ(s1.c_str(), "Hello World", "constexpr append()の実行時使用");
}

int main() {
    begin_tests("FixedString<N>");

    test_fixed_string_basic();
    test_fixed_string_append();
    test_fixed_string_capacity();
    test_fixed_string_utf8();
    test_fixed_string_clear();
    test_fixed_string_comparison();
    test_fixed_string_view_conversion();
    test_fixed_string_iteration();
    test_fixed_string_null_termination();
    test_fixed_string_constexpr();

    return end_tests();
}

} // namespace fixed_string_test
