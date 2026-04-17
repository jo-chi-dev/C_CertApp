#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// --- 資格一覧を表示する関数 (Read) ---
void list_certifications(PGconn *conn) {
    printf("\n【登録済みの資格一覧】\n");
    printf("--------------------------------------------------\n");
    PGresult *res = PQexec(conn, "SELECT id, name, target_date, status FROM certifications ORDER BY target_date ASC");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "データの取得に失敗しました: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        printf("登録されている資格はありません。\n");
    } else {
        for (int i = 0; i < rows; i++) {
            char *id = PQgetvalue(res, i, 0);
            char *name = PQgetvalue(res, i, 1);
            char *date = PQgetvalue(res, i, 2);
            int status_code = atoi(PQgetvalue(res, i, 3));

            char status_str[20];
            if (status_code == 0) strcpy(status_str, "予定");
            else if (status_code == 1) strcpy(status_str, "学習中");
            else if (status_code == 2) strcpy(status_str, "✨合格✨");
            else if (status_code == 3) strcpy(status_str, "不合格");
            else strcpy(status_str, "不明");

            printf("[%s] %-20s (目標日: %s) [%s]\n", id, name, date, status_str);
        }
    }
    printf("--------------------------------------------------\n\n");
    PQclear(res);
}

// --- 新しい資格を登録する関数 (Create) ---
void add_certification(PGconn *conn) {
    char name[100];
    char date[15];

    printf("\n【新しい資格の登録】\n");
    printf("資格名を入力してください: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("目標日を入力してください (例: 2026-10-18): ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = '\0';

    const char *command = "INSERT INTO certifications (name, target_date, status) VALUES ($1, $2, 1)";
    const char *paramValues[2] = {name, date};

    PGresult *res = PQexecParams(conn, command, 2, NULL, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "❌ 登録に失敗しました: %s\n", PQerrorMessage(conn));
    } else {
        printf("✅ 「%s」を登録しました！\n\n", name);
    }
    PQclear(res);
}

// --- 学習記録を追加する関数 ---
void add_study_log(PGconn *conn) {
    char cert_id_str[10];
    char minutes_str[10];
    char description[200];

    printf("\n【学習記録の追加】\n");
    list_certifications(conn);

    printf("勉強した資格のIDを入力してください: ");
    fgets(cert_id_str, sizeof(cert_id_str), stdin);
    cert_id_str[strcspn(cert_id_str, "\n")] = '\0';

    printf("勉強時間（分）を入力してください: ");
    fgets(minutes_str, sizeof(minutes_str), stdin);
    minutes_str[strcspn(minutes_str, "\n")] = '\0';

    printf("学習内容（メモ）を入力してください: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    const char *command = "INSERT INTO study_logs (cert_id, minutes, description) VALUES ($1, $2, $3)";
    const char *paramValues[3] = {cert_id_str, minutes_str, description};

    PGresult *res = PQexecParams(conn, command, 3, NULL, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "❌ 記録に失敗しました: %s\n", PQerrorMessage(conn));
    } else {
        printf("✅ 学習記録を保存しました！\n\n");
    }
    PQclear(res);
}

// --- ステータスを「合格」に更新する関数 (Update) ---
void update_status(PGconn *conn) {
    char cert_id_str[10];
    printf("\n【💮 合格報告 💮】\n");
    list_certifications(conn);
    printf("見事合格した資格のIDを入力してください: ");
    fgets(cert_id_str, sizeof(cert_id_str), stdin);
    cert_id_str[strcspn(cert_id_str, "\n")] = '\0';

    const char *command = "UPDATE certifications SET status = 2 WHERE id = $1";
    const char *paramValues[1] = {cert_id_str};

    PGresult *res = PQexecParams(conn, command, 1, NULL, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "❌ 更新に失敗しました: %s\n", PQerrorMessage(conn));
    } else {
        if (atoi(PQcmdTuples(res)) > 0) printf("🎉 合格ステータスに更新しました！\n\n");
        else printf("⚠️ IDが見つかりません。\n\n");
    }
    PQclear(res);
}

// --- ★新規追加: 資格を削除する関数 (Delete) ---
void delete_certification(PGconn *conn) {
    char cert_id_str[10];
    char confirm[10];

    printf("\n【資格の削除】\n");
    list_certifications(conn);

    printf("削除したい資格のIDを入力してください: ");
    fgets(cert_id_str, sizeof(cert_id_str), stdin);
    cert_id_str[strcspn(cert_id_str, "\n")] = '\0';

    printf("⚠️ この資格に関連するすべての学習記録も削除されます。本当によろしいですか？(y/n): ");
    fgets(confirm, sizeof(confirm), stdin);

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("キャンセルしました。\n\n");
        return;
    }

    const char *command = "DELETE FROM certifications WHERE id = $1";
    const char *paramValues[1] = {cert_id_str};

    PGresult *res = PQexecParams(conn, command, 1, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "❌ 削除に失敗しました: %s\n", PQerrorMessage(conn));
    } else {
        if (atoi(PQcmdTuples(res)) > 0) printf("🗑️ 資格ID: %s を削除しました。\n\n", cert_id_str);
        else printf("⚠️ 該当するIDが見つかりませんでした。\n\n");
    }
    PQclear(res);
}

// --- 進捗と統計を確認する関数 ---
void show_progress(PGconn *conn) {
    printf("\n【現在の学習進捗一覧】\n");
    printf("--------------------------------------------------\n");
    const char *query = 
        "SELECT c.name, (c.target_date - CURRENT_DATE) AS days_left, "
        "COALESCE(SUM(s.minutes), 0) AS total_minutes, c.status "
        "FROM certifications c LEFT JOIN study_logs s ON c.id = s.cert_id "
        "GROUP BY c.id, c.name, c.target_date, c.status ORDER BY c.target_date ASC";

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            char *name = PQgetvalue(res, i, 0);
            int days_left = atoi(PQgetvalue(res, i, 1));
            int total_minutes = atoi(PQgetvalue(res, i, 2));
            int status_code = atoi(PQgetvalue(res, i, 3));
            int hours = total_minutes / 60;
            int mins = total_minutes % 60;

            if (status_code == 2) printf("💮 %s 【合格済】\n   総学習時間: %d時間 %d分\n", name, hours, mins);
            else printf("■ %s\n   総学習時間: %d時間 %d分 / 試験日まで: あと %d 日\n", name, hours, mins, days_left);
        }
    }
    printf("--------------------------------------------------\n\n");
    PQclear(res);
}

// --- メイン関数 ---
int main() {
    const char *conninfo = "dbname=cert_tracker user=postgres";
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "DB接続エラー: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    int choice = 0;
    while (1) {
        printf("=================================\n");
        printf(" 🎯 資格ロードマップ管理 (Full CRUD)\n");
        printf("=================================\n");
        printf("1: 資格一覧を見る\n");
        printf("2: 新しい資格を登録する\n");
        printf("3: 今日の勉強を記録する\n");
        printf("4: 学習進捗を確認する\n");
        printf("5: 資格を「合格」ステータスにする\n");
        printf("6: 資格を削除する\n"); // ★メニュー追加
        printf("9: 終了する\n");
        printf("---------------------------------\n");
        printf("選択してください: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            printf("⚠️ 正しい番号を入力してください。\n\n");
            continue;
        }
        while(getchar() != '\n'); 

        if (choice == 1) list_certifications(conn);
        else if (choice == 2) add_certification(conn);
        else if (choice == 3) add_study_log(conn);
        else if (choice == 4) show_progress(conn);
        else if (choice == 5) update_status(conn);
        else if (choice == 6) delete_certification(conn); // ★処理追加
        else if (choice == 9) break;
    }
    PQfinish(conn);
    return 0;
}