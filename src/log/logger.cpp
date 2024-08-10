#include "logger.h"
#include "sys/stat.h"
#include <atomic>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "utils/process_util.h"
#include "utils/time_util.h"
#include "tpool/tpool.h"

using namespace std;
Logger *Logger::Instance() {
  static Logger logger;
  return &logger;
}

void Logger::Start() {
  TPOOL::AsyncTask([this] {
    while (!isExit) {
      DirectWriteLog(GetLogs());
    }
  });
}

void Logger::Stop() {
  std::unique_lock guard(logMutex);
  isExit = true;
  cond.notify_all();
}

void Logger::DirectWriteLog(const vector<std::string> &vecLogs) {
  std::lock_guard<std::mutex> guard(fileMutex);
  if (!fileStream.is_open() || !CheckLogFileSize()) {
    NewLogFile();
  }
  for (const auto &log : vecLogs) {
    fileStream << log << '\n';
  }
}

void Logger::AddLogs(const vector<string> &vecLogs) {
  std::unique_lock guard(logMutex);
  for (const auto &log : vecLogs) {
    logs.push_back(log);
  }
  cond.notify_one();
}

std::vector<std::string> Logger::GetLogs() {
  std::vector<string> tmp;
  std::unique_lock guard(logMutex);
  while (logs.empty()) {
    if (isExit) {
      return {};
    }
    cond.wait(guard);
  }
  logs.swap(tmp);
  return move(tmp);
}

bool Logger::CheckLogFileSize() {
  struct stat buf;
  return stat(fileName.c_str(), &buf) == 0 && buf.st_size < fileSize;
}

void Logger::SetLogPath(const std::string &logPath) { this->logPath = logPath; }

void Logger::NewLogFile() {
  std::string fileName;
  if (logPath.empty()) {
    char *pwd = getenv("PWD");
    fileName.append(pwd ? "" : string(pwd));
  } else {
    fileName.append(logPath);
  }
  string logName(process_util::getProcessName());
  logName.append(time_util::GetCurrentTimestamp());
  fileName.append(logName);
  fileStream.open(fileName, ios::app | ios::out);
}
