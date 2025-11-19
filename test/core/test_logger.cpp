// Logger のユニットテスト

#include <omusubi/core/fixed_string.hpp>
#include <omusubi/core/logger.hpp>

#include "../test_framework.hpp"

using namespace omusubi;
using namespace test;

// ========================================
// モックLogOutput実装
// ========================================

class MockLogOutput : public LogOutput {
private:
    FixedString<256> last_message_;
    LogLevel last_level_;
    uint32_t write_count_;
    uint32_t flush_count_;

public:
    MockLogOutput() : last_message_(), last_level_(LogLevel::DEBUG), write_count_(0), flush_count_(0) {}

    void write(LogLevel level, StringView message) override {
        last_level_ = level;
        last_message_.clear();
        last_message_.append(message);
        write_count_++;
    }

    void flush() override { flush_count_++; }

    // テスト用アクセッサ
    StringView get_last_message() const { return last_message_.view(); }

    LogLevel get_last_level() const { return last_level_; }

    uint32_t get_write_count() const { return write_count_; }

    uint32_t get_flush_count() const { return flush_count_; }

    void reset() {
        last_message_.clear();
        last_level_ = LogLevel::DEBUG;
        write_count_ = 0;
        flush_count_ = 0;
    }
};

// ========================================
// 基本的なログ出力
// ========================================

void test_logger_basic_output() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.info(StringView("Hello", 5));

    TEST_ASSERT_EQ(output.get_write_count(), 1U, "ログが1回出力された");
    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::INFO), "INFOレベルで出力");

    // StringView比較
    auto msg = output.get_last_message();
    TEST_ASSERT_EQ(msg.byte_length(), 5U, "メッセージ長が正しい");
    bool msg_equal = (msg.data()[0] == 'H' && msg.data()[1] == 'e' && msg.data()[2] == 'l' && msg.data()[3] == 'l' && msg.data()[4] == 'o');
    TEST_ASSERT(msg_equal, "メッセージ内容が正しい");
}

// ========================================
// ログレベルフィルタリング
// ========================================

void test_logger_level_filtering_blocks_lower() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::WARNING);

    logger.debug(StringView("debug", 5));
    logger.info(StringView("info", 4));

    TEST_ASSERT_EQ(output.get_write_count(), 0U, "WARNINGレベル以下は出力されない");
}

void test_logger_level_filtering_allows_higher() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::WARNING);

    logger.warning(StringView("warn", 4));
    logger.error(StringView("error", 5));
    logger.critical(StringView("crit", 4));

    TEST_ASSERT_EQ(output.get_write_count(), 3U, "WARNING以上は出力される");
}

// ========================================
// 各ログレベルメソッド
// ========================================

void test_logger_debug_method() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.debug(StringView("debug message", 13));

    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::DEBUG), "DEBUGレベル");
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "1回出力");
}

void test_logger_info_method() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.info(StringView("info message", 12));

    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::INFO), "INFOレベル");
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "1回出力");
}

void test_logger_warning_method() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.warning(StringView("warning message", 15));

    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::WARNING), "WARNINGレベル");
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "1回出力");
}

void test_logger_error_method() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.error(StringView("error message", 13));

    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::ERROR), "ERRORレベル");
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "1回出力");
}

void test_logger_critical_method() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.critical(StringView("critical message", 16));

    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::CRITICAL), "CRITICALレベル");
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "1回出力");
}

// ========================================
// 最小ログレベル変更
// ========================================

void test_logger_set_min_level() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    TEST_ASSERT_EQ(static_cast<uint8_t>(logger.get_min_level()), static_cast<uint8_t>(LogLevel::DEBUG), "初期レベルはDEBUG");

    logger.set_min_level(LogLevel::ERROR);
    TEST_ASSERT_EQ(static_cast<uint8_t>(logger.get_min_level()), static_cast<uint8_t>(LogLevel::ERROR), "レベルをERRORに変更");

    logger.info(StringView("info", 4));
    TEST_ASSERT_EQ(output.get_write_count(), 0U, "INFOは出力されない");

    logger.error(StringView("error", 5));
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "ERRORは出力される");
}

// ========================================
// nullptr出力先
// ========================================

void test_logger_null_output() {
    Logger logger(nullptr, LogLevel::DEBUG);

    // クラッシュせずに完了すればOK
    logger.info(StringView("test", 4));
    logger.flush();

    TEST_ASSERT(true, "nullptr出力でもクラッシュしない");
}

// ========================================
// flush() メソッド
// ========================================

void test_logger_flush() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    logger.flush();
    TEST_ASSERT_EQ(output.get_flush_count(), 1U, "flushが呼ばれた");

    logger.flush();
    logger.flush();
    TEST_ASSERT_EQ(output.get_flush_count(), 3U, "複数回flush可能");
}

// ========================================
// log_level_to_string() 関数
// ========================================

void test_log_level_to_string_debug() {
    auto str = log_level_to_string(LogLevel::DEBUG);
    TEST_ASSERT_EQ(str.byte_length(), 5U, "DEBUG文字列長");
    bool equal = (str.data()[0] == 'D' && str.data()[1] == 'E' && str.data()[2] == 'B' && str.data()[3] == 'U' && str.data()[4] == 'G');
    TEST_ASSERT(equal, "DEBUG文字列内容");
}

void test_log_level_to_string_info() {
    auto str = log_level_to_string(LogLevel::INFO);
    TEST_ASSERT_EQ(str.byte_length(), 4U, "INFO文字列長");
    bool equal = (str.data()[0] == 'I' && str.data()[1] == 'N' && str.data()[2] == 'F' && str.data()[3] == 'O');
    TEST_ASSERT(equal, "INFO文字列内容");
}

void test_log_level_to_string_warning() {
    auto str = log_level_to_string(LogLevel::WARNING);
    TEST_ASSERT_EQ(str.byte_length(), 4U, "WARN文字列長");
    bool equal = (str.data()[0] == 'W' && str.data()[1] == 'A' && str.data()[2] == 'R' && str.data()[3] == 'N');
    TEST_ASSERT(equal, "WARN文字列内容");
}

void test_log_level_to_string_error() {
    auto str = log_level_to_string(LogLevel::ERROR);
    TEST_ASSERT_EQ(str.byte_length(), 5U, "ERROR文字列長");
    bool equal = (str.data()[0] == 'E' && str.data()[1] == 'R' && str.data()[2] == 'R' && str.data()[3] == 'O' && str.data()[4] == 'R');
    TEST_ASSERT(equal, "ERROR文字列内容");
}

void test_log_level_to_string_critical() {
    auto str = log_level_to_string(LogLevel::CRITICAL);
    TEST_ASSERT_EQ(str.byte_length(), 4U, "CRIT文字列長");
    bool equal = (str.data()[0] == 'C' && str.data()[1] == 'R' && str.data()[2] == 'I' && str.data()[3] == 'T');
    TEST_ASSERT(equal, "CRIT文字列内容");
}

// ========================================
// log_at<Level>() テンプレート関数
// ========================================

void test_log_at_info() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    log_at<LogLevel::INFO>(logger, StringView("template log", 12));

    TEST_ASSERT_EQ(output.get_write_count(), 1U, "log_at<INFO>で出力");
    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::INFO), "INFOレベル");
}

void test_log_at_error() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    log_at<LogLevel::ERROR>(logger, StringView("error log", 9));

    TEST_ASSERT_EQ(output.get_write_count(), 1U, "log_at<ERROR>で出力");
    TEST_ASSERT_EQ(static_cast<uint8_t>(output.get_last_level()), static_cast<uint8_t>(LogLevel::ERROR), "ERRORレベル");
}

// ========================================
// log_at<DEBUG>() のフィルタリング（デバッグビルド時のみ）
// ========================================

void test_log_at_debug_in_debug_build() {
    MockLogOutput output;
    Logger logger(&output, LogLevel::DEBUG);

    log_at<LogLevel::DEBUG>(logger, StringView("debug log", 9));

#ifdef NDEBUG
    // リリースビルドではDEBUGログは削除される
    TEST_ASSERT_EQ(output.get_write_count(), 0U, "リリースビルド: DEBUGログ削除");
#else
    // デバッグビルドではDEBUGログが出力される
    TEST_ASSERT_EQ(output.get_write_count(), 1U, "デバッグビルド: DEBUGログ出力");
#endif
}

int main() {
    begin_tests("Logger");

    // 基本出力
    test_logger_basic_output();

    // レベルフィルタリング
    test_logger_level_filtering_blocks_lower();
    test_logger_level_filtering_allows_higher();

    // 各ログレベルメソッド
    test_logger_debug_method();
    test_logger_info_method();
    test_logger_warning_method();
    test_logger_error_method();
    test_logger_critical_method();

    // 最小レベル変更
    test_logger_set_min_level();

    // nullptr出力
    test_logger_null_output();

    // flush
    test_logger_flush();

    // log_level_to_string
    test_log_level_to_string_debug();
    test_log_level_to_string_info();
    test_log_level_to_string_warning();
    test_log_level_to_string_error();
    test_log_level_to_string_critical();

    // log_at
    test_log_at_info();
    test_log_at_error();
    test_log_at_debug_in_debug_build();

    return end_tests();
}
