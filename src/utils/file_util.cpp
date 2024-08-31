#include "file_util.h"
#include <fstream>
using namespace std;

namespace file_util {
std::string GetFileContent(const std::string &file_path) {
    std::ifstream ifs(file_path);
    if (!ifs) {
        return {};
    }
    return {std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>())};
}
} // namespace file_util