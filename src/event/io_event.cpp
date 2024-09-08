#include "io_event.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <sys/epoll.h>
#include <unistd.h>
#include <mutex>
#include <fcntl.h>
#include "tpool/tpool.h"

namespace EVENT
{
    constexpr int MAX_EVENTS = 64;
    constexpr int TIMEOUT = 5 * 1000;
    EventCtl &EventCtl::GetInstance()
    {
        static EventCtl instance;
        return instance;
    }

    EventCtl::EventCtl()
    {
        epfd = epoll_create1(EPOLL_CLOEXEC);
        if (epfd == -1) {
            std::string err = strerror(errno);
            std::cerr << "Failed to create epoll file descriptor" << err << '\n';
            throw std::runtime_error("Failed to create epoll file descriptor");
        }
        auto func = std::make_shared<std::function<void()>>();
        *func = [func, this]() {
            struct epoll_event events[MAX_EVENTS];
            int nfds = epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT);
            if (nfds == -1) {
                perror("epoll_wait");
                TPOOL::AsyncTask(*func);
                return;
            }
            printf("nfds: %d\n", nfds);
            for (int i = 0; i < nfds; ++i) {
                printf("events[%d].data.fd: %d\n", i, events[i].data.fd);
                printf("events[%d].events: %d\n", i, events[i].events);
                    // 读取数据
                std::shared_ptr<Handler> handler;
                {
                    std::shared_lock<std::shared_mutex> lock(mtx);
                    auto iter = fdToHandler.find(events[i].data.fd);
                    if (iter == fdToHandler.end()) {
                        char buf[1024];
                        while(read(events[i].data.fd, buf, sizeof(buf)) > 0) {};
                        continue;
                    }
                    handler = iter->second;
                }
                (*handler)(events[i].data.fd, events[i]);
            }
            TPOOL::AsyncTask(*func);
        };
        TPOOL::AsyncTask(*func);
    }

    EventCtl::~EventCtl()
    {
        if (epfd != -1) {
            close(epfd);
        }
    }

    Result<void> EventCtl::AddEvent(int fd, epoll_event &evt, Handler && handler)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            return false;
        }
        flags |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, flags) == -1) {
            return false;
        }
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) == -1) {
            return false;
        }
        std::unique_lock<std::shared_mutex> lock(mtx);
        fdToHandler[fd] = std::make_shared<Handler>(std::move(handler));
        printf("AddEvent fd: %d\n", fd);
        return {};
    }

    Result<void> EventCtl::ModEvent(int fd, epoll_event &evt)
    {
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt) == -1) {
            return false;
        }
        return {};
    }

    Result<void> EventCtl::DelEvent(int fd)
    {
        if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            return false;
        }
        std::unique_lock<std::shared_mutex> lock(mtx);
        fdToHandler.erase(fd);
        return {};
    }
} // namespace EVENT