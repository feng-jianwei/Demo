#include "tpool.h"
#include <functional>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

constexpr size_t defaultpoolsize = 4;
namespace TPOOL {

Tpool *Tpool::Instance() {
    static Tpool tpool;
    return &tpool;
}

Tpool::Tpool() { AddThread(defaultpoolsize); }

Tpool::~Tpool() { Stop(); }

void Tpool::Stop() {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        isExit = true;
        cond.notify_all();
    }
    for (auto &t : threadPool) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Tpool::AddThread(int num) {
    for (int i = 0; i < num; i++) {
        std::unique_lock<mutex> lock(threadsMutex);
        if (threadPool.size() >= threadsMaxNum) {
        return;
        }
        std::thread t([this] {
        while (!isExit) {
            std::unique_lock<mutex> lock(tasksMutex);
            while (tasks.empty()) {
                if (isExit) {
                    return;
                }
                cond.wait(lock);
            }
            auto task = std::move(tasks.front());
            tasks.pop();
            lock.unlock();
            task();
        }
        });
        pthread_setname_np(t.native_handle(),
                        string("tpool " + std::to_string(i)).c_str());
        threadPool.emplace_back(std::move(t));
    }
}

void Tpool::AddTask(std::function<void()> &&task) {
    std::unique_lock<mutex> lock(tasksMutex);
    tasks.emplace(std::move(task));
    cond.notify_one();
}

} // namespace TPOOL
