// basic_format_stringのテスト（コンパイル時検証）

#include <cassert>
#include <cstdio>
#include <cstring>

#include "../include/omusubi/core/format.hpp"

using namespace omusubi;

// コンパイル時検証のテスト
void test_format_string_valid() {
    printf("test_format_string_valid...\n");

    // プレースホルダー数と引数数が一致（コンパイル成功）
    {
        format_string<int> fs1("Value: {}");
        assert(strcmp(fs1.c_str(), "Value: {}") == 0);
        assert(fs1.length() == 9); // "Value: {}" = 9文字
    }

    {
        format_string<int, int> fs2("{} + {} = ?");
        assert(strcmp(fs2.c_str(), "{} + {} = ?") == 0);
    }

    {
        format_string<const char*, int, bool> fs3("String: {}, Int: {}, Bool: {}");
        assert(strcmp(fs3.c_str(), "String: {}, Int: {}, Bool: {}") == 0);
    }

    // プレースホルダーなし（引数なし）
    {
        format_string<> fs4("No placeholders");
        assert(strcmp(fs4.c_str(), "No placeholders") == 0);
    }

    // エスケープされた括弧は無視
    {
        format_string<int> fs5("{{Value: {}}}");
        assert(strcmp(fs5.c_str(), "{{Value: {}}}") == 0);
    }

    printf("  OK\n");
}

void test_format_string_view() {
    printf("test_format_string_view...\n");

    format_string<int, int> fs("x={}, y={}");

    // StringViewとして取得
    auto view = fs.view();
    assert(view.byte_length() == 10); // "x={}, y={}" = 10文字
    assert(view[0] == 'x');

    // C文字列として取得
    auto c_str = fs.c_str();
    assert(strcmp(c_str, "x={}, y={}") == 0);

    // 引数数の確認
    assert(fs.arg_count() == 2);

    printf("  OK\n");
}

void test_format_with_format_string() {
    printf("test_format_with_format_string...\n");

    // format_stringを使ったフォーマット
    {
        format_string<const char*> fs("Hello, {}!");
        auto result = format<128>(fs, "World");
        assert(strcmp(result.c_str(), "Hello, World!") == 0);
    }

    // 複数の引数
    {
        format_string<int, int, int> fs("{} + {} = {}");
        auto result = format<128>(fs, 1, 2, 3);
        assert(strcmp(result.c_str(), "1 + 2 = 3") == 0);
    }

    // 型の混在
    {
        format_string<const char*, int, bool> fs("String: {}, Int: {}, Bool: {}");
        auto result = format<128>(fs, "test", 42, true);
        assert(strcmp(result.c_str(), "String: test, Int: 42, Bool: true") == 0);
    }

    printf("  OK\n");
}

void test_format_to_with_format_string() {
    printf("test_format_to_with_format_string...\n");

    FixedString<128> str;

    // format_toとformat_string
    {
        format_string<const char*, int> fs("Name: {}, Age: {}");
        format_to(str, fs, "Alice", 25);
        assert(strcmp(str.c_str(), "Name: Alice, Age: 25") == 0);
    }

    {
        format_string<int, int> fs("x={}, y={}");
        format_to(str, fs, 10, 20);
        assert(strcmp(str.c_str(), "x=10, y=20") == 0);
    }

    printf("  OK\n");
}

void test_fmt_with_format_string() {
    printf("test_fmt_with_format_string...\n");

    // fmtとformat_string
    {
        format_string<const char*> fs("Hello, {}!");
        auto result = fmt(fs, "World");
        assert(strcmp(result.c_str(), "Hello, World!") == 0);
    }

    {
        format_string<int, int, int> fs("x={}, y={}, z={}");
        auto result = fmt(fs, 1, 2, 3);
        assert(strcmp(result.c_str(), "x=1, y=2, z=3") == 0);
    }

    printf("  OK\n");
}

void test_format_string_placeholder_counting() {
    printf("test_format_string_placeholder_counting...\n");

    // エスケープされた括弧のテスト
    {
        format_string<int> fs1("{{}} {}");
        auto result = format<64>(fs1, 42);
        assert(strcmp(result.c_str(), "{} 42") == 0);
    }

    {
        format_string<int, int> fs2("{{{}}} and {}");
        auto result = format<64>(fs2, 1, 2);
        assert(strcmp(result.c_str(), "{1} and 2") == 0);
    }

    printf("  OK\n");
}

// コンパイルエラーになるケース（コメントアウト）
void test_format_string_compile_errors() {
    printf("test_format_string_compile_errors...\n");

    // 以下はコンパイルエラーになるべきケース（テストでは無効化）

    // プレースホルダー数と引数数の不一致
    // format_string<int, int> fs1("{}");  // エラー: 2引数だが1プレースホルダー
    // format_string<int> fs2("{} {}");    // エラー: 1引数だが2プレースホルダー

    printf("  OK (compile-time validation skipped in runtime tests)\n");
}

int main() {
    printf("=== format_string Tests ===\n");

    test_format_string_valid();
    test_format_string_view();
    test_format_with_format_string();
    test_format_to_with_format_string();
    test_fmt_with_format_string();
    test_format_string_placeholder_counting();
    test_format_string_compile_errors();

    printf("\nAll tests passed!\n");
    return 0;
}
