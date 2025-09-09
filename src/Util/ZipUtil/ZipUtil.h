#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>

class ZipUtil {
public:
    static void zip(const std::string& source, const std::string& destination);
    static void unzip(const std::string& source, const std::string& destination);
};

#endif // ZIP_UTIL_H
