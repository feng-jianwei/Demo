#ifndef __TABLE_H__
#define __TABLE_H__
#include <memory>

class Table {
    public:
        Table() = default;
        virtual ~Table() = default;
        Table(const Table &) = delete;
        Table &operator=(const Table &) = delete;
        Table(Table &&) = delete;
        Table &operator=(Table &&) = delete;
        virtual std::string FetchRes() = 0;
        virtual bool HasNextRow() = 0;
};

#endif // __TABLE_H__