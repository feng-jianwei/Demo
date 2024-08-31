#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <string>
#include <vector>

namespace string_util {
    std::vector<std::string> Split(const std::string &s, char delim);
}

#endif // __STRING_UTIL_H__