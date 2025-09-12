#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>
#include <vector>

namespace CHTL {

class ZipUtil {
public:
    static bool compress(const std::string& sourcePath, const std::string& zipPath);
    static bool extract(const std::string& zipPath, const std::string& destPath);
    static std::vector<std::string> listFiles(const std::string& zipPath);
    static bool isValidZip(const std::string& zipPath);
};

} // namespace CHTL

#endif // ZIP_UTIL_H