# C++17機能の完全削除 - サマリー

## 概要

C++17対応マイコンを使用しないため、プロジェクト全体からC++17以降の機能を**完全に削除**しました。

## 削除された機能

### 1. `static_string.hpp` - C++17ユーザー定義リテラル

**削除内容**:
```cpp
// ❌ 削除されたコード
namespace literals {

#if __cplusplus >= 201703L
template <typename T, T... Chars>
constexpr auto operator""_ss() noexcept {
    constexpr char str[] = {Chars..., '\0'};
    return static_string(str);
}
#endif

} // namespace literals
```

**理由**:
- C++17の`template <typename T, T... Chars>`構文はC++14では使用不可
- C++17対応マイコンを使用しないため、条件付きコンパイルも不要

**代替手段**:
`static_string()`関数を直接使用（`uint32_t`テンプレートパラメータに統一済み）：
```cpp
// ✅ C++14で使用
auto str = static_string("Hello");
constexpr auto str2 = static_string("World");  // constexpr文脈でも可
```

**注意:** 不要なラッパー関数`make_static_string()`は削除されました。
詳細は[型システムの統一](type_system_unification.md)を参照してください。

## 修正された機能

### 1. `mcu_config.h` - `inline constexpr` 関数

**修正前**:
```cpp
inline constexpr bool is_debug_build() { ... }
```

**修正後**:
```cpp
constexpr bool is_debug_build() { ... }
```

**理由**: C++14では`constexpr`関数は暗黙的に`inline`

## 検証結果

### プロジェクト全体のチェック
```bash
✓ C++17機能は完全に削除されました
✓ 条件付きコンパイル (`#if __cplusplus >= 201703L`) も存在しません
✓ すべてのコードがC++14でコンパイル可能
```

### テスト結果
```bash
cd test
make clean && make
./test_runner
# → 160個以上のテストがすべて成功
```

### コンパイラフラグ
```makefile
# Makefile
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude

# test/Makefile
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic -I../include
```

## 影響範囲

### 削除されたファイル
なし（コードの一部のみ削除）

### 修正されたファイル
1. `/workspace/include/omusubi/core/mcu_config.h` - `inline` キーワード削除
2. `/workspace/include/omusubi/core/static_string.hpp` - C++17リテラル削除
3. `/workspace/include/omusubi/core/string_base.hpp` - テンプレート修正
4. `/workspace/Makefile` - C++14フラグ設定
5. `/workspace/docs/cpp14_compatibility.md` - ドキュメント更新

### API変更
- ❌ `"Hello"_ss` リテラル（C++17）は使用不可
- ❌ `make_static_string()` ヘルパー関数も削除（不要なラッパー）
- ✅ `static_string("Hello")` を直接使用（`uint32_t`に統一済み）

## 今後の方針

1. **C++14のみを使用**: C++17以降の機能は一切使用しない
2. **条件付きコンパイル禁止**: `#if __cplusplus >= ...` は使用しない
3. **幅広い互換性**: 組み込みデバイスで広く使用可能なC++14に限定

## まとめ

- ✅ C++17機能を完全に削除
- ✅ 条件付きコンパイルも削除
- ✅ すべてのテストが成功
- ✅ C++14で完全に動作
- ✅ 組み込みデバイスの幅広い互換性を確保

**プロジェクト全体がC++14のみに依存し、C++17以降の機能は一切含まれていません。**

---

## 関連ドキュメント

- [C++14互換性ガイド](cpp14_compatibility.md) - C++14準拠の詳細
- [型システムの統一](type_system_unification.md) - `make_static_string()`削除の詳細
- [`auto`使用ガイド](auto_usage_guide.md) - C++14の型推論活用

---

**Version:** 1.0.1
**Last Updated:** 2025-11-16
