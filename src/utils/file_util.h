#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__
#include <string>

namespace file_util {
    std::string GetFileContent(const std::string &file_path);
}


#endif // __FILE_UTIL_H__