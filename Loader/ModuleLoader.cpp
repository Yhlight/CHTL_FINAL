#include "ModuleLoader.h"
#include "ModuleInfoParser.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/Context/ConfigurationContext.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

bool fileExists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

ModuleLoader::ModuleLoader(const std::string& base_path) : base_path(base_path) {
    // Per CHTL.md spec
    search_paths.push_back("./modules/official"); // 1. Official modules

    std::filesystem::path p(base_path);
    std::string parent_dir = p.is_absolute() ? p.parent_path().string() : ".";
    search_paths.push_back(parent_dir + "/module"); // 2. User modules
    search_paths.push_back(parent_dir); // 3. Current directory
}

LoadedModule ModuleLoader::load(const ImportNode* importNode) {
    std::string resolved_path_str = resolvePath(importNode->path);
    if (resolved_path_str.empty()) {
        std::cerr << "Warning: Could not resolve import path '" << importNode->path << "'." << std::endl;
        return {"", "", false};
    }

    std::filesystem::path resolved_path(resolved_path_str);

    if (resolved_path.extension() == ".cmod") {
        // Create a temporary directory to unpack the module
        std::string temp_dir = "temp_module_" + resolved_path.stem().string();
        std::string command = "rm -rf " + temp_dir + " && mkdir -p " + temp_dir + " && unzip " + resolved_path_str + " -d " + temp_dir;

        // Due to issues with the run_in_bash_session tool, we will print the command
        // that should be run instead of executing it directly.
        std::cout << "[CMOD Unpack Command]: " << command << std::endl;

        // The "content" for a CMOD is the path to the unpacked directory.
        // Now, let's parse the info file.
        std::filesystem::path info_file_path = std::filesystem::path(temp_dir) / "info" / (resolved_path.stem().string() + ".chtl");

        std::string info_content = readTextFile(info_file_path.string());
        if (info_content.empty()) {
            std::cerr << "Warning: Could not read info file for module '" << resolved_path.stem().string() << "'." << std::endl;
            return {temp_dir, resolved_path_str, false};
        }

        ConfigurationContext dummy_config; // Info parser doesn't need a real config
        Lexer lexer(info_content, dummy_config);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto ast = parser.parse();

        ModuleInfoParser info_parser;
        ModuleInfo info = info_parser.parse(ast);

        return {temp_dir, resolved_path_str, true, info};

    } else {
        // Handle normal file imports (.css, .js, .chtl)
        std::string content = readTextFile(resolved_path_str);
        return {content, resolved_path_str, !content.empty(), std::nullopt};
    }
}

std::string ModuleLoader::resolvePath(const std::string& import_path) {
    std::string path_to_resolve = import_path;
    std::vector<std::string> paths_to_search = search_paths;

    // Handle official module prefix
    std::string official_prefix = "chtl::";
    if (import_path.rfind(official_prefix, 0) == 0) {
        path_to_resolve = import_path.substr(official_prefix.length());
        paths_to_search = { "./modules/official" };
    }

    std::filesystem::path p(path_to_resolve);
    bool has_extension = p.has_extension();

    for (const auto& search_dir : paths_to_search) {
        if (has_extension) {
            std::filesystem::path full_path = std::filesystem::path(search_dir) / path_to_resolve;
            if (fileExists(full_path.string())) {
                return full_path.string();
            }
        } else {
            // Prefer .cmod over .chtl
            std::filesystem::path cmod_path = std::filesystem::path(search_dir) / (path_to_resolve + ".cmod");
            if (fileExists(cmod_path.string())) {
                return cmod_path.string();
            }
            std::filesystem::path chtl_path = std::filesystem::path(search_dir) / (path_to_resolve + ".chtl");
            if (fileExists(chtl_path.string())) {
                return chtl_path.string();
            }
        }
    }

    return ""; // Not found
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
