#!/bin/bash

# すべてのユニットテストを実行

echo "========================================"
echo "Omusubi Framework - All Unit Tests"
echo "========================================"
echo ""

# テスト結果を追跡
TOTAL_TESTS=0
TOTAL_PASSED=0
FAILED_TESTS=""

# テストファイルのリスト
TESTS=(
    "test/core/test_optional.cpp"
    "test/core/test_result.cpp"
    "test/core/test_logger.cpp"
)

# 各テストをビルド・実行
for TEST_FILE in "${TESTS[@]}"; do
    TEST_NAME=$(basename "$TEST_FILE" .cpp)
    TEST_BIN="/tmp/${TEST_NAME}"

    echo "----------------------------------------"
    echo "Building: $TEST_NAME"
    echo "----------------------------------------"

    # ビルド
    if ! clang++ -std=c++14 -I/workspace/include "/workspace/$TEST_FILE" -o "$TEST_BIN" 2>&1; then
        echo "❌ Build failed: $TEST_FILE"
        FAILED_TESTS="${FAILED_TESTS}${TEST_NAME} (build failed)\n"
        continue
    fi

    echo "✓ Build successful"
    echo ""

    # 実行
    echo "Running: $TEST_NAME"
    echo ""

    OUTPUT=$("$TEST_BIN" 2>&1)
    EXIT_CODE=$?

    echo "$OUTPUT"
    echo ""

    if [ $EXIT_CODE -eq 0 ]; then
        # 成功したテスト数を抽出 (例: "結果: 34 / 34 テスト成功")
        PASSED=$(echo "$OUTPUT" | grep -oP '結果: \K\d+(?= / \d+ テスト成功)')
        TOTAL=$(echo "$OUTPUT" | grep -oP '結果: \d+ / \K\d+(?= テスト成功)')

        if [ -n "$PASSED" ] && [ -n "$TOTAL" ]; then
            TOTAL_TESTS=$((TOTAL_TESTS + TOTAL))
            TOTAL_PASSED=$((TOTAL_PASSED + PASSED))
        fi

        echo "✓ Test passed: $TEST_NAME"
    else
        echo "❌ Test failed: $TEST_NAME"
        FAILED_TESTS="${FAILED_TESTS}${TEST_NAME}\n"
    fi

    echo ""
done

# 最終結果
echo "========================================"
echo "Final Results"
echo "========================================"
echo ""
echo "Total tests run: $TOTAL_PASSED / $TOTAL_TESTS"
echo ""

if [ -z "$FAILED_TESTS" ]; then
    echo "✅ All tests passed!"
    exit 0
else
    echo "❌ Some tests failed:"
    echo -e "$FAILED_TESTS"
    exit 1
fi
