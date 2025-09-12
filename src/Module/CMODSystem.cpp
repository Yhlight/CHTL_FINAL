#include "Module/CMODSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {

CMODSystem::CMODSystem() : debug_mode_(false) {
}

void CMODSystem::initialize(const std::string& official_module_path, const std::string& user_module_path) {
    official_module_path_ = official_module_path;
    user_module_path_ = user_module_path;
    
    if (debug_mode_) {
        std::cout << "CMOD System initialized:" << std::endl;
        std::cout << "  Official module path: " << official_module_path_ << std::endl;
        std::cout << "  User module path: " << user_module_path_ << std::endl;
    }
}

std::shared_ptr<CMODModule> CMODSystem::loadModule(const std::string& module_name) {
    if (debug_mode_) {
        std::cout << "Loading CMOD module: " << module_name << std::endl;
    }
    
    // 检查是否已经加载
    auto it = loaded_modules_.find(module_name);
    if (it != loaded_modules_.end()) {
        if (debug_mode_) {
            std::cout << "Module already loaded: " << module_name << std::endl;
        }
        return it->second;
    }
    
    // 搜索模块
    std::string module_path = searchModule(module_name);
    if (module_path.empty()) {
        addError("Module not found: " + module_name);
        return nullptr;
    }
    
    // 从路径加载模块
    return loadModuleFromPath(module_path);
}

std::shared_ptr<CMODModule> CMODSystem::loadModuleFromPath(const std::string& module_path) {
    if (debug_mode_) {
        std::cout << "Loading CMOD module from path: " << module_path << std::endl;
    }
    
    auto module = std::make_shared<CMODModule>();
    module->path = module_path;
    
    // 解析模块信息
    module->info = parseModuleInfo(module_path);
    if (module->info.name.empty()) {
        addError("Failed to parse module info: " + module_path);
        return nullptr;
    }
    
    module->name = module->info.name;
    
    // 解析模块源码
    module->source_code = parseModuleSource(module_path);
    if (module->source_code.empty()) {
        addError("Failed to parse module source: " + module_path);
        return nullptr;
    }
    
    // 解析导出表
    module->info.exports = parseExports(module->source_code);
    
    // 加载子模块
    loadSubmodules(module, module_path);
    
    module->is_loaded = true;
    loaded_modules_[module->name] = module;
    
    if (debug_mode_) {
        std::cout << "Successfully loaded module: " << module->name << std::endl;
        std::cout << "  Version: " << module->info.version << std::endl;
        std::cout << "  Exports: " << module->info.exports.size() << " items" << std::endl;
    }
    
    return module;
}

CMODInfo CMODSystem::parseModuleInfo(const std::string& module_path) {
    CMODInfo info;
    
    // 查找info文件夹
    std::string info_dir = module_path + "/info";
    if (!directoryExists(info_dir)) {
        addError("Info directory not found: " + info_dir);
        return info;
    }
    
    // 查找info.chtl文件
    std::string info_file = info_dir + "/" + std::filesystem::path(module_path).filename().string() + ".chtl";
    if (!fileExists(info_file)) {
        addError("Info file not found: " + info_file);
        return info;
    }
    
    return parseInfoFile(info_file);
}

std::string CMODSystem::parseModuleSource(const std::string& module_path) {
    // 查找src文件夹
    std::string src_dir = module_path + "/src";
    if (!directoryExists(src_dir)) {
        addError("Source directory not found: " + src_dir);
        return "";
    }
    
    // 查找主模块文件
    std::string module_name = std::filesystem::path(module_path).filename().string();
    std::string main_file = src_dir + "/" + module_name + ".chtl";
    
    if (fileExists(main_file)) {
        return readFile(main_file);
    }
    
    // 如果没有主模块文件，尝试查找其他.chtl文件
    auto chtl_files = getFilesInDirectory(src_dir, ".chtl");
    if (!chtl_files.empty()) {
        return readFile(chtl_files[0]);
    }
    
    addError("No source files found in: " + src_dir);
    return "";
}

std::string CMODSystem::searchModule(const std::string& module_name) {
    std::vector<std::string> search_paths;
    
    // 添加官方模块路径
    if (!official_module_path_.empty()) {
        search_paths.push_back(official_module_path_);
    }
    
    // 添加用户模块路径
    if (!user_module_path_.empty()) {
        search_paths.push_back(user_module_path_);
    }
    
    // 添加当前目录
    search_paths.push_back(".");
    
    return searchModuleInPaths(module_name, search_paths);
}

std::unordered_map<std::string, std::string> CMODSystem::getModuleExports(const std::string& module_name) {
    auto module = loadModule(module_name);
    if (module) {
        return module->info.exports;
    }
    return {};
}

bool CMODSystem::moduleExists(const std::string& module_name) {
    return !searchModule(module_name).empty();
}

std::vector<std::shared_ptr<CMODModule>> CMODSystem::getLoadedModules() const {
    std::vector<std::shared_ptr<CMODModule>> modules;
    for (const auto& pair : loaded_modules_) {
        modules.push_back(pair.second);
    }
    return modules;
}

void CMODSystem::clearLoadedModules() {
    loaded_modules_.clear();
}

void CMODSystem::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

std::vector<std::string> CMODSystem::getErrors() const {
    return errors_;
}

void CMODSystem::clearErrors() {
    errors_.clear();
}

void CMODSystem::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "CMOD Error: " << error << std::endl;
    }
}

std::string CMODSystem::searchModuleInPaths(const std::string& module_name, 
                                           const std::vector<std::string>& search_paths) {
    for (const auto& path : search_paths) {
        if (debug_mode_) {
            std::cout << "Searching in path: " << path << std::endl;
        }
        
        // 搜索.cmod文件
        std::string cmod_file = path + "/" + module_name + ".cmod";
        if (fileExists(cmod_file)) {
            if (debug_mode_) {
                std::cout << "Found .cmod file: " << cmod_file << std::endl;
            }
            return cmod_file;
        }
        
        // 搜索.chtl文件
        std::string chtl_file = path + "/" + module_name + ".chtl";
        if (fileExists(chtl_file)) {
            if (debug_mode_) {
                std::cout << "Found .chtl file: " << chtl_file << std::endl;
            }
            return chtl_file;
        }
        
        // 搜索模块目录
        std::string module_dir = path + "/" + module_name;
        if (directoryExists(module_dir)) {
            if (debug_mode_) {
                std::cout << "Found module directory: " << module_dir << std::endl;
            }
            return module_dir;
        }
    }
    
    return "";
}

CMODInfo CMODSystem::parseInfoFile(const std::string& info_file_path) {
    CMODInfo info;
    std::string content = readFile(info_file_path);
    
    if (content.empty()) {
        addError("Failed to read info file: " + info_file_path);
        return info;
    }
    
    // 简单的解析逻辑，查找[Info]块
    std::regex info_regex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, info_regex)) {
        std::string info_content = match[1].str();
        
        // 解析键值对 - 使用简单的字符串分割方法
        std::istringstream iss(info_content);
        std::string line;
        
        while (std::getline(iss, line)) {
            // 查找等号
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);
                
                // 清理键名
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                // 清理值
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // 移除引号
                if (value.length() >= 2 && value[0] == '"' && value[value.length()-1] == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                
                // 移除分号
                if (!value.empty() && value[value.length()-1] == ';') {
                    value = value.substr(0, value.length() - 1);
                }
                
                if (key == "name") info.name = value;
                else if (key == "version") info.version = value;
                else if (key == "description") info.description = value;
                else if (key == "author") info.author = value;
                else if (key == "license") info.license = value;
                else if (key == "dependencies") info.dependencies = value;
                else if (key == "category") info.category = value;
                else if (key == "minCHTLVersion") info.minCHTLVersion = value;
            }
        }
    } else {
        addError("No [Info] block found in: " + info_file_path);
    }
    
    return info;
}

std::unordered_map<std::string, std::string> CMODSystem::parseExports(const std::string& source_code) {
    std::unordered_map<std::string, std::string> exports;
    
    // 查找[Export]块
    std::regex export_regex(R"(\[Export\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(source_code, match, export_regex)) {
        std::string export_content = match[1].str();
        
        // 解析导出项 - 使用简单的字符串分割方法
        std::istringstream iss(export_content);
        std::string line;
        
        while (std::getline(iss, line)) {
            // 查找冒号
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value = line.substr(colon_pos + 1);
                
                // 清理键名
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                // 清理值
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // 移除引号
                if (value.length() >= 2 && value[0] == '"' && value[value.length()-1] == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                
                // 移除分号
                if (!value.empty() && value[value.length()-1] == ';') {
                    value = value.substr(0, value.length() - 1);
                }
                
                exports[key] = value;
            }
        }
    }
    
    return exports;
}

void CMODSystem::loadSubmodules(std::shared_ptr<CMODModule> parent_module, const std::string& module_path) {
    std::string src_dir = module_path + "/src";
    if (!directoryExists(src_dir)) {
        return;
    }
    
    auto subdirs = getFilesInDirectory(src_dir);
    for (const auto& subdir : subdirs) {
        std::string full_path = src_dir + "/" + subdir;
        if (directoryExists(full_path)) {
            auto submodule = loadModuleFromPath(full_path);
            if (submodule) {
                parent_module->submodules.push_back(submodule);
            }
        }
    }
}

bool CMODSystem::fileExists(const std::string& file_path) {
    return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
}

bool CMODSystem::directoryExists(const std::string& dir_path) {
    return std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path);
}

std::vector<std::string> CMODSystem::getFilesInDirectory(const std::string& dir_path, 
                                                        const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (extension.empty() || (filename.length() >= extension.length() && 
                    filename.substr(filename.length() - extension.length()) == extension)) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        addError("Error reading directory " + dir_path + ": " + e.what());
    }
    
    return files;
}

std::string CMODSystem::readFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        addError("Failed to open file: " + file_path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace CHTL