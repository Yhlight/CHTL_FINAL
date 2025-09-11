#include "ModuleLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

bool fileExists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

ModuleLoader::ModuleLoader(const std::string& base_path) : base_path(base_path) {
    std::filesystem::path p(base_path);
    search_paths.push_back(p.is_absolute() ? p.parent_path().string() : std::filesystem::current_path().string());
    search_paths.push_back((search_paths[0] / std::filesystem::path("module")).string());
}

LoadedModule ModuleLoader::load(const ImportNode* importNode) {
    std::string resolved_path = resolvePath(importNode->path);
    if (resolved_path.empty()) {
        std::cerr << "Warning: Could not resolve import path '" << importNode->path << "'." << std::endl;
        return {"", "", false};
    }
    std::string content = readTextFile(resolved_path);
    return {content, resolved_path, !content.empty()};
}

std::string ModuleLoader::resolvePath(const std::string& import_path) {
    std::filesystem::path p(import_path);
    if (p.is_absolute() && fileExists(import_path)) {
        return import_path;
    }
    for (const auto& search_dir : search_paths) {
        std::filesystem::path full_path = std::filesystem::path(search_dir) / import_path;
        if (fileExists(full_path.string())) {
            return full_path.string();
        }
    }
    return "";
}

std::string ModuleLoader::readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open imported file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
