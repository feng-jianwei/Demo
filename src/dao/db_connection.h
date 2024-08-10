#ifndef __DB_CONNECTIOIN_H__
#define __DB_CONNECTIOIN_H__

#include <memory>
#include <string>
#include <vector>
#include "template/result.h"
#include "table.h"

class DBConnection {
    public:
        DBConnection() = default;
        virtual ~DBConnection() = default;
        virtual bool IsOk() = 0;
        virtual void ReConnect() = 0;
        virtual Result<std::unique_ptr<Table>> ExecuteAnySql(std::string sql) = 0;
};


#endif // __DB_CONNECTIOIN_H__