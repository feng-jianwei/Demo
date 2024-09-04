#ifndef __IOEVENT_H__
#define __IOEVENT_H__
#include "template/result.h"
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include "sys/epoll.h"

namespace EVENT
{
    using Handler = std::function<void(int, const epoll_event &)>;
    class EventCtl {
        public:
            static EventCtl &GetInstance();
            Result<void> AddEvent(int fd, epoll_event &evt, Handler &&handler);
            Result<void> ModEvent(int fd, epoll_event &evt);
            Result<void> DelEvent(int fd);
        private:
            EventCtl();
            ~EventCtl();
            std::shared_mutex mtx;
            std::unordered_map<int, std::shared_ptr<Handler>> fdToHandler;
            int epfd {-1};
    };
} // namespace EVENT

#endif // __IOEVENT_H__