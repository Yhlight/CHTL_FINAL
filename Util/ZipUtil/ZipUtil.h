#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>

namespace CHTL {

class ZipUtil {
public:
    // Compress a directory into a zip file.
    // This will be needed for packaging CMOD/CJMOD modules.
    static bool CompressDirectory(const std::string& dirPath, const std::string& zipPath);

    // Decompress a zip file into a directory.
    // This will be needed for importing/using CMOD/CJMOD modules.
    static bool DecompressToDirectory(const std::string& zipPath, const std::string& dirPath);
};

} // namespace CHTL

#endif // ZIP_UTIL_H
