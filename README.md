# Omusubi Framework

**⚠️ アルファ版 - 現在開発中**

このプロジェクトはアルファ版です。APIは予告なく変更される可能性があります。
本番環境での使用は推奨されません。

---

マイコン向けの軽量で型安全なC++14フレームワーク

## 概要

Omusubiは、組み込みデバイス向けに設計されたC++14フレームワークです。
SystemContextを中心としたクリーンなアーキテクチャにより、ハードウェアへの統一的なアクセスを提供します。

## 特徴

- ゼロオーバーヘッド抽象化（動的メモリ確保なし、例外なし）
- C++14標準準拠
- インターフェース/実装の完全分離
- SystemContextによる統一的なハードウェアアクセス

## サポートデバイス

- M5Stack (Basic, Core2, etc.)

## 開発環境

Dev Container環境で開発できます。

```bash
# VS Codeで開く
code .

# "Reopen in Container"を選択
```

詳細は[CLAUDE.md](CLAUDE.md)を参照してください。

## クイックスタート

サンプルコードは[examples/](examples/)ディレクトリを参照してください。

基本的な使い方:
- **Hello World**: `examples/platform/m5stack/hello_world/`
- **その他のサンプル**: `examples/platform/m5stack/`

## ドキュメント

詳細は以下を参照してください:

- **[CLAUDE.md](CLAUDE.md)** - 開発ガイドとアーキテクチャ詳細
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - 開発環境とワークフロー
- **[docs/](docs/)** - 技術ドキュメント
  - [licenses.md](docs/licenses.md) - ライセンスと依存関係
  - [error-handling.md](docs/error-handling.md) - エラーハンドリング
  - [testing.md](docs/testing.md) - テストガイド
  - [performance.md](docs/performance.md) - パフォーマンス最適化
  - [debug.md](docs/debug.md) - デバッグガイド

## ライセンス

**Omusubiコア部分は外部ライブラリに依存していません。**

- コア機能（Optional, Result, Logger, StringViewなど）は標準C++のみ使用
- ライセンス制約なし、商用利用完全自由
- プラットフォーム実装（M5Stackなど）のみが外部ライブラリに依存

詳細は [docs/licenses.md](docs/licenses.md) を参照してください。

**Omusubiフレームワーク自体のライセンス:** 未定（MIT または Apache 2.0 を検討中）

## 作者

Takumi Okayasu
