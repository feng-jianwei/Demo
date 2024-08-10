
#ifndef __LOG_H__
#define __LOG_H__

#include <cstdio>
#include <string>
#include <utility>

#define LIGHT_EXPORT __attribute__((visibility("default")))

namespace Log {
LIGHT_EXPORT void OpenAyncLog();

LIGHT_EXPORT void CloseLog();

LIGHT_EXPORT void LightAyncLog(std::string &&log);

LIGHT_EXPORT void LightSyncLog(std::string &&log);


template <typename... types>
LIGHT_EXPORT void AyncLog(const std::string &formatLog, types &&...args) {
  char LogStr[300] = {0};
  size_t logSize =
      snprintf(LogStr, 300, formatLog.c_str(), std::forward<types>(args)...);
  LightAyncLog(std::string(LogStr));
}

template <typename... types>
LIGHT_EXPORT void SyncLog(const std::string &formatLog, types &&...args) {
  char LogStr[300] = {0};
  size_t logSize =
      snprintf(LogStr, 300, formatLog.c_str(), std::forward<types>(args)...);
  LightSyncLog(std::string(LogStr));
}
} // namespace LightLog
#endif
