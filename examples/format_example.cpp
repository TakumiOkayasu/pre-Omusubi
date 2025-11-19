// format関数の使用例

#include <cstdio>

#include "../include/omusubi/core/format.hpp"

using namespace omusubi;

int main() {
    printf("=== Format Example ===\n\n");

    printf("--- テンプレート引数を明示する方法 ---\n");

    // 基本的な使用例
    auto msg1 = format<128>("Hello, {}!", "World");
    printf("%s\n", msg1.c_str());

    // 複数の引数
    auto msg2 = format<128>("Name: {}, Age: {}", "Alice", 25);
    printf("%s\n", msg2.c_str());

    // 様々な型
    auto msg3 = format<128>("Int: {}, Bool: {}, Char: {}", 42, true, 'X');
    printf("%s\n", msg3.c_str());

    // 16進数フォーマット
    auto hex1 = format_hex<64>(255);
    printf("%s\n", hex1.c_str());

    auto hex2 = format_hex<64>(0xDEADBEEF, true);
    printf("%s\n", hex2.c_str());

    printf("\n--- テンプレート引数を隠蔽する方法（推奨） ---\n");

    // FixedStringを事前に宣言してformat_toを使用
    FixedString<128> str;

    // 基本的な使用
    format_to(str, "Hello, {}!", "World");
    printf("%s\n", str.c_str());

    // 複数の引数
    format_to(str, "Name: {}, Age: {}", "Bob", 30);
    printf("%s\n", str.c_str());

    // 様々な型
    format_to(str, "Int: {}, Bool: {}, Char: {}", 99, false, 'Z');
    printf("%s\n", str.c_str());

    // 16進数フォーマット
    FixedString<64> hex_str;
    format_hex_to(hex_str, 0xCAFEBABE);
    printf("%s\n", hex_str.c_str());

    format_hex_to(hex_str, 0xFEED, true);
    printf("%s\n", hex_str.c_str());

    // エスケープ
    format_to(str, "Escaped: {{}} and value: {}", 100);
    printf("%s\n", str.c_str());

    printf("\n--- デフォルト容量ラッパー関数（最も簡潔） ---\n");

    // fmt() - Capacity指定不要（デフォルト256）
    auto simple1 = fmt("Hello, {}!", "World");
    printf("%s\n", simple1.c_str());

    // 複数の引数
    auto simple2 = fmt("x={}, y={}, z={}", 1, 2, 3);
    printf("%s\n", simple2.c_str());

    // 様々な型
    auto simple3 = fmt("String: {}, Int: {}, Bool: {}", "test", 42, true);
    printf("%s\n", simple3.c_str());

    // 16進数フォーマット（デフォルト容量）
    auto hex_simple1 = fmt_hex(255);
    printf("%s\n", hex_simple1.c_str());

    auto hex_simple2 = fmt_hex(0xDEADBEEF, true);
    printf("%s\n", hex_simple2.c_str());

    printf("\n--- パフォーマンス重視の使用例 ---\n");

    // 再利用可能なバッファ
    FixedString<256> buffer;

    for (uint32_t i = 0; i < 5; ++i) {
        format_to(buffer, "Iteration: {}, Value: {}", i, i * 10);
        printf("%s\n", buffer.c_str());
    }

    printf("\n--- 型安全なformat_string（C++23互換） ---\n");

    // basic_format_stringを使った型安全なフォーマット
    // テンプレート引数で型を明示することで、コンパイル時に型チェックが可能
    {
        format_string<const char*> fs("Hello, {}!");
        auto result = fmt(fs, "World");
        printf("%s\n", result.c_str());
    }

    // 複数の型を指定
    {
        format_string<const char*, int, bool> fs("Name: {}, Age: {}, Active: {}");
        auto result = fmt(fs, "Alice", 25, true);
        printf("%s\n", result.c_str());
    }

    // 引数数の取得
    {
        format_string<int, int, int> fs("x={}, y={}, z={}");
        printf("This format string expects %u arguments\n", fs.arg_count());
    }

    printf("\n--- 使い分けのガイドライン ---\n");
    printf("1. fmt() - 最も簡潔、一般的な用途向け（容量256で十分な場合）\n");
    printf("2. format_to() - バッファ再利用、パフォーマンス重視\n");
    printf("3. format<N>() - 容量を厳密に制御したい場合\n");
    printf("4. format_string<Args...> - 型安全性が必要な場合（C++23互換）\n");

    return 0;
}
