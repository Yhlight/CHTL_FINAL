#include "CHTLLoader.h"

CHTLLoader::CHTLLoader() {}

std::string CHTLLoader::load(const std::string& path) {
    // For now, directly use FileSystem. Later, this can handle
    // more complex path resolution (e.g., relative paths, search paths).
    return FileSystem::readFile(path);
}
