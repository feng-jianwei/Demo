#ifndef __TIMER_MGR_H__
#define __TIMER_MGR_H__
#include <chrono>
#include <functional>
#include <shared_mutex>
#include <unordered_map>

class TimerMgr {
  using Handler = std::function<void()>;

public:
  static TimerMgr &GetInstance();
  int AddTimer(Handler &&cb, std::chrono::seconds duration,
               std::chrono::seconds interval = std::chrono::seconds(0));
  void DelTimer(int timerId);

private:
  TimerMgr() = default;
  ~TimerMgr() = default;
  std::shared_mutex mtx;
  std::unordered_map<int, Handler> timerIdToHandler;
};

#endif // __TIMER_MGR_H__