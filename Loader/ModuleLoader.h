#pragma once

#include "CHTLNode/ImportNode.h"
#include <string>
#include <vector>

#include "ModuleInfo.h"
#include <optional>

// Represents the content of a loaded module.
struct LoadedModule {
    std::string content; // For simple files, this is the text. For CMOD, this is the path to the temp dir.
    std::string full_path;
    bool success;
    std::optional<ModuleInfo> info; // For CMOD modules
};

class ModuleLoader {
public:
    // The loader needs a base path to resolve relative imports.
    explicit ModuleLoader(const std::string& base_path);

    LoadedModule load(const ImportNode* importNode);
    std::string readTextFile(const std::string& path);

private:
    std::string resolvePath(const std::string& import_path);

    std::string base_path;
    std::vector<std::string> search_paths;
};
