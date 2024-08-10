#ifndef __RESULT_H__
#define __RESULT_H__

#include <utility>
template <typename T>
class Result {
    public:
        Result() = default;
        Result(bool isSuccess): isSuccess(isSuccess) {}
        Result(T &&result): result(std::forward<T>(result)) {}
        ~Result() = default;
        T GetReult() {
            return result;
        }
        bool IsSuccess() {
            return isSuccess;
        }
        T result {};
        bool isSuccess {true};
};

template <>
class Result<void> {
    public:
        Result() = default;
        Result(bool isSuccess): isSuccess(isSuccess) {}
        ~Result() = default;
        bool IsSuccess() {
            return isSuccess;
        }
        bool isSuccess {true};
};

#endif // __RESULT_H__