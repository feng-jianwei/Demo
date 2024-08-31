#ifndef __POSTGRES_CONN_H__
#define __POSTGRES_CONN_H__

#include <iterator>
#include <memory>
#include <string>
#include "db_connection.h"
#include "template/result.h"


class PostGresConn : public DBConnection {
    public:
        PostGresConn(std::string connInfo);
        PostGresConn(PostGresConn &&connection);
        ~PostGresConn();
        bool IsOk() override;
        void ReConnect() override;
        Result<std::unique_ptr<Table>> ExecuteAnySql(std::string sql) override;
        Result<std::unique_ptr<Table>> ExecuteEvent(std::vector<std::string> sqls) override;
    private:
        class Impl;
        Impl* m_conn {nullptr};
        std::string m_connInfo;
};


#endif // __POSTGRES_CONN_H__