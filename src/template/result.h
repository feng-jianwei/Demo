#ifndef __RESULT_H__
#define __RESULT_H__

#include <utility>
template <typename T>
class Result {
    public:
        Result() = default;
        Result(bool isSuccess): isSuccess(isSuccess) {}
        Result(Result &&other_result): result(std::move(other_result.result)), isSuccess(other_result.isSuccess) {}
        Result& operator=(Result&& other) noexcept {
            if (this != &other) {
                result = std::move(other.result);
                isSuccess = other.isSuccess;
            }
            return *this;
        }
        ~Result() = default;
        T& GetReult() {
            return result;
        }
        bool IsSuccess() {
            return isSuccess;
        }
    private:
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