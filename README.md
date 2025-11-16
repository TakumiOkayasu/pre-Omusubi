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
- **[docs/](docs/)** - その他のドキュメント

## ライセンス

未定（検討中）

## 作者

Takumi Okayasu
