# C++14互換性ガイド

このドキュメントでは、Omusubiフレームワークが厳格にC++14に準拠していることを説明し、C++17以降の機能を使用しないようにするためのガイドラインを提供します。

## プロジェクトの要件

**Omusubiは厳格にC++14に準拠しています。**

- コンパイラ: Clang/Clang++ (C++14サポート)
- 標準: `-std=c++14`
- **C++17以降の機能は完全に削除されています**
- C++17対応マイコンは使用しないため、条件付きコンパイルも使用しません
- 組み込みデバイスの幅広い互換性を確保

## 修正済みのC++17機能

### 1. `inline constexpr` 変数 → `constexpr` 変数

**問題**: C++17では`inline constexpr`変数が許可されていますが、C++14では変数に`inline`を使用できません。

**修正内容**: `/workspace/include/omusubi/core/mcu_config.h`

```cpp
// ❌ C++17（修正前）
inline constexpr std::size_t MAX_STRING_LENGTH = 256;
inline constexpr bool is_debug_build() { ... }

// ✅ C++14（修正後）
constexpr std::size_t MAX_STRING_LENGTH = 256;
constexpr bool is_debug_build() { ... }
```

**理由**:
- C++14では名前空間スコープの`constexpr`変数は各翻訳単位で内部リンケージを持つため、`inline`は不要
- `constexpr`関数は暗黙的に`inline`なので、明示的な`inline`キーワードは冗長

### 2. テンプレートユーザー定義リテラル → 削除

**問題**: C++17の`template <typename T, T... Chars>`を使用したユーザー定義リテラルはC++14では使用不可。

**修正内容**: `/workspace/include/omusubi/core/static_string.hpp`

C++17のユーザー定義リテラル`operator""_ss`は完全に削除されました。

**重要:** 不要なラッパー関数`make_static_string()`も削除されています。
`static_string()`関数が`uint32_t`テンプレートパラメータに統一されました。

**使用方法**:

```cpp
// ✅ C++14（推奨）
auto str = static_string("Hello");

// ✅ constexpr文脈でも使用可能
constexpr auto str = static_string("Hello");
```

**関連:** [型システムの統一](type_system_unification.md)も参照してください。

### 3. Makefile の修正

**修正内容**: `/workspace/Makefile`

```makefile
# ❌ 修正前
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# ✅ 修正後
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude
```

## C++14で使用可能な機能

以下のC++14機能は積極的に使用してください：

### 1. 拡張された `constexpr` (重要: 可能な限り使用すること)

**CRITICAL RULE: 可能な限りすべての関数に`constexpr`を付けること**

C++14では複数の文、ループ、条件分岐、変数の変更が可能になり、実用的な`constexpr`関数が書けます。

**基本方針:**
- すべてのコンストラクタ、メソッド、関数に`constexpr`を付ける
- I/Oやハードウェアアクセスなど、副作用がある関数のみ除外
- `constexpr`を付けない場合は、理由をコメントで明記

**例（FixedString）:**
```cpp
// ✅ すべてのメソッドにconstexpr
template <uint32_t Capacity>
class FixedString {
public:
    constexpr FixedString() noexcept : byte_length_(0) { buffer_[0] = '\0'; }

    constexpr explicit FixedString(const char* str) noexcept : byte_length_(0) {
        buffer_[0] = '\0';
        if (str != nullptr) {
            append(str);
        }
    }

    constexpr uint32_t byte_length() const noexcept { return byte_length_; }

    constexpr bool append(StringView view) noexcept {
        if (byte_length_ + view.byte_length() > Capacity) {
            return false;
        }
        for (uint32_t i = 0; i < view.byte_length(); ++i) {
            buffer_[byte_length_++] = view[i];
        }
        buffer_[byte_length_] = '\0';
        return true;
    }
};

// ✅ UTF-8処理もconstexpr
namespace utf8 {
constexpr uint8_t get_char_byte_length(uint8_t first_byte) noexcept {
    if ((first_byte & 0x80) == 0x00) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 1;
}

constexpr uint32_t count_chars(const char* str, uint32_t byte_length) noexcept {
    uint32_t char_count = 0;
    uint32_t i = 0;
    while (i < byte_length) {
        const uint8_t char_len = get_char_byte_length(static_cast<uint8_t>(str[i]));
        i += char_len;
        ++char_count;
    }
    return char_count;
}
}

// ✅ ヘルパー関数もconstexpr
template <uint32_t N>
constexpr FixedString<N - 1> fixed_string(const char (&str)[N]) noexcept {
    return FixedString<N - 1>(str);
}
```

**constexprの利点:**
- コンパイル時計算により実行時オーバーヘッドがゼロ
- バイナリサイズの削減
- `static_assert`やテンプレートパラメータで使用可能
- 型安全性の向上

**コンパイル時評価の例:**
```cpp
// コンパイル時に文字列を構築
constexpr auto str = fixed_string("Hello");
static_assert(str.byte_length() == 5, "compile-time check");

// UTF-8処理もコンパイル時
constexpr auto utf8_str = fixed_string("こんにちは");
static_assert(utf8_str.char_length() == 5, "UTF-8 char count");
```

### 2. ジェネリックラムダ
```cpp
auto lambda = [](auto x, auto y) { return x + y; };
```

### 3. 戻り値型の推論
```cpp
auto get_value() {
    return 42;
}
```

### 4. バイナリリテラルと桁区切り
```cpp
int binary = 0b1010;
int large = 1'000'000;
```

### 5. 変数テンプレート
```cpp
template <typename T>
constexpr T pi = T(3.1415926535897932385);
```

## 禁止されているC++17以降の機能

以下の機能は**使用禁止**です：

### ❌ `inline` 変数
```cpp
// ❌ 使用禁止
inline constexpr int value = 42;

// ✅ 代わりにこれを使用
constexpr int value = 42;
```

### ❌ `if constexpr`
```cpp
// ❌ 使用禁止
template <typename T>
void func(T value) {
    if constexpr (std::is_integral_v<T>) {
        // ...
    }
}

// ✅ 代わりにテンプレート特殊化やSFINAEを使用
```

### ❌ 構造化束縛
```cpp
// ❌ 使用禁止
auto [x, y] = get_pair();

// ✅ 代わりにこれを使用
auto pair = get_pair();
auto x = pair.first;
auto y = pair.second;
```

### ❌ Fold expressions
```cpp
// ❌ 使用禁止
template <typename... Args>
auto sum(Args... args) {
    return (... + args);
}

// ✅ 代わりに再帰テンプレートを使用
```

### ❌ `std::optional`, `std::variant`, `std::any`
```cpp
// ❌ 使用禁止
std::optional<int> value;

// ✅ 代わりにカスタム型やポインタを使用
```

### ❌ Nested namespace定義
```cpp
// ❌ 使用禁止
namespace A::B::C {
    // ...
}

// ✅ 代わりにこれを使用
namespace A {
namespace B {
namespace C {
    // ...
}
}
}
```

### ❌ クラステンプレートの引数推論（CTAD）
```cpp
// ❌ 使用禁止
std::pair p(1, 2.0);

// ✅ 代わりに明示的に型を指定
std::pair<int, double> p(1, 2.0);
```

## コンパイル確認

### テストのビルド
```bash
cd test
make clean && make
```

すべてのテストが`-std=c++14`フラグでコンパイルされます。

### プロジェクトのビルド
```bash
make clean && make
```

メインプロジェクトも`-std=c++14`でコンパイルされます。

## まとめ

- ✅ すべてのコードはC++14に準拠
- ✅ C++17以降の機能は完全に削除（条件付きコンパイルなし）
- ✅ `static_string()`に統一（不要なラッパー関数は削除）
- ✅ Makefileは`-std=c++14`を使用
- ✅ すべてのテストがC++14でコンパイル・実行可能

C++14の厳格な準拠により、Omusubiフレームワークは幅広い組み込み環境で使用できます。

---

## 関連ドキュメント

- [型システムの統一](type_system_unification.md) - `uint32_t`への統一と`make_static_string()`削除の詳細
- [`auto`使用ガイド](auto_usage_guide.md) - C++14の型推論活用方法
- CLAUDE.md - コーディング規約全般

---

**Version:** 1.1.0
**Last Updated:** 2025-11-16
