// Result<T, E> のユニットテスト

#include "omusubi/core/result.hpp"

#include "../test_framework.hpp"

using namespace omusubi;
using namespace test;

// テスト用の構造体
struct TestStruct {
    int value;

    constexpr TestStruct(int v) : value(v) {}

    constexpr bool operator==(const TestStruct& other) const { return value == other.value; }
};

// カスタムエラー型
enum class CustomError : uint8_t { ERROR_A = 1, ERROR_B = 2, ERROR_C = 3 };

// ========================================
// Result::ok() ファクトリメソッド
// ========================================

void test_result_ok_factory() {
    auto result = Result<int>::ok(42);
    TEST_ASSERT(result.is_ok(), "ok()で成功を作成");
    TEST_ASSERT(!result.is_err(), "is_err()はfalse");
    TEST_ASSERT_EQ(result.value(), 42, "値が正しい");
}

void test_result_ok_with_struct() {
    auto result = Result<TestStruct>::ok(TestStruct(100));
    TEST_ASSERT(result.is_ok(), "構造体でok()成功");
    TEST_ASSERT_EQ(result.value().value, 100, "構造体の値が正しい");
}

// ========================================
// Result::err() ファクトリメソッド
// ========================================

void test_result_err_factory() {
    auto result = Result<int>::err(Error::NOT_CONNECTED);
    TEST_ASSERT(result.is_err(), "err()でエラーを作成");
    TEST_ASSERT(!result.is_ok(), "is_ok()はfalse");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result.error()), static_cast<uint8_t>(Error::NOT_CONNECTED), "エラー値が正しい");
}

void test_result_err_custom_error() {
    auto result = Result<int, CustomError>::err(CustomError::ERROR_B);
    TEST_ASSERT(result.is_err(), "カスタムエラー型でerr()成功");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result.error()), static_cast<uint8_t>(CustomError::ERROR_B), "カスタムエラー値が正しい");
}

// ========================================
// コピーコンストラクタ
// ========================================

void test_result_copy_constructor_ok() {
    auto result1 = Result<int>::ok(42);
    auto result2(result1);
    TEST_ASSERT(result2.is_ok(), "成功をコピー: is_ok()");
    TEST_ASSERT_EQ(result2.value(), 42, "成功をコピー: 値が正しい");
}

void test_result_copy_constructor_err() {
    auto result1 = Result<int>::err(Error::TIMEOUT);
    auto result2(result1);
    TEST_ASSERT(result2.is_err(), "エラーをコピー: is_err()");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result2.error()), static_cast<uint8_t>(Error::TIMEOUT), "エラーをコピー: エラー値が正しい");
}

// ========================================
// ムーブコンストラクタ
// ========================================

void test_result_move_constructor_ok() {
    auto result1 = Result<int>::ok(42);
    auto result2(static_cast<Result<int>&&>(result1));
    TEST_ASSERT(result2.is_ok(), "成功をムーブ: is_ok()");
    TEST_ASSERT_EQ(result2.value(), 42, "成功をムーブ: 値が正しい");
}

void test_result_move_constructor_err() {
    auto result1 = Result<int>::err(Error::BUFFER_FULL);
    auto result2(static_cast<Result<int>&&>(result1));
    TEST_ASSERT(result2.is_err(), "エラーをムーブ: is_err()");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result2.error()), static_cast<uint8_t>(Error::BUFFER_FULL), "エラーをムーブ: エラー値が正しい");
}

// ========================================
// コピー代入演算子
// ========================================

void test_result_copy_assignment_ok_to_ok() {
    auto result1 = Result<int>::ok(42);
    auto result2 = Result<int>::ok(10);
    result2 = result1;
    TEST_ASSERT(result2.is_ok(), "成功→成功: is_ok()");
    TEST_ASSERT_EQ(result2.value(), 42, "成功→成功: 値が正しい");
}

void test_result_copy_assignment_err_to_ok() {
    auto result1 = Result<int>::err(Error::INVALID_PARAMETER);
    auto result2 = Result<int>::ok(10);
    result2 = result1;
    TEST_ASSERT(result2.is_err(), "エラー→成功: is_err()");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result2.error()), static_cast<uint8_t>(Error::INVALID_PARAMETER), "エラー→成功: エラー値が正しい");
}

void test_result_copy_assignment_ok_to_err() {
    auto result1 = Result<int>::ok(99);
    auto result2 = Result<int>::err(Error::TIMEOUT);
    result2 = result1;
    TEST_ASSERT(result2.is_ok(), "成功→エラー: is_ok()");
    TEST_ASSERT_EQ(result2.value(), 99, "成功→エラー: 値が正しい");
}

void test_result_copy_assignment_err_to_err() {
    auto result1 = Result<int>::err(Error::READ_FAILED);
    auto result2 = Result<int>::err(Error::WRITE_FAILED);
    result2 = result1;
    TEST_ASSERT(result2.is_err(), "エラー→エラー: is_err()");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result2.error()), static_cast<uint8_t>(Error::READ_FAILED), "エラー→エラー: エラー値が正しい");
}

// ========================================
// ムーブ代入演算子
// ========================================

void test_result_move_assignment_ok() {
    auto result1 = Result<int>::ok(42);
    auto result2 = Result<int>::ok(10);
    result2 = static_cast<Result<int>&&>(result1);
    TEST_ASSERT(result2.is_ok(), "成功をムーブ代入: is_ok()");
    TEST_ASSERT_EQ(result2.value(), 42, "成功をムーブ代入: 値が正しい");
}

void test_result_move_assignment_err() {
    auto result1 = Result<int>::err(Error::FILE_NOT_FOUND);
    auto result2 = Result<int>::ok(10);
    result2 = static_cast<Result<int>&&>(result1);
    TEST_ASSERT(result2.is_err(), "エラーをムーブ代入: is_err()");
    TEST_ASSERT_EQ(static_cast<uint8_t>(result2.error()), static_cast<uint8_t>(Error::FILE_NOT_FOUND), "エラーをムーブ代入: エラー値が正しい");
}

// ========================================
// value_or() メソッド
// ========================================

void test_result_value_or_ok() {
    auto result = Result<int>::ok(42);
    TEST_ASSERT_EQ(result.value_or(0), 42, "value_or: 成功時は元の値");
}

void test_result_value_or_err() {
    auto result = Result<int>::err(Error::UNKNOWN);
    TEST_ASSERT_EQ(result.value_or(99), 99, "value_or: エラー時はデフォルト値");
}

// ========================================
// bool変換演算子
// ========================================

void test_result_bool_conversion_ok() {
    auto result = Result<int>::ok(42);
    TEST_ASSERT(static_cast<bool>(result), "bool変換: 成功時true");
}

void test_result_bool_conversion_err() {
    auto result = Result<int>::err(Error::TIMEOUT);
    TEST_ASSERT(!static_cast<bool>(result), "bool変換: エラー時false");
}

// ========================================
// operator* (間接参照)
// ========================================

void test_result_operator_dereference() {
    auto result = Result<int>::ok(42);
    TEST_ASSERT_EQ(*result, 42, "operator*で値アクセス");
}

void test_result_operator_dereference_const() {
    const auto result = Result<int>::ok(42);
    TEST_ASSERT_EQ(*result, 42, "operator* (const) で値アクセス");
}

// ========================================
// operator-> (ポインタ風アクセス)
// ========================================

void test_result_operator_arrow() {
    auto result = Result<TestStruct>::ok(TestStruct(42));
    TEST_ASSERT_EQ(result->value, 42, "operator->で値アクセス");
}

void test_result_operator_arrow_const() {
    const auto result = Result<TestStruct>::ok(TestStruct(42));
    TEST_ASSERT_EQ(result->value, 42, "operator-> (const) で値アクセス");
}

// ========================================
// エラー型のカスタマイズ
// ========================================

void test_result_custom_error_type() {
    auto ok_result = Result<int, CustomError>::ok(100);
    auto err_result = Result<int, CustomError>::err(CustomError::ERROR_C);

    TEST_ASSERT(ok_result.is_ok(), "カスタムエラー型: 成功");
    TEST_ASSERT_EQ(ok_result.value(), 100, "カスタムエラー型: 成功値");
    TEST_ASSERT(err_result.is_err(), "カスタムエラー型: エラー");
    TEST_ASSERT_EQ(static_cast<uint8_t>(err_result.error()), static_cast<uint8_t>(CustomError::ERROR_C), "カスタムエラー型: エラー値");
}

// ========================================
// 複雑な型でのテスト
// ========================================

void test_result_with_struct() {
    auto result = Result<TestStruct>::ok(TestStruct(999));
    TEST_ASSERT(result.is_ok(), "構造体Result: is_ok()");
    TEST_ASSERT_EQ(result.value().value, 999, "構造体Result: 値が正しい");
    TEST_ASSERT_EQ(result->value, 999, "構造体Result: operator->でアクセス");
}

int main() {
    begin_tests("Result<T, E>");

    // ファクトリメソッド
    test_result_ok_factory();
    test_result_ok_with_struct();
    test_result_err_factory();
    test_result_err_custom_error();

    // コピー/ムーブコンストラクタ
    test_result_copy_constructor_ok();
    test_result_copy_constructor_err();
    test_result_move_constructor_ok();
    test_result_move_constructor_err();

    // コピー/ムーブ代入
    test_result_copy_assignment_ok_to_ok();
    test_result_copy_assignment_err_to_ok();
    test_result_copy_assignment_ok_to_err();
    test_result_copy_assignment_err_to_err();
    test_result_move_assignment_ok();
    test_result_move_assignment_err();

    // value_or()
    test_result_value_or_ok();
    test_result_value_or_err();

    // bool変換
    test_result_bool_conversion_ok();
    test_result_bool_conversion_err();

    // operator*
    test_result_operator_dereference();
    test_result_operator_dereference_const();

    // operator->
    test_result_operator_arrow();
    test_result_operator_arrow_const();

    // カスタムエラー型
    test_result_custom_error_type();

    // 構造体
    test_result_with_struct();

    return end_tests();
}
