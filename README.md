# 資格ロードマップ管理システム (Certification Tracker CLI)

C言語とPostgreSQLを高度に連携させ、学習進捗を動的に管理・可視化するためのCLIアプリケーションです。

## 🎯 プロジェクトの概要
本システムは、データベースエンジンの計算能力を最大限に活用し、試験日までのカウントダウンや総学習時間の動的な集計をリアルタイムで行うことを目的としています。

* **マスタ管理**: 資格名、目標日、合格ステータスのCRUD操作。
* **学習ログ記録**: 日々の学習時間と内容の蓄積。
* **統計分析**: `JOIN` による資格ごとの総学習時間と目標日までの残り日数の算出。

---

## 2. ディレクトリ構成
```text
C_CertApp/
├── main.c                 # アプリケーションソースコード
├── app                    # 実行バイナリ（gitignore対象）
├── docs/                  # 詳細ドキュメント
│   ├── environment.md     # 動作環境
│   ├── architecture.md    # アーキテクチャ・設計
│   ├── sql_guide.md       # SQL/libpq実装ガイド
│   └── connection_spec.md # 接続仕様・ビルド手順
└── README.md              # プロジェクト全体像（本ファイル）
```

---

## 3. 技術スタック
* **Language**: C言語 (GCC)
* **Database**: PostgreSQL 17
* **Interface**: libpq (PostgreSQL C API)

---

## 4. クイックスタート
```bash
# コンパイル（pg_configでパスを自動解決）
gcc main.c -o app $(pg_config --cflags --libs)

# 実行（PostgreSQL権限で実行）
sudo -u postgres ./app
```

---

## 5. 各ドキュメントへのリンク
* [動作環境・セットアップ](./docs/environment.md)
* [アーキテクチャ・設計思想](./docs/architecture.md)
* [DB接続仕様・ビルドガイド](./docs/connection_spec.md)
* [SQL/libpq 実装ガイド](./docs/sql_guide.md)