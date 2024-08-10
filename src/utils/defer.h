#ifndef __DEFER_H__
#define __DEFER_H__

#include <functional>

class DeferObj {
    public:
        DeferObj(std::function<void()> f) : f(move(f)) {}
        ~DeferObj() { f(); }
    private:
        std::function<void()> f;
};


#endif // __DEFER_H__