#include "timer_mgr.h"
#include "event/io_event.h"
#include <cerrno>
#include <cstring>
#include <shared_mutex>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include "tpool/tpool.h"

TimerMgr &TimerMgr::GetInstance() 
{
  static TimerMgr instance;
  return instance;
}

int TimerMgr::AddTimer(Handler &&cb, std::chrono::seconds duration,
                       std::chrono::seconds interval) 
{
    int timerId = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec timer {};
    if (duration.count() == 0) {
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = 1;
    } else {
        timer.it_value.tv_sec = duration.count();
        timer.it_value.tv_nsec = 0;
    }
    if (interval.count() != 0) {
        timer.it_interval.tv_sec = interval.count();
        timer.it_interval.tv_nsec = 0;
    }
    auto ret =timerfd_settime(timerId, 0, &timer, nullptr);
    if (ret == -1) {
        perror("timerfd_settime");
        return -1;
    }
    epoll_event evt{};
    evt.events = EPOLLIN | EPOLLET;
    evt.data.fd = timerId;
    EVENT::Handler handler = [this](int fd, [[maybe_unused]]const epoll_event &evt) {
        uint64_t exp;
        auto ret = read(fd, &exp, sizeof(exp));
        if (ret == -1) {
            perror("read");
            return;
        }
        std::shared_lock lock(mtx);
        auto iter = timerIdToHandler.find(fd);
        if (iter == timerIdToHandler.end()) {
            return;
        }
        TPOOL::AsyncTask([handler = iter->second]() { handler(); });
    };
    std::unique_lock lock(mtx);
    auto result = EVENT::EventCtl::GetInstance().AddEvent(timerId, evt, std::move(handler));
    if (!result.IsSuccess()) {
        close(timerId);
        return -1;
    }
    timerIdToHandler[timerId] = std::move(cb);
    return timerId;
}

void TimerMgr::DelTimer(int timerId) 
{
    std::unique_lock lock(mtx);
    timerIdToHandler.erase(timerId);
    EVENT::EventCtl::GetInstance().DelEvent(timerId);
    auto ret = close(timerId);
    if (ret == -1) {
        perror(std::string("close, " + std::string(strerror(errno))).c_str());
    }
}