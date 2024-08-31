#include "dao/postgres_conn.h"
#include "utils/file_util.h"
#include "utils/string_util.h"

bool Init_DB() {
    // Connect to the database
    std::string connInfo = "host=localhost port=5432 dbname=postgres user=postgres password=Changeme_123";
    PostGresConn conn(connInfo);
    if (!conn.IsOk()) {
        return false;
    }
    auto sqls = string_util::Split(file_util::GetFileContent("config/init_database.sql"), ';');
    // Create the table
    sqls.pop_back();
    conn.ExecuteEvent(std::move(sqls));
    return true;
}

bool Init_Service() {
    return Init_DB();
}