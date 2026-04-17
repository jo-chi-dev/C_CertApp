# PostgreSQL & C言語連携

## 1. psql メタコマンド（ターミナル操作）
PostgreSQLのコンソール（`postgres=#` など）で実行する管理用コマンドです。

```bash
\l           # データベースの一覧を表示 (List)
\c [db_name] # 指定したデータベースに接続 (Connect)
\dt          # 現在のDB内のテーブル一覧を表示 (Display Tables)
\d [table]   # テーブルの構造（カラムや型）を確認 (Describe)
\q           # psqlを終了してターミナルに戻る (Quit)
```

## 2. データベース・テーブルの定義 (DDL)

### データベースの作成と削除
```sql
CREATE DATABASE my_database;
DROP DATABASE my_database;
```

### テーブルの作成
```sql
CREATE TABLE certifications (
    id SERIAL PRIMARY KEY,              -- 自動連番の主キー
    name VARCHAR(100) NOT NULL,         -- 最大100文字、空を許可しない
    target_date DATE NOT NULL,          -- 日付型
    status INTEGER DEFAULT 0            -- デフォルト値を0に設定
);
```

### テーブルの変更・削除
```sql
-- カラムの追加
ALTER TABLE certifications ADD COLUMN category VARCHAR(50);

-- テーブルの削除（依存するデータごと強制削除する場合は CASCADE を付与）
DROP TABLE certifications CASCADE;
```

## 3. データの操作 (CRUD)

### Create (登録: INSERT)
```sql
-- 単一レコードの挿入
INSERT INTO certifications (name, target_date, status) 
VALUES ('システムアーキテクト', '2026-04-19', 1);

-- 複数レコードの一括挿入
INSERT INTO certifications (name, target_date, status) VALUES 
('AWS SAA', '2026-06-10', 1),
('日商簿記2級', '2026-06-14', 1);
```

### Read (取得: SELECT)
```sql
-- 全件取得
SELECT * FROM certifications;

-- カラムを指定して取得
SELECT name, target_date FROM certifications;

-- 条件指定 (WHERE)
SELECT * FROM certifications WHERE status = 1;

-- 並び替え (ORDER BY: ASC=昇順, DESC=降順)
SELECT * FROM certifications ORDER BY target_date ASC;
```

### Update (更新: UPDATE)
```sql
-- 条件に一致するレコードを更新（WHEREを忘れると全件更新されるので注意）
UPDATE certifications 
SET status = 2 
WHERE id = 3;
```

### Delete (削除: DELETE)
```sql
-- 条件に一致するレコードを削除
DELETE FROM certifications WHERE id = 3;

-- テーブル内の全データを消去（テーブルの枠組みは残る）
TRUNCATE TABLE certifications;
```

## 4. 複数テーブルの連携と集計 (高度な操作)

### テーブルの結合 (JOIN)
```sql
-- LEFT JOIN (左側のテーブルを基準に結合)
SELECT 
    c.name, 
    s.minutes 
FROM certifications c
LEFT JOIN study_logs s ON c.id = s.cert_id;
```

### 集計関数とグループ化 (GROUP BY)
```sql
-- 特定のカラムの合計値を算出する (SUM) / グループ化
SELECT 
    c.name, 
    SUM(s.minutes) AS total_minutes 
FROM certifications c
JOIN study_logs s ON c.id = s.cert_id
GROUP BY c.name;
```

### 便利な組み込み関数
```sql
CURRENT_DATE                                    -- 現在の日付を取得
SELECT target_date - CURRENT_DATE;              -- 日付の計算（現在から目標日までの差分日数）
COALESCE(SUM(minutes), 0)                       -- NULLを別の値に変換する（NULLなら0にする）
```

---

## 5. 【おまけ】C言語 (libpq) 必須関数まとめ

### 接続と切断
```c
// 接続
PGconn *conn = PQconnectdb("dbname=my_db user=postgres");

// 接続状態の確認
if (PQstatus(conn) != CONNECTION_OK) { /* エラー処理 */ }

// 切断（メモリ解放）※超重要
PQfinish(conn);
```

### クエリの実行と結果の取得
```c
// SELECT文の実行 (Read)
PGresult *res = PQexec(conn, "SELECT id, name FROM certifications");
int rows = PQntuples(res);           // 行数の取得
char *name = PQgetvalue(res, 0, 1);  // 0行目、1列目のデータを文字列として取得

// プレースホルダを使った安全な実行 (Create, Update, Delete)
const char *command = "INSERT INTO certifications (name) VALUES ($1)";
const char *params[1] = {"基本情報技術者"};
PGresult *res = PQexecParams(conn, command, 1, NULL, params, NULL, NULL, 0);

// 結果メモリの解放 ※超重要
PQclear(res);
```