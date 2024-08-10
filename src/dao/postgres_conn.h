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
        ~PostGresConn() = default;
        bool IsOk() override;
        void ReConnect() override;
        Result<std::unique_ptr<Table>> ExecuteAnySql(std::string sql) override;
    private:
        class Impl;
        std::unique_ptr<Impl> conn {nullptr};
        std::string connInfo;
};


#endif // __POSTGRES_CONN_H__