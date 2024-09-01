#ifndef __SERVICE_CTL_H__
#define __SERVICE_CTL_H__

#include <mutex>
#include <condition_variable>

class ServiceCtl {
public:
    static void RegistSignal();
    static bool IsExit();
    static void WaitExit();
private:
    static int fd;
    static std::mutex mtx;
    static std::condition_variable cond;
    static bool isExit;
};

#endif // __SERVICE_CTL_H__