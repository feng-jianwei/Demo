#include "postgres_conn.h"
#include "utils/defer.h"
#include <functional>
#include <memory>
#include <string>
#include <libpq-fe.h>
#include <iostream>
#include <type_traits>

using namespace std;

class PostGreSQLResult : public Table {
    public:
        PostGreSQLResult(PGresult *res) : res(res) {}
        ~PostGreSQLResult() {
            if (res != nullptr) {
                PQclear(res);
            }
        }
        std::string FetchRes() override {
            return PQresultStatus(res) == PGRES_TUPLES_OK && m_col++ < PQnfields(res) 
                ? PQgetvalue(res, m_row, m_col) : "";
        }
        bool HasNextRow() override {
            return m_row++ < PQntuples(res);
        }
    private:
        PGresult *res;
        int m_row {};
        int m_col {};
};


class PostGresConn::Impl {
    public:
        Impl(PGconn *conn) : conn(conn) {}
        PGconn *GetConn() {
            return conn;
        }
        ~Impl() {
            if (conn != nullptr) {
                PQfinish(conn);
            }
        }
    private:
        PGconn* conn;
};

PostGresConn::PostGresConn(std::string connInfo): 
    m_connInfo(std::move(connInfo)) {
        m_conn = new Impl(PQconnectdb(m_connInfo.c_str()));
    }

PostGresConn::PostGresConn(PostGresConn &&connection) {
    m_conn = connection.m_conn;
    connection.m_conn = nullptr;
}

bool PostGresConn::IsOk() {
    if (m_conn == nullptr) {
        return false;
    }
    if (PQstatus(m_conn->GetConn()) != CONNECTION_OK) {
        std::cout << "Connection to database failed: " << PQerrorMessage(m_conn->GetConn()) << std::endl;
        return false;
    }
    return true;
}

void PostGresConn::ReConnect() {
    if (m_conn != nullptr) {
        delete m_conn;
    }
    m_conn = new Impl(PQconnectdb(m_connInfo.c_str()));
}

Result<std::unique_ptr<Table>> PostGresConn::ExecuteAnySql(std::string sql) {
    if (IsOk() != true) {
        std::cerr << "Connection is not established" << std::endl;
        return false;
    }
    cout << "Executing SQL: " << sql << endl;
    PGresult *res = PQexec(m_conn->GetConn(), sql.c_str());
    auto result = PQresultStatus(res);
    if (result != PGRES_TUPLES_OK && result != PGRES_COMMAND_OK) {
        std::cout << "execution failed: " << PQerrorMessage(m_conn->GetConn()) << std::endl;
        return false;
    }
    return new PostGreSQLResult(res);
}

PostGresConn::~PostGresConn() {
    if (m_conn != nullptr) {
        delete m_conn;
    }
    m_conn = nullptr;
}

Result<void> PostGresConn::ExecuteEvent(std::vector<std::string> sqls) {
    auto res = ExecuteAnySql("BEGIN;");
    if (!res.IsSuccess()) {
        return false;
    }
    for (auto &&sql : sqls) {
        res = ExecuteAnySql(std::move(sql));
        if (!res.IsSuccess()) {
            res = ExecuteAnySql("ROLLBACK;");
            return false;
        }
    }
    res = ExecuteAnySql("COMMIT;");
    if (!res.IsSuccess()) {
        return false;
    }
    return true;
}
