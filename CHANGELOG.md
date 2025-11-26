# Changelog

このプロジェクトの全ての注目すべき変更は、このファイルに記録されます。

フォーマットは [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) に従い、
バージョニングは [Semantic Versioning](https://semver.org/ja/) に従います。

## [Unreleased]

## [0.2.1] - 2024-11-26

### Changed

- **テストフレームワークの移行**
  - CMake + 手動 Unity セットアップから PlatformIO 統合テストに移行
  - `pio test` コマンドで簡単にテスト実行可能
  - Unity の自動ダウンロード・管理（手動セットアップ不要）

- **テストファイルの再構成**
  - PlatformIO 標準の階層構造に対応
  - `test/test_payload/`, `test/test_sensor/`, `test/test_serial_protocol/` に整理
  - 共有ユーティリティを `test/common/` に集約

- **ドキュメント更新**
  - TESTING.md：CMake コマンドから `pio test` コマンドに更新
  - README.md：テスト実行手順を簡潔化
  - GitHub Actions 統合例を追加

### Removed

- CMakeLists.txt（PlatformIO が自動管理）
- 古いテストファイル（test/test_*.cpp）
- 古いモック ファイル（test/mock_*.cpp）

### Technical Details

- `platformio.ini` に `test_framework = unity` を追加
- Unity の自動インストール・管理（手動 git clone 不要）
- テスト実行時に Unity が自動的にダウンロード・リンク

## [0.2.0] - 2024-11-26

### Added

- **ユニットテストスイート (Unity フレームワーク)**
  - `test/test_payload.cpp` - JSON ペイロード生成の 6 テストケース
  - `test/test_sensor.cpp` - センサーデータ解析の 7 テストケース
  - `test/test_serial_protocol.cpp` - プロトコルロジックの 7 テストケース
  - `test/mock_modem.hpp/cpp` - テスト用のモック モデム実装

- **テストビルド設定**
  - `CMakeLists.txt` - CMake による統合テストビルド設定
  - `TESTING.md` - テスト実行ガイドとドキュメント

- **テストカバレッジ**
  - 有効・無効なデータの入力検証
  - エッジケース（距離 0、最大値）
  - チェックサム検証
  - リトライロジック（最大 3 回）
  - タイムスタンプ付き/なしペイロード
  - 複数フィールド ID のサポート

### Changed

- **プログラムコメント**
  - すべてのソースコードのコメントを日本語に統一
  - ドキュメンテーションコメントも日本語化

- **README.md**
  - テストセクションを拡張
  - テスト実行方法とテスト例を追加
  - テストカバレッジの詳細を記載

## [0.1.0] - 2024-11-26

### Added

- **モジュール化されたアーキテクチャ**
  - `src/payload.hpp/cpp` - JSON ペイロード生成（純粋ビジネスロジック）
  - `src/sensor.hpp/cpp` - センサーデータ解析（純粋ロジック）
  - `src/modem.hpp/cpp` - ハードウェア抽象化レイヤー（依存性注入）
  - `src/serial_protocol.hpp/cpp` - AT コマンドシーケンス管理

- **テスト可能な設計**
  - `ModemInterface` 抽象クラスによる依存性注入
  - 外部依存から分離した純粋ロジック
  - 設定可能な `Config` 構造体

- **ドキュメント**
  - `README.md` - プロジェクト概要とアーキテクチャドキュメント
  - CLAUDE.md - プロジェクト指針

### Changed

- **lib.cpp のリファクタリング**
  - 180 行から 40 行に削減
  - 低レベルの `sendATCommand()` と `sendBody()` を削除
  - `SerialProtocol` コンポジションを使用

- **ファイル構成**
  - `lib.cpp` を `lib.hpp/cpp` に分割
  - 関心の分離による保守性向上

### Fixed

- **初期ビルド エラー**
  - `lib.cpp` に不足していた include ファイルを追加
  - 外部変数の extern 宣言を追加
  - 非 const グローバル変数の外部参照に対応

### Technical Details

- **ビルド結果**
  - RAM: 13.5% (44,124 / 327,680 bytes)
  - Flash: 66.8% (875,740 / 1,310,720 bytes)

- **改善点**
  - ✅ ユニットテスト可能なコンポーネント（ハードウェア不要）
  - ✅ 依存性注入でモック化が可能
  - ✅ 設定が Config 構造体に集約
  - ✅ 関心の明確な分離
  - ✅ 拡張とメンテナンスが容易
