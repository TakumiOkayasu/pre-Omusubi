# Hello World サンプル

Omusubiフレームワークの最小限のサンプルプログラムです。

## 概要

シリアル通信を使用して"Hello, Omusubi!"というメッセージを出力します。

## コード

```cpp
#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

SystemContext& ctx = get_system_context();
SerialContext* serial = nullptr;

void setup() {
    ctx.begin();
    serial = ctx.get_connectable_context()->get_serial_context(0);
    serial->write_line("Hello, Omusubi!"_sv);
}

void loop() {
    ctx.update();
    ctx.delay(10);
}
```

## 学べること

- `get_system_context()`によるSystemContextの取得
- メソッドチェーンによるデバイスアクセス: `get_connectable_context()->get_serial_context()`
- `_sv`リテラルを使った文字列の扱い方
- `setup()`でデバイスを取得し、グローバル変数にキャッシュするパターン

## ビルドと実行

```bash
# ビルド
make

# 実行
make run
```

## 出力例

```
Hello, Omusubi!
```
