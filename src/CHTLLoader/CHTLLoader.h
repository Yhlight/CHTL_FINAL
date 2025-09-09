#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include "Util/FileSystem/FileSystem.h"

class CHTLLoader {
public:
    CHTLLoader();
    std::string load(const std::string& path);
};

#endif // CHTL_LOADER_H
