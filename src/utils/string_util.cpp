#include "string_util.h"
#include <vector>
#include <sstream>

namespace string_util {
    std::vector<std::string> Split(const std::string &s, char delim) {
        std::vector<std::string> result;
        std::string token;
        std::stringstream tokenStream(s);
        while (std::getline(tokenStream, token, delim)) {
            result.emplace_back(token);
        }
        return result;
    }
}