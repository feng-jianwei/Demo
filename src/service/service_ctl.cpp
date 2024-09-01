#include "sys/eventfd.h"
#include "sys/epoll.h"
#include "event/io_event.h"
#include "service_ctl.h"
#include <cstdint>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <csignal>

using namespace std;

void ServiceCtl::RegistSignal()
{
    struct sigaction oldAction{};
    if (sigaction(SIGINT, nullptr, &oldAction) == -1) {
        perror("sigaction");
        return;
    }
    if (oldAction.sa_handler != SIG_DFL) {
        printf("SIGINT has been registered by other process\n");
        return;
    }
    epoll_event evt{};
    evt.events = EPOLLIN;
    evt.data.fd = fd;
    EVENT::handler handler = [](int fd, [[maybe_unused]]const epoll_event &evt) {
        if (fd != ServiceCtl::fd) {
            return;
        }
        eventfd_t buf;
        auto ret = eventfd_read(fd, &buf);
        if (ret == -1) {
            if (errno == EAGAIN) {
                return;
            }
            perror("read");
            return;
        }
        std::unique_lock lock(ServiceCtl::mtx);
        if (buf == 1) {
            ServiceCtl::isExit = true;
            ServiceCtl::cond.notify_all();
        }
    };
    EVENT::EventCtl::GetInstance().AddEvent(fd, evt, std::move(handler));
    auto sighandler = [](int sig) {
        if (sig == SIGINT || sig == SIGTERM) {
            eventfd_write(ServiceCtl::fd, 1);
        }
    };
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    printf("RegistSignal\n");
}

bool ServiceCtl::IsExit()
{
    std::unique_lock lock(mtx);
    return false;
}

void ServiceCtl::WaitExit()
{
    std::unique_lock lock(mtx);
    while (!isExit) {
        cond.wait_for(lock, chrono::seconds(5));
    }
}

int ServiceCtl::fd = eventfd(0, EFD_NONBLOCK);
mutex ServiceCtl::mtx;
bool ServiceCtl::isExit = false;
condition_variable ServiceCtl::cond;