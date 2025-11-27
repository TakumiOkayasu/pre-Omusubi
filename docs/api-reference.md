# APIリファレンス

Omusubi APIの完全なリファレンスドキュメントです。

## 目次

- [Core Types](#core-types)
- [Interfaces](#interfaces)
- [Contexts](#contexts)
- [SystemContext](#systemcontext)

## Core Types

### std::string_view（UTF-8ヘルパー付き）

標準ライブラリの`std::string_view`を使用し、UTF-8ヘルパー関数を提供。

```cpp
// std::string_viewを直接使用
#include <string_view>
#include <omusubi/core/string_view.h>  // UTF-8ヘルパー

// omusubiが提供するヘルパー関数
namespace omusubi {
    // UTF-8文字数を取得
    constexpr uint32_t char_length(std::string_view sv) noexcept;

    // UTF-8文字インデックスからバイト位置を取得
    constexpr uint32_t get_char_position(std::string_view sv, uint32_t char_index) noexcept;

    // 空かどうか判定
    constexpr bool is_empty(std::string_view sv) noexcept;

    // 等価判定
    constexpr bool equals(std::string_view a, std::string_view b) noexcept;

    // C文字列からstd::string_viewを構築
    std::string_view from_c_string(const char* str) noexcept;
}

```

**使用例:**

```cpp
// 文字列リテラル（標準ライブラリの_svを使用）
using namespace std::literals;
std::string_view msg = "Hello"sv;

// C文字列から
const char* str = "World";
std::string_view view = omusubi::from_c_string(str);

// 比較
if (msg == "Hello"sv) {
    // ...
}

// UTF-8文字数（日本語など）
std::string_view japanese = "こんにちは"sv;
uint32_t char_count = omusubi::char_length(japanese);  // 5
```

### FixedString<N>

固定長のUTF-8文字列バッファ。ヒープを使わずにスタック上に確保。

```cpp
// 容量を指定して作成
FixedString<256> str;
str.append("Hello"sv);
str.append(" World"sv);

// ヘルパー関数でサイズ自動推論
auto str2 = fixed_string("Hello");  // FixedString<5>

// std::string_view / C文字列として取得
std::string_view view = str.view();
const char* cstr = str.c_str();
```

### FixedBuffer<N>

固定長のバイトバッファ。

```cpp
FixedBuffer<1024> buffer;
buffer.append(0x01);
buffer.append(0x02);

const uint8_t* data = buffer.data();
uint32_t len = buffer.length();
```

### span<T>

非所有のメモリビュー（C++20 std::span相当）。ゼロコピーでデータを渡す。

```cpp
uint8_t buffer[256];
span<uint8_t> s(buffer, 256);

// 関数に渡す
void process(span<const uint8_t> data) {
    for (uint8_t byte : data) { /* ... */ }
}
process(s);
```

### Vector3

3次元ベクトル（センサーデータ用）。`float x, y, z` メンバーを持つ。

```cpp
Vector3 accel = sensor->get_values();
```

### PowerState

電源状態の列挙型。

```cpp
enum class PowerState : uint8_t {
    BATTERY,   // バッテリー駆動
    USB,       // USB給電
    EXTERNAL,  // 外部電源
    UNKNOWN    // 不明
};
```

### ButtonState

ボタン状態の列挙型。

```cpp
enum class ButtonState : uint8_t {
    PRESSED,   // 押されている
    RELEASED   // 離されている
};
```

### format()

型安全な文字列フォーマット関数。`{}` でプレースホルダーを指定。

```cpp
auto msg = format("Hello, {}!", "World");          // "Hello, World!"
auto hex = format("Value: 0x{:X}", 255);           // "Value: 0xFF"
auto log = format("[{}] {}", "INFO", "started");   // "[INFO] started"
```

**フォーマット指定子:** `{}`, `{:d}`, `{:x}`, `{:X}`, `{:b}`, `{:f}`, `{:s}`

### Result<T, E>

Rust風のエラーハンドリング型。例外を使わずにエラーを返す。

```cpp
Result<uint32_t, Error> read_sensor() {
    if (!ready) return Result<uint32_t, Error>::err(Error::NOT_CONNECTED);
    return Result<uint32_t, Error>::ok(value);
}

auto result = read_sensor();
if (result.is_ok()) {
    process(result.value());
}

// デフォルト値付き
uint32_t value = read_sensor().value_or(0);
```

### Logger

シングルトンパターンによるグローバルログ機能。

```cpp
// 初期化（setup()で1回）
static SerialLogOutput log_output(serial);
get_logger().set_output(&log_output);

// グローバル関数でログ出力
log<LogLevel::INFO>("System running"sv);
log<LogLevel::DEBUG>("Debug info"sv);
log<LogLevel::WARNING>("Low memory"sv);
log<LogLevel::ERROR>("Connection failed"sv);

// format()と組み合わせ
auto msg = format("Sensor: {}", value);
log<LogLevel::DEBUG>(msg.view());

```

**ポイント:** リリースビルド（`NDEBUG`）ではDEBUGログは完全削除される。

## Interfaces

インターフェースはヘッダーファイル（`include/omusubi/interface/`）を参照。

| インターフェース | 機能 |
|-----------------|------|
| `ByteReadable` | バイト読み取り（`read(span<uint8_t>)`） |
| `TextReadable` | テキスト読み取り（`read_line(span<char>)`） |
| `ByteWritable` | バイト書き込み（`write(span<const uint8_t>)`） |
| `TextWritable` | テキスト書き込み（`write("text"sv)`） |
| `Connectable` | 接続管理（`connect()`, `disconnect()`） |
| `Scannable` | スキャン（`start_scan()`, `get_found_count()`） |
| `Pressable` | ボタン状態（`get_state()`） |
| `Measurable` | センサー値（`get_value()`） |
| `Measurable3D` | 3軸センサー（`get_values()` → `Vector3`） |
| `Displayable` | 表示（`clear()`, `draw_pixel()`） |

## Contexts

Contextはヘッダーファイル（`include/omusubi/context/`）を参照。

| Context | 取得メソッド |
|---------|-------------|
| `ConnectableContext` | `get_serial_context(port)`, `get_wifi_context()`, `get_ble_context()` |
| `SensorContext` | `get_accelerometer_context()`, `get_gyroscope_context()` |
| `InputContext` | `get_button_context(index)` |
| `OutputContext` | `get_display_context()`, `get_led_context()` |
| `PowerContext` | `get_battery_level()`, `get_power_state()` |
| `SystemInfoContext` | `get_device_name()`, `get_firmware_version()` |

**使用例:**

```cpp
ConnectableContext* conn = ctx.get_connectable_context();
SerialContext* serial = conn->get_serial_context(0);
WiFiContext* wifi = conn->get_wifi_context();
```

## SystemContext

`get_system_context()` でシステム全体のContextを取得。

```cpp
#include <omusubi/omusubi.h>
using namespace omusubi;

SystemContext& ctx = get_system_context();
SerialContext* serial = nullptr;

void setup() {
    ctx.begin();
    serial = ctx.get_connectable_context()->get_serial_context(0);
}

void loop() {
    ctx.update();
    serial->write("Hello"sv);
    ctx.delay(1000);
}
```

## Tips

**Contextポインタのキャッシュ:**
```cpp
// ✅ setup()で一度だけ取得してキャッシュ
SerialContext* serial = ctx.get_connectable_context()->get_serial_context(0);
```

**ゼロコピー:**
```cpp
// ✅ std::string_view / span を使用
void process(std::string_view str) { }
```

---

**Version:** 3.0.0
**Last Updated:** 2025-11-27
