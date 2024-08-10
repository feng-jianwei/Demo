#include "postgres_conn.h"
#include "utils/defer.h"
#include <functional>
#include <memory>
#include <string>
#include <libpq-fe.h>
#include <iostream>

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
    conn(std::make_unique<Impl>(PQconnectdb(connInfo.c_str()))), connInfo(connInfo) {}

PostGresConn::PostGresConn(PostGresConn &&connection) {
    conn = std::move(connection.conn);
    connection.conn = nullptr;
}

bool PostGresConn::IsOk() {
    if (conn == nullptr) {
        return false;
    }
    return PQstatus(conn->GetConn()) == CONNECTION_OK;
}

void PostGresConn::ReConnect() {
    if (conn != nullptr) {
        conn.reset();
    }
    conn = std::make_unique<Impl>(PQconnectdb(connInfo.c_str()));
}

Result<std::unique_ptr<Table>> PostGresConn::ExecuteAnySql(std::string sql) {
    if (IsOk() != true) {
        std::cerr << "Connection is not established" << std::endl;
        return false;
    }

    PGresult *res = PQexec(conn->GetConn(), sql.c_str());
    auto result = PQresultStatus(res);
    if (result != PGRES_TUPLES_OK && result != PGRES_COMMAND_OK) {
        std::cerr << "execution failed: " << PQerrorMessage(conn->GetConn()) << std::endl;
        return false;
    }
    return new PostGreSQLResult(res);
}
