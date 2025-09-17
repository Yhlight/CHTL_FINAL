#include "FileSystem.h"
#include <fstream>
#include <streambuf>
#include <sys/stat.h>

namespace CHTL {

bool FileSystem::ReadFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    // Lock the file to prevent other processes from writing to it while it is being read
    // This is a simple implementation, a more robust solution might be needed for a real-world application
    // but for now, this should suffice.
    // Note: fstream does not have a built-in locking mechanism, so we have to rely on OS-specific features.
    // For this sandbox environment, we will assume that we don't need to worry about concurrent access.
    content.assign((std::istreambuf_iterator<char>(file)),
                   (std::istreambuf_iterator<char>()));
    file.close();
    return true;
}

bool FileSystem::WriteFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    // Similar to ReadFile, we should lock the file before writing to it.
    file << content;
    file.close();
    return true;
}

bool FileSystem::FileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

} // namespace CHTL
