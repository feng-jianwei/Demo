#include "logger.h"
#include "log.h"
#include <thread>
#include <sstream>

using namespace std;
namespace Log {
void LightAyncLog(std::string &&log)
{
	stringstream str;
	str << "[T" << std::this_thread::get_id() << ']' << log;
	Logger::Instance()->AddLogs({str.str()});
}

void LightSyncLog(std::string &&log)
{
	stringstream str;
	str << "[T" << std::this_thread::get_id() << ']'<< std::move(log);
	Logger::Instance()->DirectWriteLog({str.str()});
}

void OpenAyncLog()
{
	Logger::Instance()->Start();
}

void CloseLog()
{
	Logger::Instance()->Stop();
}
}
