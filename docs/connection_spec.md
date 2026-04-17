# C言語とPostgreSQLの接続仕様 (Database Connection)

## 1. 接続設定
PostgreSQL標準のCライブラリである `libpq` を使用し、UNIXドメインソケット経由でローカルデータベースへ接続します。

* **ターゲットDB**: `cert_tracker`
* **接続ユーザー**: `postgres`
* **接続文字列**: `"dbname=cert_tracker user=postgres"`

## 2. ビルド（コンパイル）コマンド
コンパイル時には、PostgreSQLのヘッダーファイルパスと共有ライブラリのリンクを明示する必要があります。

```bash
# 標準的なビルド（Ubuntu/WSL2）
gcc main.c -o app -I/usr/include/postgresql -lpq

# 推奨：pg_config を使用して環境依存を解決するビルド
gcc main.c -o app $(pg_config --cflags --libs)
```

## 3. 実装上の重要ルール (Best Practices)

### ① SQLインジェクション対策
ユーザーの入力値を直接SQL文字列に結合することは禁止します。必ず `PQexecParams` を使用し、プレースホルダ（`$1`, `$2`...）に値をバインドしてください。

### ② 厳格なメモリ管理
`libpq` が動的に確保したメモリは開発者が責任を持って解放します。クエリ実行で取得した `PGresult` は、使用後に必ず `PQclear(res)` を呼び出してください。

### ③ セッションの安全な切断
プログラム終了時（正常・異常を問わず）には、必ず `PQfinish(conn)` を呼び出して接続を閉じ、DB側のリソースを適切に返却してください。

### ④ 堅牢なエラーハンドリング
`PQstatus(conn)` での接続確認と、`PQresultStatus(res)` による実行結果のチェックを各工程で徹底します。