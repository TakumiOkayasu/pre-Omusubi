// Optional<T> のユニットテスト

#include "omusubi/core/optional.hpp"

#include "../test_framework.hpp"

using namespace omusubi;
using namespace test;

// テスト用の構造体
struct TestStruct {
    int value;

    constexpr TestStruct(int v) : value(v) {}

    constexpr bool operator==(const TestStruct& other) const { return value == other.value; }
};

void test_optional_default_constructor() {
    Optional<int> opt;
    TEST_ASSERT(!opt.has_value(), "デフォルトコンストラクタで値なし");
    TEST_ASSERT(!static_cast<bool>(opt), "bool変換でfalse");
}

void test_optional_value_constructor() {
    Optional<int> opt(42);
    TEST_ASSERT(opt.has_value(), "値コンストラクタで値あり");
    TEST_ASSERT(static_cast<bool>(opt), "bool変換でtrue");
    TEST_ASSERT_EQ(opt.value(), 42, "値が正しい");
}

void test_optional_copy_constructor() {
    Optional<int> opt1(42);
    Optional<int> opt2(opt1);
    TEST_ASSERT(opt2.has_value(), "コピーコンストラクタで値あり");
    TEST_ASSERT_EQ(opt2.value(), 42, "コピーされた値が正しい");
}

void test_optional_copy_constructor_empty() {
    Optional<int> opt1;
    Optional<int> opt2(opt1);
    TEST_ASSERT(!opt2.has_value(), "空のOptionalをコピー");
}

void test_optional_copy_assignment() {
    Optional<int> opt1(42);
    Optional<int> opt2(10);
    opt2 = opt1;
    TEST_ASSERT(opt2.has_value(), "コピー代入で値あり");
    TEST_ASSERT_EQ(opt2.value(), 42, "コピー代入された値が正しい");
}

void test_optional_copy_assignment_empty() {
    Optional<int> opt1;
    Optional<int> opt2(10);
    opt2 = opt1;
    TEST_ASSERT(!opt2.has_value(), "空をコピー代入");
}

void test_optional_value_or() {
    Optional<int> opt1(42);
    Optional<int> opt2;
    TEST_ASSERT_EQ(opt1.value_or(0), 42, "value_or: 値ありの場合");
    TEST_ASSERT_EQ(opt2.value_or(99), 99, "value_or: 値なしの場合");
}

void test_optional_operator_arrow() {
    Optional<TestStruct> opt(TestStruct(42));
    TEST_ASSERT_EQ(opt->value, 42, "operator->で値アクセス");
}

void test_optional_operator_dereference() {
    Optional<int> opt(42);
    TEST_ASSERT_EQ(*opt, 42, "operator*で値アクセス");
}

void test_optional_reset() {
    Optional<int> opt(42);
    TEST_ASSERT(opt.has_value(), "reset前は値あり");
    opt.reset();
    TEST_ASSERT(!opt.has_value(), "reset後は値なし");
}

void test_optional_emplace() {
    Optional<int> opt;
    TEST_ASSERT(!opt.has_value(), "emplace前は値なし");
    opt.emplace(42);
    TEST_ASSERT(opt.has_value(), "emplace後は値あり");
    TEST_ASSERT_EQ(opt.value(), 42, "emplaceした値が正しい");
}

void test_optional_equality_both_empty() {
    Optional<int> opt1;
    Optional<int> opt2;
    TEST_ASSERT(opt1 == opt2, "両方空で等価");
    TEST_ASSERT(!(opt1 != opt2), "両方空で非等価でない");
}

void test_optional_equality_both_value() {
    Optional<int> opt1(42);
    Optional<int> opt2(42);
    TEST_ASSERT(opt1 == opt2, "同じ値で等価");
    TEST_ASSERT(!(opt1 != opt2), "同じ値で非等価でない");
}

void test_optional_equality_different_values() {
    Optional<int> opt1(42);
    Optional<int> opt2(10);
    TEST_ASSERT(opt1 != opt2, "異なる値で非等価");
    TEST_ASSERT(!(opt1 == opt2), "異なる値で等価でない");
}

void test_optional_equality_one_empty() {
    Optional<int> opt1(42);
    Optional<int> opt2;
    TEST_ASSERT(opt1 != opt2, "片方空で非等価");
    TEST_ASSERT(!(opt1 == opt2), "片方空で等価でない");
}

void test_optional_value_equality() {
    Optional<int> opt(42);
    TEST_ASSERT(opt == 42, "値との等価比較: 等しい");
    TEST_ASSERT(!(opt != 42), "値との非等価比較: 等しい場合");
    TEST_ASSERT(opt != 10, "値との非等価比較: 異なる");
    TEST_ASSERT(!(opt == 10), "値との等価比較: 異なる場合");
}

void test_optional_value_equality_empty() {
    Optional<int> opt;
    TEST_ASSERT(opt != 42, "空と値の非等価");
    TEST_ASSERT(!(opt == 42), "空と値は等価でない");
}

int main() {
    begin_tests("Optional<T>");

    test_optional_default_constructor();
    test_optional_value_constructor();
    test_optional_copy_constructor();
    test_optional_copy_constructor_empty();
    test_optional_copy_assignment();
    test_optional_copy_assignment_empty();
    test_optional_value_or();
    test_optional_operator_arrow();
    test_optional_operator_dereference();
    test_optional_reset();
    test_optional_emplace();
    test_optional_equality_both_empty();
    test_optional_equality_both_value();
    test_optional_equality_different_values();
    test_optional_equality_one_empty();
    test_optional_value_equality();
    test_optional_value_equality_empty();

    return end_tests();
}
