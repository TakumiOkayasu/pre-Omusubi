// Capacity自動計算のテスト

#include <cassert>
#include <cstdio>
#include <cstring>

#include "../include/omusubi/core/format.hpp"

using namespace omusubi;

void test_auto_capacity_basic() {
    printf("test_auto_capacity_basic...\n");

    // Capacity自動計算版のformat
    {
        auto str = format("Value: {}", 42);
        assert(strcmp(str.c_str(), "Value: 42") == 0);
        printf("  Capacity: %u\n", str.capacity());
    }

    {
        auto str = format("Hello, {}!", "World");
        assert(strcmp(str.c_str(), "Hello, World!") == 0);
        printf("  Capacity: %u\n", str.capacity());
    }

    {
        auto str = format("x={}, y={}", 10, 20);
        assert(strcmp(str.c_str(), "x=10, y=20") == 0);
        printf("  Capacity: %u\n", str.capacity());
    }

    printf("  OK\n");
}

void test_auto_capacity_types() {
    printf("test_auto_capacity_types...\n");

    // 様々な型での自動計算
    {
        auto str = format("Bool: {}", true);
        assert(strcmp(str.c_str(), "Bool: true") == 0);
        printf("  Bool capacity: %u\n", str.capacity());
    }

    {
        auto str = format("Char: {}", 'A');
        assert(strcmp(str.c_str(), "Char: A") == 0);
        printf("  Char capacity: %u\n", str.capacity());
    }

    {
        auto str = format("Int: {}, Bool: {}, Char: {}", 123, false, 'X');
        assert(strcmp(str.c_str(), "Int: 123, Bool: false, Char: X") == 0);
        printf("  Mixed capacity: %u\n", str.capacity());
    }

    printf("  OK\n");
}

void test_auto_capacity_format_to() {
    printf("test_auto_capacity_format_to...\n");

    // format_toの自動容量版
    {
        auto str = format_to("Value: {}", 42);
        assert(strcmp(str.c_str(), "Value: 42") == 0);
        printf("  Capacity: %u\n", str.capacity());
    }

    {
        auto str = format_to("Name: {}, Age: {}", "Alice", 25);
        assert(strcmp(str.c_str(), "Name: Alice, Age: 25") == 0);
        printf("  Capacity: %u\n", str.capacity());
    }

    printf("  OK\n");
}

void test_capacity_calculation() {
    printf("test_capacity_calculation...\n");

    // 容量計算の詳細確認
    {
        // "Value: {}" + int32_t(最大11文字) = 7 + 11 + 1(null) = 19
        auto str = format("Value: {}", 42);
        printf("  'Value: {}' with int32_t: capacity=%u, actual='%s'\n", str.capacity(), str.c_str());
        // 実際の出力は "Value: 42" = 9文字だが、容量は最大を想定
    }

    {
        // "x={}, y={}" = 9文字 + int32_t * 2 = 9 + 11 + 11 + 1 = 32
        auto str = format("x={}, y={}", 10, 20);
        printf("  'x={}, y={}' with int32_t*2: capacity=%u, actual='%s'\n", str.capacity(), str.c_str());
    }

    {
        // "Bool: {}" + bool(最大5文字) = 6 + 5 + 1 = 12
        auto str = format("Bool: {}", true);
        printf("  'Bool: {}' with bool: capacity=%u, actual='%s'\n", str.capacity(), str.c_str());
    }

    printf("  OK\n");
}

void test_explicit_vs_auto() {
    printf("test_explicit_vs_auto...\n");

    // 明示的指定と自動計算の比較
    {
        // 明示的指定
        auto str1 = format<128>("Value: {}", 42);
        printf("  Explicit capacity=128: '%s'\n", str1.c_str());

        // 自動計算
        auto str2 = format("Value: {}", 42);
        printf("  Auto capacity=%u: '%s'\n", str2.capacity(), str2.c_str());

        assert(strcmp(str1.c_str(), str2.c_str()) == 0);
    }

    printf("  OK\n");
}

void test_max_string_length() {
    printf("test_max_string_length...\n");

    // 型ごとの最大文字列長の確認
    printf("  int8_t: %u\n", detail::max_string_length<int8_t>::value);
    printf("  uint8_t: %u\n", detail::max_string_length<uint8_t>::value);
    printf("  int16_t: %u\n", detail::max_string_length<int16_t>::value);
    printf("  uint16_t: %u\n", detail::max_string_length<uint16_t>::value);
    printf("  int32_t: %u\n", detail::max_string_length<int32_t>::value);
    printf("  uint32_t: %u\n", detail::max_string_length<uint32_t>::value);
    printf("  int64_t: %u\n", detail::max_string_length<int64_t>::value);
    printf("  uint64_t: %u\n", detail::max_string_length<uint64_t>::value);
    printf("  bool: %u\n", detail::max_string_length<bool>::value);
    printf("  char: %u\n", detail::max_string_length<char>::value);
    printf("  const char*: %u\n", detail::max_string_length<const char*>::value);
    printf("  StringView: %u\n", detail::max_string_length<StringView>::value);

    printf("  OK\n");
}

int main() {
    printf("=== Auto Capacity Tests ===\n");

    test_auto_capacity_basic();
    test_auto_capacity_types();
    test_auto_capacity_format_to();
    test_capacity_calculation();
    test_explicit_vs_auto();
    test_max_string_length();

    printf("\nAll tests passed!\n");
    return 0;
}
