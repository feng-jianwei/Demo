#include "time_util.h"

namespace time_util {
std::string GetCurrentTimestamp() {
    time_t t;
    time(&t);
    struct tm *timeLocal = localtime(&t);
    char timestamp[100] = {0};
    strftime(timestamp, 100, "%Y%m%d%H%M%S", timeLocal);
    return timestamp;
}

}