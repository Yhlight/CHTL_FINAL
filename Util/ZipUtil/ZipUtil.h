#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {
namespace Util {

class ZipUtil {
public:
    // Packs a directory into a zip file.
    // The paths inside the zip will be relative to the base_directory.
    static bool packDirectory(const std::string& directory_path, const std::string& zip_path);

    // Unpacks a zip file into a map where the key is the filename
    // and the value is the file content.
    static std::map<std::string, std::string> unpackToMemory(const std::string& zip_path);
};

} // namespace Util
} // namespace CHTL

#endif // ZIP_UTIL_H
