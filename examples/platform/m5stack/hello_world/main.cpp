// Hello Worldサンプル
// シリアル通信で"Hello, Omusubi!"を出力する最小限の例

#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

// グローバル変数: setup()で一度取得し、loop()で再利用
SystemContext& ctx = get_system_context();
SerialContext* serial = nullptr;

void setup() {
    // システム初期化
    ctx.begin();

    // シリアルデバイス取得（ポート0）
    serial = ctx.get_connectable_context()->get_serial_context(0);

    // メッセージ出力
    serial->write_line("Hello, Omusubi!"_sv);
}

void loop() {
    // システム更新
    ctx.update();

    // 10ms待機
    ctx.delay(10);
}
