# 動作環境 (Operating Environment)

本システムは以下の環境にて動作を確認しています。

## インフラ・OS
* **OS**: Ubuntu 24.04.1 LTS (WSL2 / Windows 11)
* **ターミナル**: WSL Terminal / VS Code 内蔵ターミナル

## 開発言語・コンパイラ
* **言語**: C言語 (標準C11準拠推奨)
* **コンパイラ**: GCC 13.2.0 (`build-essential` パッケージ内包)

## データベース・ミドルウェア
* **データベース**: PostgreSQL 16.13
* **連携ライブラリ**: `libpq` (`libpq-dev` パッケージ / PostgreSQL C API)