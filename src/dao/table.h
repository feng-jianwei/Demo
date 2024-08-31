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

        template<typename T>
        Table& operator>> (T& t) {
            const std::string res = FetchRes();

            if constexpr(std::is_same_v<T, std::string>) {
                t = res;
            } else if constexpr(std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                if constexpr(std::is_same_v<T, int>) t = std::stoi(res);
                else if constexpr(std::is_same_v<T, long>) t = std::stol(res);
                else if constexpr(std::is_same_v<T, long long>) t = std::stoll(res);
                else if constexpr(std::is_same_v<T, unsigned long>) t = std::stoul(res);
                else if constexpr(std::is_same_v<T, unsigned long long>) t = std::stoull(res);
            } else if constexpr(std::is_floating_point_v<T>) {
                if constexpr(std::is_same_v<T, float>) t = std::stof(res);
                else if constexpr(std::is_same_v<T, double>) t = std::stod(res);
            } else if constexpr(std::is_same_v<T, bool>) {
                t = res == "1" || res == "true" || res == "t";
            } else {
                static_assert(std::is_same_v<T, void>, "Type not supported");
            }
            return *this;
        }
};

#endif // __TABLE_H__