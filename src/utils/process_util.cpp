#include "process_util.h"
#include <climits>
#include "unistd.h"
#include "libgen.h"

namespace process_util {
std::string getProcessName() {
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(basename(buffer));
    } else {
        return std::string("Unknown");
    }
}

}