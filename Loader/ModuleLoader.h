#pragma once

#include "CHTLNode/ImportNode.h"
#include <string>
#include <vector>

// Represents the content of a loaded module.
struct LoadedModule {
    std::string content;
    std::string full_path;
    bool success;
};

class ModuleLoader {
public:
    // The loader needs a base path to resolve relative imports.
    explicit ModuleLoader(const std::string& base_path);

    LoadedModule load(const ImportNode* importNode);

private:
    std::string resolvePath(const std::string& import_path);
    std::string readTextFile(const std::string& path);

    std::string base_path;
    std::vector<std::string> search_paths;
};
