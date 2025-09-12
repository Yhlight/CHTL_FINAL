#include "Module/CJMODSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {

// Arg implementation
void Arg::print() const {
    std::cout << "Arg: [";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << values[i] << "\"";
    }
    std::cout << "]" << std::endl;
}

void Arg::fillValue(const Arg& other) {
    if (other.values.size() <= values.size()) {
        for (size_t i = 0; i < other.values.size(); ++i) {
            values[i] = other.values[i];
        }
    }
}

std::string Arg::getValue(size_t index) const {
    if (index < values.size()) {
        return values[index];
    }
    return "";
}

void Arg::setValue(size_t index, const std::string& value) {
    if (index < values.size()) {
        values[index] = value;
    }
}

// Syntax implementation
Arg Syntax::analyze(const std::string& syntax) {
    Arg result;
    
    // 简单的语法分析，按空格分割
    std::istringstream iss(syntax);
    std::string token;
    
    while (iss >> token) {
        result.values.push_back(token);
    }
    
    return result;
}

// CJMODScanner implementation
Arg CJMODScanner::scan(const Arg& args, const std::string& keyword) {
    Arg result;
    
    if (keyword.empty()) {
        // 如果没有指定关键字，返回原始参数
        return args;
    }
    
    // 查找关键字并提取相关参数
    for (size_t i = 0; i < args.values.size(); ++i) {
        if (args.values[i] == keyword) {
            // 找到关键字，提取前后的参数
            if (i > 0) {
                result.values.push_back(args.values[i - 1]);
            }
            result.values.push_back(args.values[i]);
            if (i + 1 < args.values.size()) {
                result.values.push_back(args.values[i + 1]);
            }
            break;
        }
    }
    
    return result;
}

// CJMODGenerator implementation
std::string CJMODGenerator::exportResult(const Arg& args) {
    std::string result;
    
    for (size_t i = 0; i < args.values.size(); ++i) {
        if (i > 0) result += " ";
        result += args.values[i];
    }
    
    return result;
}

// CHTLJSFunction implementation
std::string CHTLJSFunction::createFunction(const std::string& name, 
                                         const std::vector<std::string>& parameters,
                                         const std::string& body) {
    std::string result = "function " + name + "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) result += ", ";
        result += parameters[i];
    }
    
    result += ") {\n";
    result += "  " + body + "\n";
    result += "}";
    
    return result;
}

// CJMODSystem implementation
CJMODSystem::CJMODSystem() : debug_mode_(false) {
}

void CJMODSystem::initialize(const std::string& official_module_path, const std::string& user_module_path) {
    official_module_path_ = official_module_path;
    user_module_path_ = user_module_path;
    
    if (debug_mode_) {
        std::cout << "CJMOD System initialized:" << std::endl;
        std::cout << "  Official module path: " << official_module_path_ << std::endl;
        std::cout << "  User module path: " << user_module_path_ << std::endl;
    }
}

std::shared_ptr<CJMODModule> CJMODSystem::loadModule(const std::string& module_name) {
    if (debug_mode_) {
        std::cout << "Loading CJMOD module: " << module_name << std::endl;
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

std::shared_ptr<CJMODModule> CJMODSystem::loadModuleFromPath(const std::string& module_path) {
    if (debug_mode_) {
        std::cout << "Loading CJMOD module from path: " << module_path << std::endl;
    }
    
    auto module = std::make_shared<CJMODModule>();
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
    
    // 加载子模块
    loadSubmodules(module, module_path);
    
    module->is_loaded = true;
    loaded_modules_[module->name] = module;
    
    if (debug_mode_) {
        std::cout << "Successfully loaded module: " << module->name << std::endl;
        std::cout << "  Version: " << module->info.version << std::endl;
        std::cout << "  Functions: " << module->functions.size() << " items" << std::endl;
    }
    
    return module;
}

CJMODInfo CJMODSystem::parseModuleInfo(const std::string& module_path) {
    CJMODInfo info;
    
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

std::string CJMODSystem::parseModuleSource(const std::string& module_path) {
    // 查找src文件夹
    std::string src_dir = module_path + "/src";
    if (!directoryExists(src_dir)) {
        addError("Source directory not found: " + src_dir);
        return "";
    }
    
    // 查找C++源文件
    auto cpp_files = getFilesInDirectory(src_dir, ".cpp");
    if (!cpp_files.empty()) {
        return readFile(cpp_files[0]);
    }
    
    // 查找头文件
    auto h_files = getFilesInDirectory(src_dir, ".h");
    if (!h_files.empty()) {
        return readFile(h_files[0]);
    }
    
    addError("No source files found in: " + src_dir);
    return "";
}

std::string CJMODSystem::searchModule(const std::string& module_name) {
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

bool CJMODSystem::moduleExists(const std::string& module_name) {
    return !searchModule(module_name).empty();
}

std::vector<std::shared_ptr<CJMODModule>> CJMODSystem::getLoadedModules() const {
    std::vector<std::shared_ptr<CJMODModule>> modules;
    for (const auto& pair : loaded_modules_) {
        modules.push_back(pair.second);
    }
    return modules;
}

void CJMODSystem::clearLoadedModules() {
    loaded_modules_.clear();
}

std::string CJMODSystem::executeFunction(const std::string& module_name, 
                                        const std::string& function_name,
                                        const std::vector<std::string>& args) {
    auto module = loadModule(module_name);
    if (!module) {
        addError("Module not found: " + module_name);
        return "";
    }
    
    auto it = module->functions.find(function_name);
    if (it == module->functions.end()) {
        addError("Function not found: " + function_name + " in module: " + module_name);
        return "";
    }
    
    try {
        return it->second(args);
    } catch (const std::exception& e) {
        addError("Error executing function " + function_name + ": " + e.what());
        return "";
    }
}

void CJMODSystem::registerFunction(const std::string& module_name,
                                 const std::string& function_name,
                                 std::function<std::string(const std::vector<std::string>&)> function) {
    auto module = loadModule(module_name);
    if (module) {
        module->functions[function_name] = function;
        
        if (debug_mode_) {
            std::cout << "Registered function: " << function_name << " in module: " << module_name << std::endl;
        }
    }
}

void CJMODSystem::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

std::vector<std::string> CJMODSystem::getErrors() const {
    return errors_;
}

void CJMODSystem::clearErrors() {
    errors_.clear();
}

void CJMODSystem::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "CJMOD Error: " << error << std::endl;
    }
}

std::string CJMODSystem::searchModuleInPaths(const std::string& module_name, 
                                            const std::vector<std::string>& search_paths) {
    for (const auto& path : search_paths) {
        if (debug_mode_) {
            std::cout << "Searching in path: " << path << std::endl;
        }
        
        // 搜索.cjmod文件
        std::string cjmod_file = path + "/" + module_name + ".cjmod";
        if (fileExists(cjmod_file)) {
            if (debug_mode_) {
                std::cout << "Found .cjmod file: " << cjmod_file << std::endl;
            }
            return cjmod_file;
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

CJMODInfo CJMODSystem::parseInfoFile(const std::string& info_file_path) {
    CJMODInfo info;
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

void CJMODSystem::loadSubmodules(std::shared_ptr<CJMODModule> parent_module, const std::string& module_path) {
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

bool CJMODSystem::fileExists(const std::string& file_path) {
    return std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path);
}

bool CJMODSystem::directoryExists(const std::string& dir_path) {
    return std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path);
}

std::vector<std::string> CJMODSystem::getFilesInDirectory(const std::string& dir_path, 
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

std::string CJMODSystem::readFile(const std::string& file_path) {
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