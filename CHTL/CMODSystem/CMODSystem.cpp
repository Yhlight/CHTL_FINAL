#include "CMODSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace CHTL {

// CMODModule implementation
CMODModule::CMODModule() {
}

CMODModule::~CMODModule() {
}

void CMODModule::setInfo(const CMODInfo& info) {
    info_ = info;
}

const CMODInfo& CMODModule::getInfo() const {
    return info_;
}

void CMODModule::addSourceFile(const std::string& filename, const std::string& content) {
    source_files_[filename] = content;
}

const std::map<std::string, std::string>& CMODModule::getSourceFiles() const {
    return source_files_;
}

std::string CMODModule::getSourceFile(const std::string& filename) const {
    auto it = source_files_.find(filename);
    if (it != source_files_.end()) {
        return it->second;
    }
    return "";
}

void CMODModule::addExport(const CMODExport& export_item) {
    exports_.push_back(export_item);
}

const std::vector<CMODExport>& CMODModule::getExports() const {
    return exports_;
}

void CMODModule::addSubModule(const std::string& name, std::shared_ptr<CMODModule> submodule) {
    sub_modules_[name] = submodule;
}

const std::map<std::string, std::shared_ptr<CMODModule>>& CMODModule::getSubModules() const {
    return sub_modules_;
}

std::shared_ptr<CMODModule> CMODModule::getSubModule(const std::string& name) const {
    auto it = sub_modules_.find(name);
    if (it != sub_modules_.end()) {
        return it->second;
    }
    return nullptr;
}

bool CMODModule::isValid() const {
    return validateInfo() && validateSourceFiles() && validateExports();
}

std::vector<std::string> CMODModule::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!validateInfo()) {
        errors.push_back("Invalid module info");
    }
    
    if (!validateSourceFiles()) {
        errors.push_back("Invalid source files");
    }
    
    if (!validateExports()) {
        errors.push_back("Invalid exports");
    }
    
    return errors;
}

bool CMODModule::validateInfo() const {
    return !info_.name.empty() && 
           !info_.version.empty() && 
           !info_.description.empty() && 
           !info_.author.empty() && 
           !info_.license.empty();
}

bool CMODModule::validateSourceFiles() const {
    return !source_files_.empty();
}

bool CMODModule::validateExports() const {
    // 导出项验证逻辑
    return true;
}

// CMODSystem implementation
CMODSystem::CMODSystem() : debug_mode_(false) {
    module_path_ = "./modules";
}

CMODSystem::~CMODSystem() {
}

bool CMODSystem::loadModule(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            if (debug_mode_) {
                std::cerr << "Module path does not exist: " << path << std::endl;
            }
            return false;
        }
        
        if (std::filesystem::is_directory(path)) {
            return loadModuleFromDirectory(path);
        } else if (path.ends_with(".cmod")) {
            return loadModuleFromFile(path);
        } else {
            if (debug_mode_) {
                std::cerr << "Invalid module path: " << path << std::endl;
            }
            return false;
        }
    } catch (const std::exception& e) {
        if (debug_mode_) {
            std::cerr << "Error loading module: " << e.what() << std::endl;
        }
        return false;
    }
}

bool CMODSystem::loadModuleFromFile(const std::string& filepath) {
    // 这里需要实现.cmod文件的解包和加载
    // 暂时返回false
    if (debug_mode_) {
        std::cerr << "Loading from .cmod file not implemented yet: " << filepath << std::endl;
    }
    return false;
}

bool CMODSystem::loadModuleFromDirectory(const std::string& dirpath) {
    try {
        auto module = std::make_shared<CMODModule>();
        
        // 查找info文件
        std::string infoPath = dirpath + "/info";
        if (std::filesystem::exists(infoPath)) {
            std::vector<std::string> infoFiles = getFilesInDirectory(infoPath);
            for (const std::string& infoFile : infoFiles) {
                if (infoFile.ends_with(".chtl")) {
                    std::string content = readFile(infoPath + "/" + infoFile);
                    CMODInfo info;
                    if (parseModuleInfo(content, info)) {
                        module->setInfo(info);
                    }
                    
                    // 解析导出项
                    std::vector<CMODExport> exports;
                    if (parseModuleExports(content, exports)) {
                        for (const CMODExport& export_item : exports) {
                            module->addExport(export_item);
                        }
                    }
                    break;
                }
            }
        }
        
        // 查找src文件
        std::string srcPath = dirpath + "/src";
        if (std::filesystem::exists(srcPath)) {
            std::vector<std::string> srcFiles = getFilesInDirectory(srcPath);
            for (const std::string& srcFile : srcFiles) {
                if (srcFile.ends_with(".chtl")) {
                    std::string content = readFile(srcPath + "/" + srcFile);
                    module->addSourceFile(srcFile, content);
                }
            }
        }
        
        // 查找子模块
        std::vector<std::string> subdirs = getFilesInDirectory(dirpath);
        for (const std::string& subdir : subdirs) {
            std::string subdirPath = dirpath + "/" + subdir;
            if (isDirectory(subdirPath) && subdir != "src" && subdir != "info") {
                auto subModule = std::make_shared<CMODModule>();
                if (loadModuleFromDirectory(subdirPath)) {
                    module->addSubModule(subdir, subModule);
                }
            }
        }
        
        if (module->isValid()) {
            modules_[module->getInfo().name] = module;
            return true;
        } else {
            if (debug_mode_) {
                std::vector<std::string> errors = module->getValidationErrors();
                for (const std::string& error : errors) {
                    std::cerr << "Module validation error: " << error << std::endl;
                }
            }
            return false;
        }
    } catch (const std::exception& e) {
        if (debug_mode_) {
            std::cerr << "Error loading module from directory: " << e.what() << std::endl;
        }
        return false;
    }
}

bool CMODSystem::saveModule(const std::string& name, const std::string& path) {
    auto module = getModule(name);
    if (!module) {
        if (debug_mode_) {
            std::cerr << "Module not found: " << name << std::endl;
        }
        return false;
    }
    
    try {
        // 创建模块目录
        std::filesystem::create_directories(path);
        
        // 保存info文件
        std::string infoPath = path + "/info";
        std::filesystem::create_directories(infoPath);
        
        std::string infoContent = generateInfoContent(module);
        writeFile(infoPath + "/" + name + ".chtl", infoContent);
        
        // 保存src文件
        std::string srcPath = path + "/src";
        std::filesystem::create_directories(srcPath);
        
        const auto& sourceFiles = module->getSourceFiles();
        for (const auto& file : sourceFiles) {
            writeFile(srcPath + "/" + file.first, file.second);
        }
        
        // 保存子模块
        const auto& subModules = module->getSubModules();
        for (const auto& subModule : subModules) {
            std::string subModulePath = path + "/" + subModule.first;
            saveModule(subModule.first, subModulePath);
        }
        
        return true;
    } catch (const std::exception& e) {
        if (debug_mode_) {
            std::cerr << "Error saving module: " << e.what() << std::endl;
        }
        return false;
    }
}

std::shared_ptr<CMODModule> CMODSystem::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    if (it != modules_.end()) {
        return it->second;
    }
    return nullptr;
}

const std::map<std::string, std::shared_ptr<CMODModule>>& CMODSystem::getAllModules() const {
    return modules_;
}

std::vector<std::string> CMODSystem::searchModules(const std::string& query) const {
    std::vector<std::string> results;
    
    for (const auto& module : modules_) {
        const CMODInfo& info = module.second->getInfo();
        if (info.name.find(query) != std::string::npos ||
            info.description.find(query) != std::string::npos ||
            info.category.find(query) != std::string::npos) {
            results.push_back(module.first);
        }
    }
    
    return results;
}

std::vector<std::string> CMODSystem::getModulesByCategory(const std::string& category) const {
    std::vector<std::string> results;
    
    for (const auto& module : modules_) {
        if (module.second->getInfo().category == category) {
            results.push_back(module.first);
        }
    }
    
    return results;
}

std::vector<std::string> CMODSystem::resolveDependencies(const std::string& moduleName) const {
    std::vector<std::string> dependencies;
    // 这里实现依赖解析逻辑
    return dependencies;
}

bool CMODSystem::checkDependencies(const std::string& moduleName) const {
    // 这里实现依赖检查逻辑
    return true;
}

bool CMODSystem::packModule(const std::string& moduleName, const std::string& outputPath) {
    // 这里实现模块打包逻辑
    if (debug_mode_) {
        std::cout << "Packing module " << moduleName << " to " << outputPath << std::endl;
    }
    return true;
}

bool CMODSystem::unpackModule(const std::string& filepath, const std::string& outputPath) {
    // 这里实现模块解包逻辑
    if (debug_mode_) {
        std::cout << "Unpacking module from " << filepath << " to " << outputPath << std::endl;
    }
    return true;
}

void CMODSystem::setModulePath(const std::string& path) {
    module_path_ = path;
}

const std::string& CMODSystem::getModulePath() const {
    return module_path_;
}

void CMODSystem::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

bool CMODSystem::isDebugMode() const {
    return debug_mode_;
}

bool CMODSystem::parseModuleInfo(const std::string& content, CMODInfo& info) {
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoRegex)) {
        std::string infoContent = match[1].str();
        
        // 解析各个字段
        std::regex fieldRegex(R"(\s*(\w+)\s*=\s*"([^"]+)"\s*)");
        std::sregex_iterator iter(infoContent.begin(), infoContent.end(), fieldRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string key = (*iter)[1].str();
            std::string value = (*iter)[2].str();
            
            if (key == "name") info.name = value;
            else if (key == "version") info.version = value;
            else if (key == "description") info.description = value;
            else if (key == "author") info.author = value;
            else if (key == "license") info.license = value;
            else if (key == "dependencies") info.dependencies = value;
            else if (key == "category") info.category = value;
            else if (key == "minCHTLVersion") info.minCHTLVersion = value;
            else if (key == "maxCHTLVersion") info.maxCHTLVersion = value;
        }
        
        return true;
    }
    
    return false;
}

bool CMODSystem::parseModuleExports(const std::string& content, std::vector<CMODExport>& exports) {
    std::regex exportRegex(R"(\[Export\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, exportRegex)) {
        std::string exportContent = match[1].str();
        
        // 解析导出项
        std::regex itemRegex(R"(\s*(\[Custom\]|\[Template\]|\[Origin\]|\[Configuration\])\s*@(\w+)\s+([^,;]+)(?:,|\s*;))");
        std::sregex_iterator iter(exportContent.begin(), exportContent.end(), itemRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            CMODExport export_item;
            export_item.type = (*iter)[1].str();
            export_item.name = (*iter)[2].str();
            export_item.content = (*iter)[3].str();
            exports.push_back(export_item);
        }
        
        return true;
    }
    
    return false;
}

std::string CMODSystem::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CMODSystem::writeFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::vector<std::string> CMODSystem::getFilesInDirectory(const std::string& dirpath) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirpath)) {
            std::string filename = entry.path().filename().string();
            files.push_back(filename);
        }
    } catch (const std::exception& e) {
        if (debug_mode_) {
            std::cerr << "Error reading directory: " << e.what() << std::endl;
        }
    }
    
    return files;
}

bool CMODSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool CMODSystem::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string CMODSystem::generateInfoContent(std::shared_ptr<CMODModule> module) const {
    std::stringstream ss;
    const CMODInfo& info = module->getInfo();
    
    ss << "[Info]\n";
    ss << "{\n";
    ss << "    name = \"" << info.name << "\";\n";
    ss << "    version = \"" << info.version << "\";\n";
    ss << "    description = \"" << info.description << "\";\n";
    ss << "    author = \"" << info.author << "\";\n";
    ss << "    license = \"" << info.license << "\";\n";
    ss << "    dependencies = \"" << info.dependencies << "\";\n";
    ss << "    category = \"" << info.category << "\";\n";
    ss << "    minCHTLVersion = \"" << info.minCHTLVersion << "\";\n";
    ss << "    maxCHTLVersion = \"" << info.maxCHTLVersion << "\";\n";
    ss << "}\n\n";
    
    const auto& exports = module->getExports();
    if (!exports.empty()) {
        ss << "[Export]\n";
        ss << "{\n";
        for (const CMODExport& export_item : exports) {
            ss << "    " << export_item.type << " @";
            ss << export_item.name << " " << export_item.content << ";\n";
        }
        ss << "}\n";
    }
    
    return ss.str();
}

} // namespace CHTL