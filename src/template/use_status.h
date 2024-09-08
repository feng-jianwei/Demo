#ifndef __TEMPLATE_USE_STATUS__
#define __TEMPLATE_USE_STATUS__


template <typename T>
class UseStatus {
public:
    UseStatus(T &ptr) : ptr(ptr) {
        if (ptr) {
            ptr->Use();
        }
    }

    ~UseStatus() {
        if (ptr) {
            ptr->Unuse();
        }
    }

    T operator->() {
        return ptr;
    }

    T *Get() {
        return ptr;
    }
    T &ptr;
};

#endif /* __TEMPLATE_USE_STATUS__ */
