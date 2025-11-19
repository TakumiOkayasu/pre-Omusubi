#include <cassert>
#include <cstdio>
#include <cstring>
#include <omusubi/core/format.hpp>

using namespace omusubi;

// テストヘルパー
void test_format_basic() {
    printf("test_format_basic...\n");

    // 基本的な文字列フォーマット
    auto result1 = format<128>("Hello, {}!", "World");
    assert(strcmp(result1.c_str(), "Hello, World!") == 0);

    // 複数の引数
    auto result2 = format<128>("Name: {}, Age: {}", "Alice", 25);
    assert(strcmp(result2.c_str(), "Name: Alice, Age: 25") == 0);

    // 引数なし
    auto result3 = format<128>("No placeholders");
    assert(strcmp(result3.c_str(), "No placeholders") == 0);

    printf("  OK\n");
}

void test_format_integers() {
    printf("test_format_integers...\n");

    // 整数型
    auto result1 = format<128>("Value: {}", 42);
    assert(strcmp(result1.c_str(), "Value: 42") == 0);

    // 負数
    auto result2 = format<128>("Negative: {}", -123);
    assert(strcmp(result2.c_str(), "Negative: -123") == 0);

    // 0
    auto result3 = format<128>("Zero: {}", 0);
    assert(strcmp(result3.c_str(), "Zero: 0") == 0);

    // uint32_t
    auto result4 = format<128>("Unsigned: {}", static_cast<uint32_t>(4294967295U));
    assert(strcmp(result4.c_str(), "Unsigned: 4294967295") == 0);

    // int64_t
    auto result5 = format<128>("Large: {}", static_cast<int64_t>(1234567890123LL));
    assert(strcmp(result5.c_str(), "Large: 1234567890123") == 0);

    printf("  OK\n");
}

void test_format_types() {
    printf("test_format_types...\n");

    // bool
    auto result1 = format<128>("Bool: {} and {}", true, false);
    assert(strcmp(result1.c_str(), "Bool: true and false") == 0);

    // char
    auto result2 = format<128>("Char: {}", 'A');
    assert(strcmp(result2.c_str(), "Char: A") == 0);

    // StringView
    auto result3 = format<128>("View: {}", StringView("test"));
    assert(strcmp(result3.c_str(), "View: test") == 0);

    printf("  OK\n");
}

void test_format_escape() {
    printf("test_format_escape...\n");

    // エスケープされた括弧
    auto result1 = format<128>("Escaped: {{}}");
    assert(strcmp(result1.c_str(), "Escaped: {}") == 0);

    // 混在
    auto result2 = format<128>("{{Value: {}}}", 42);
    assert(strcmp(result2.c_str(), "{Value: 42}") == 0);

    printf("  OK\n");
}

void test_format_multiple() {
    printf("test_format_multiple...\n");

    // 複数のプレースホルダー
    auto result1 = format<128>("{} + {} = {}", 1, 2, 3);
    assert(strcmp(result1.c_str(), "1 + 2 = 3") == 0);

    // 異なる型の混在
    auto result2 = format<128>("String: {}, Int: {}, Bool: {}", "test", 100, true);
    assert(strcmp(result2.c_str(), "String: test, Int: 100, Bool: true") == 0);

    printf("  OK\n");
}

void test_format_hex() {
    printf("test_format_hex...\n");

    // 16進数（小文字）
    auto result1 = format_hex<128>(255);
    assert(strcmp(result1.c_str(), "0xff") == 0);

    // 16進数（大文字）
    auto result2 = format_hex<128>(255, true);
    assert(strcmp(result2.c_str(), "0xFF") == 0);

    // より大きな値
    auto result3 = format_hex<128>(0xDEADBEEF);
    assert(strcmp(result3.c_str(), "0xdeadbeef") == 0);

    printf("  OK\n");
}

void test_format_edge_cases() {
    printf("test_format_edge_cases...\n");

    // 小さいバッファ
    auto result1 = format<16>("Short");
    assert(strcmp(result1.c_str(), "Short") == 0);

    // 最小サイズ
    auto result2 = format<1>("{}", 'X');
    assert(strcmp(result2.c_str(), "X") == 0);

    printf("  OK\n");
}

void test_format_runtime() {
    printf("test_format_runtime...\n");

    // 実行時のフォーマット
    auto result = format<128>("Runtime: {}", 42);
    assert(strcmp(result.c_str(), "Runtime: 42") == 0);
    assert(result.capacity() == 128);

    printf("  OK\n");
}

void test_format_to() {
    printf("test_format_to...\n");

    FixedString<128> str;

    // 基本的な使用
    format_to(str, "Hello, {}!", "World");
    assert(strcmp(str.c_str(), "Hello, World!") == 0);

    // 複数の引数
    format_to(str, "Name: {}, Age: {}", "Bob", 30);
    assert(strcmp(str.c_str(), "Name: Bob, Age: 30") == 0);

    // 型の混在
    format_to(str, "Int: {}, Bool: {}", 123, false);
    assert(strcmp(str.c_str(), "Int: 123, Bool: false") == 0);

    printf("  OK\n");
}

void test_format_hex_to() {
    printf("test_format_hex_to...\n");

    FixedString<64> str;

    // 16進数フォーマット（小文字）
    format_hex_to(str, 255);
    assert(strcmp(str.c_str(), "0xff") == 0);

    // 16進数フォーマット（大文字）
    format_hex_to(str, 0xABCD, true);
    assert(strcmp(str.c_str(), "0xABCD") == 0);

    printf("  OK\n");
}

void test_fmt_wrapper() {
    printf("test_fmt_wrapper...\n");

    // デフォルト容量でのフォーマット（容量指定不要）
    auto result1 = fmt("Hello, {}!", "World");
    assert(strcmp(result1.c_str(), "Hello, World!") == 0);
    assert(result1.capacity() == 256);

    // 複数の引数
    auto result2 = fmt("x={}, y={}, z={}", 10, 20, 30);
    assert(strcmp(result2.c_str(), "x=10, y=20, z=30") == 0);

    // 型の混在
    auto result3 = fmt("String: {}, Int: {}, Bool: {}", "test", 42, true);
    assert(strcmp(result3.c_str(), "String: test, Int: 42, Bool: true") == 0);

    printf("  OK\n");
}

void test_fmt_hex_wrapper() {
    printf("test_fmt_hex_wrapper...\n");

    // デフォルト容量での16進数フォーマット
    auto result1 = fmt_hex(255);
    assert(strcmp(result1.c_str(), "0xff") == 0);
    assert(result1.capacity() == 256);

    // 大文字
    auto result2 = fmt_hex(0xDEADBEEF, true);
    assert(strcmp(result2.c_str(), "0xDEADBEEF") == 0);

    printf("  OK\n");
}

int main() {
    printf("=== Format Tests ===\n");

    test_format_basic();
    test_format_integers();
    test_format_types();
    test_format_escape();
    test_format_multiple();
    test_format_hex();
    test_format_edge_cases();
    test_format_runtime();
    test_format_to();
    test_format_hex_to();
    test_fmt_wrapper();
    test_fmt_hex_wrapper();

    printf("\nAll tests passed!\n");
    return 0;
}
