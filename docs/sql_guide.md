# PostgreSQL & libpq 実装・操作ガイド

## 1. DDL (データ定義言語)
本システムで使用するデータベーススキーマの定義です。

```sql
-- 資格マスタ：資格の基本情報を管理
CREATE TABLE certifications (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    target_date DATE NOT NULL,
    status INTEGER DEFAULT 0
);

-- 学習履歴：資格に紐付く学習時間を管理（CASCADE設定により親削除時に連動）
CREATE TABLE study_logs (
    id SERIAL PRIMARY KEY,
    cert_id INTEGER REFERENCES certifications(id) ON DELETE CASCADE,
    minutes INTEGER NOT NULL,
    study_date DATE DEFAULT CURRENT_DATE,
    description TEXT
);
```

## 2. libpq 実装パターン
C言語側でクエリを安全に実行するための標準的なコードパターンです。

```c
/* プレースホルダを用いたセキュアな挿入 (INSERT) */
const char *params[2] = {"基本情報技術者", "2026-10-18"};
PGresult *res = PQexecParams(conn,
    "INSERT INTO certifications (name, target_date) VALUES ($1, $2)",
    2,       /* パラメータの数 */
    NULL,    /* パラメータの型（推論に任せる場合はNULL） */
    params,  /* 値の配列 */
    NULL,    /* 値の長さ（文字列の場合はNULL） */
    NULL,    /* 書式（テキストの場合はNULL） */
    0        /* 結果の書式（0=テキスト） */
);

if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "実行エラー: %s", PQerrorMessage(conn));
}
PQclear(res);
```

## 3. psql メタコマンド
ターミナルでのデバッグやデータ確認に頻用するコマンドです。

* `\l` : データベース一覧を表示
* `\c [DB名]` : 指定したDBに接続
* `\dt` : テーブル一覧を表示
* `\d [テーブル名]` : カラム構成や型を確認
* `\q` : psqlを終了