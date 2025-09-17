#include "CHTLLoader.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

CHTLLoader::CHTLLoader() : cache_enabled_(true) {
    context_ = std::make_shared<CHTLContext>();
    initializeDefaultPaths();
    initializeSearchPaths();
}

CHTLLoader::~CHTLLoader() {
}

void CHTLLoader::initializeDefaultPaths() {
    // 设置默认模块路径
    module_path_ = "./modules";
    official_module_path_ = "./official_modules";
    user_module_path_ = "./user_modules";
}

void CHTLLoader::initializeSearchPaths() {
    // 添加默认搜索路径
    search_paths_.push_back(".");
    search_paths_.push_back("./src");
    search_paths_.push_back("./include");
    search_paths_.push_back(module_path_);
    search_paths_.push_back(official_module_path_);
    search_paths_.push_back(user_module_path_);
}

std::string CHTLLoader::loadFile(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    
    // 检查是否已加载且未修改
    if (cache_enabled_ && loaded_files_.find(normalizedPath) != loaded_files_.end()) {
        FileInfo& fileInfo = loaded_files_[normalizedPath];
        if (!isFileModified(normalizedPath)) {
            return fileInfo.content;
        }
    }
    
    // 加载文件内容
    std::string content = loadFileContent(normalizedPath);
    if (content.empty()) {
        reportError("Failed to load file: " + normalizedPath);
        return "";
    }
    
    // 创建文件信息
    FileInfo fileInfo(normalizedPath);
    fileInfo.content = content;
    fileInfo.lastModified = FileSystem::getLastModified(normalizedPath);
    
    // 提取依赖
    fileInfo.dependencies = extractDependencies(content);
    
    // 检查循环依赖
    if (checkCircularDependencies(normalizedPath)) {
        reportError("Circular dependency detected for file: " + normalizedPath);
        return "";
    }
    
    // 解析AST
    try {
        CHTLLexer lexer(content);
        std::vector<Token> tokens = lexer.tokenize();
        CHTLParser parser(tokens);
        fileInfo.ast = parser.parse();
        fileInfo.isParsed = true;
    } catch (const std::exception& e) {
        reportError("Failed to parse file: " + std::string(e.what()));
        return content; // 返回原始内容
    }
    
    // 缓存文件信息
    loaded_files_[normalizedPath] = fileInfo;
    updateFileTimestamp(normalizedPath);
    
    return content;
}

std::string CHTLLoader::loadModule(const std::string& moduleName, const std::string& version) {
    // 检查是否已加载
    if (loaded_modules_.find(moduleName) != loaded_modules_.end()) {
        ModuleInfo& moduleInfo = loaded_modules_[moduleName];
        if (!isModuleModified(moduleInfo.path)) {
            return moduleInfo.content;
        }
    }
    
    // 查找模块路径
    std::string modulePath = findModulePath(moduleName, version);
    if (modulePath.empty()) {
        reportError("Module not found: " + moduleName);
        return "";
    }
    
    // 加载模块内容
    std::string content = loadModuleContent(modulePath);
    if (content.empty()) {
        reportError("Failed to load module: " + moduleName);
        return "";
    }
    
    // 创建模块信息
    ModuleInfo moduleInfo(moduleName, modulePath, detectModuleType(modulePath));
    moduleInfo.content = content;
    moduleInfo.lastModified = FileSystem::getLastModified(modulePath);
    
    // 解析模块信息
    std::string infoContent = parseModuleInfo(content);
    if (!infoContent.empty()) {
        // 解析模块元信息
        // 这里可以实现更复杂的模块信息解析
    }
    
    // 提取依赖
    moduleInfo.dependencies = extractDependencies(content);
    
    // 缓存模块信息
    loaded_modules_[moduleName] = moduleInfo;
    updateModuleTimestamp(modulePath);
    
    return content;
}

std::string CHTLLoader::loadImport(const std::string& importPath, ImportType type, 
                                  const std::string& alias, const std::string& namespaceName) {
    std::string resolvedPath = resolvePath(importPath);
    
    if (resolvedPath.empty()) {
        reportError("Failed to resolve import path: " + importPath);
        return "";
    }
    
    // 根据类型加载内容
    switch (type) {
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
        case ImportType::CHTL:
            return loadFile(resolvedPath);
        case ImportType::CJMOD:
            return loadModule(resolvedPath);
        default:
            reportError("Unknown import type");
            return "";
    }
}

bool CHTLLoader::addFile(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    
    if (loaded_files_.find(normalizedPath) != loaded_files_.end()) {
        return true; // 已存在
    }
    
    std::string content = loadFile(normalizedPath);
    return !content.empty();
}

bool CHTLLoader::removeFile(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    
    auto it = loaded_files_.find(normalizedPath);
    if (it != loaded_files_.end()) {
        loaded_files_.erase(it);
        file_timestamps_.erase(normalizedPath);
        return true;
    }
    
    return false;
}

bool CHTLLoader::reloadFile(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    
    // 清除缓存
    clearFileCache(normalizedPath);
    
    // 重新加载
    return addFile(normalizedPath);
}

bool CHTLLoader::isFileLoaded(const std::string& filePath) const {
    std::string normalizedPath = normalizePath(filePath);
    return loaded_files_.find(normalizedPath) != loaded_files_.end();
}

CHTLLoader::FileInfo* CHTLLoader::getFileInfo(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    auto it = loaded_files_.find(normalizedPath);
    if (it != loaded_files_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CHTLLoader::addModule(const std::string& modulePath) {
    std::string normalizedPath = normalizePath(modulePath);
    
    // 检测模块名称
    std::string moduleName = FileSystem::getStem(normalizedPath);
    
    if (loaded_modules_.find(moduleName) != loaded_modules_.end()) {
        return true; // 已存在
    }
    
    std::string content = loadModule(moduleName);
    return !content.empty();
}

bool CHTLLoader::removeModule(const std::string& moduleName) {
    auto it = loaded_modules_.find(moduleName);
    if (it != loaded_modules_.end()) {
        loaded_modules_.erase(it);
        module_timestamps_.erase(it->second.path);
        return true;
    }
    
    return false;
}

bool CHTLLoader::reloadModule(const std::string& moduleName) {
    // 清除缓存
    clearModuleCache(moduleName);
    
    // 重新加载
    return addModule(findModulePath(moduleName));
}

bool CHTLLoader::isModuleLoaded(const std::string& moduleName) const {
    return loaded_modules_.find(moduleName) != loaded_modules_.end();
}

CHTLLoader::ModuleInfo* CHTLLoader::getModuleInfo(const std::string& moduleName) {
    auto it = loaded_modules_.find(moduleName);
    if (it != loaded_modules_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<CHTLLoader::ModuleInfo> CHTLLoader::getAllModules() const {
    std::vector<ModuleInfo> result;
    for (const auto& pair : loaded_modules_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<CHTLLoader::DependencyInfo> CHTLLoader::resolveDependencies(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    
    auto it = loaded_files_.find(normalizedPath);
    if (it != loaded_files_.end()) {
        return it->second.dependencies;
    }
    
    return {};
}

std::vector<CHTLLoader::DependencyInfo> CHTLLoader::resolveModuleDependencies(const std::string& moduleName) {
    auto it = loaded_modules_.find(moduleName);
    if (it != loaded_modules_.end()) {
        return it->second.dependencies;
    }
    
    return {};
}

bool CHTLLoader::checkCircularDependencies(const std::string& filePath) {
    std::set<std::string> visited;
    std::set<std::string> recursionStack;
    return hasCircularDependency(filePath, visited, recursionStack);
}

std::vector<std::string> CHTLLoader::getDependencyChain(const std::string& filePath) {
    std::vector<std::string> chain;
    std::set<std::string> visited;
    buildDependencyChain(filePath, chain, visited);
    return chain;
}

std::string CHTLLoader::resolvePath(const std::string& path, const std::string& basePath) {
    if (isAbsolutePath(path)) {
        return normalizePath(path);
    }
    
    // 相对路径解析
    std::string base = basePath.empty() ? FileSystem::getCurrentPath() : basePath;
    std::string fullPath = joinPath(base, path);
    
    if (FileSystem::exists(fullPath)) {
        return normalizePath(fullPath);
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : search_paths_) {
        std::string candidatePath = joinPath(searchPath, path);
        if (FileSystem::exists(candidatePath)) {
            return normalizePath(candidatePath);
        }
    }
    
    return "";
}

std::string CHTLLoader::findModulePath(const std::string& moduleName, const std::string& version) {
    // 在用户模块目录查找
    std::string userPath = findModuleInPath(moduleName, user_module_path_);
    if (!userPath.empty()) {
        return userPath;
    }
    
    // 在官方模块目录查找
    std::string officialPath = findModuleInPath(moduleName, official_module_path_);
    if (!officialPath.empty()) {
        return officialPath;
    }
    
    // 在模块目录查找
    std::string modulePath = findModuleInPath(moduleName, module_path_);
    if (!modulePath.empty()) {
        return modulePath;
    }
    
    return "";
}

std::string CHTLLoader::findFile(const std::string& fileName, const std::string& basePath) {
    // 在基础路径查找
    if (!basePath.empty()) {
        std::string candidatePath = joinPath(basePath, fileName);
        if (FileSystem::exists(candidatePath)) {
            return normalizePath(candidatePath);
        }
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : search_paths_) {
        std::string candidatePath = joinPath(searchPath, fileName);
        if (FileSystem::exists(candidatePath)) {
            return normalizePath(candidatePath);
        }
    }
    
    return "";
}

void CHTLLoader::addSearchPath(const std::string& path) {
    std::string normalizedPath = normalizePath(path);
    if (std::find(search_paths_.begin(), search_paths_.end(), normalizedPath) == search_paths_.end()) {
        search_paths_.push_back(normalizedPath);
    }
}

void CHTLLoader::removeSearchPath(const std::string& path) {
    std::string normalizedPath = normalizePath(path);
    search_paths_.erase(std::remove(search_paths_.begin(), search_paths_.end(), normalizedPath), 
                       search_paths_.end());
}

void CHTLLoader::clearSearchPaths() {
    search_paths_.clear();
    initializeSearchPaths();
}

std::vector<std::string> CHTLLoader::getSearchPaths() const {
    return search_paths_;
}

void CHTLLoader::setModulePath(const std::string& path) {
    module_path_ = normalizePath(path);
}

std::string CHTLLoader::getModulePath() const {
    return module_path_;
}

void CHTLLoader::setOfficialModulePath(const std::string& path) {
    official_module_path_ = normalizePath(path);
}

std::string CHTLLoader::getOfficialModulePath() const {
    return official_module_path_;
}

void CHTLLoader::setUserModulePath(const std::string& path) {
    user_module_path_ = normalizePath(path);
}

std::string CHTLLoader::getUserModulePath() const {
    return user_module_path_;
}

void CHTLLoader::enableCache(bool enable) {
    cache_enabled_ = enable;
}

bool CHTLLoader::isCacheEnabled() const {
    return cache_enabled_;
}

void CHTLLoader::clearCache() {
    file_timestamps_.clear();
    module_timestamps_.clear();
}

void CHTLLoader::clearFileCache(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    file_timestamps_.erase(normalizedPath);
    loaded_files_.erase(normalizedPath);
}

void CHTLLoader::clearModuleCache(const std::string& moduleName) {
    auto it = loaded_modules_.find(moduleName);
    if (it != loaded_modules_.end()) {
        module_timestamps_.erase(it->second.path);
        loaded_modules_.erase(it);
    }
}

void CHTLLoader::setErrorHandler(std::function<void(const std::string&, int, int)> handler) {
    error_handler_ = handler;
}

void CHTLLoader::reportError(const std::string& message, int line, int column) {
    logError(message, line, column);
    
    if (error_handler_) {
        error_handler_(message, line, column);
    }
}

bool CHTLLoader::hasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CHTLLoader::getErrors() const {
    return errors_;
}

void CHTLLoader::clearErrors() {
    errors_.clear();
}

size_t CHTLLoader::getLoadedFileCount() const {
    return loaded_files_.size();
}

size_t CHTLLoader::getLoadedModuleCount() const {
    return loaded_modules_.size();
}

size_t CHTLLoader::getCacheSize() const {
    return file_timestamps_.size() + module_timestamps_.size();
}

std::string CHTLLoader::getCacheInfo() const {
    std::stringstream ss;
    ss << "Files: " << loaded_files_.size() << ", ";
    ss << "Modules: " << loaded_modules_.size() << ", ";
    ss << "Cache: " << getCacheSize() << " entries";
    return ss.str();
}

void CHTLLoader::clear() {
    loaded_files_.clear();
    loaded_modules_.clear();
    file_timestamps_.clear();
    module_timestamps_.clear();
    errors_.clear();
}

std::string CHTLLoader::loadFileContent(const std::string& filePath) {
    return FileSystem::readFile(filePath);
}

std::string CHTLLoader::loadModuleContent(const std::string& modulePath) {
    return FileSystem::readFile(modulePath);
}

std::string CHTLLoader::parseModuleInfo(const std::string& content) {
    // 简单的模块信息解析
    // 实际实现中应该解析[Info]块
    std::regex infoRegex(R"(\[Info\]\s*\{([^}]+)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, infoRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::vector<CHTLLoader::DependencyInfo> CHTLLoader::extractDependencies(const std::string& content) {
    std::vector<DependencyInfo> dependencies;
    
    // 解析@Html导入
    std::regex htmlRegex(R"(@Html\s+from\s+["']([^"']+)["'](?:\s+as\s+(\w+))?)");
    std::sregex_iterator htmlBegin(content.begin(), content.end(), htmlRegex);
    std::sregex_iterator htmlEnd;
    
    for (auto it = htmlBegin; it != htmlEnd; ++it) {
        std::string path = (*it)[1].str();
        std::string alias = (*it)[2].str();
        dependencies.emplace_back(path, path, ImportType::HTML, ModuleType::DIRECT);
        dependencies.back().alias = alias;
    }
    
    // 解析@Style导入
    std::regex styleRegex(R"(@Style\s+from\s+["']([^"']+)["'](?:\s+as\s+(\w+))?)");
    std::sregex_iterator styleBegin(content.begin(), content.end(), styleRegex);
    std::sregex_iterator styleEnd;
    
    for (auto it = styleBegin; it != styleEnd; ++it) {
        std::string path = (*it)[1].str();
        std::string alias = (*it)[2].str();
        dependencies.emplace_back(path, path, ImportType::STYLE, ModuleType::DIRECT);
        dependencies.back().alias = alias;
    }
    
    // 解析@JavaScript导入
    std::regex jsRegex(R"(@JavaScript\s+from\s+["']([^"']+)["'](?:\s+as\s+(\w+))?)");
    std::sregex_iterator jsBegin(content.begin(), content.end(), jsRegex);
    std::sregex_iterator jsEnd;
    
    for (auto it = jsBegin; it != jsEnd; ++it) {
        std::string path = (*it)[1].str();
        std::string alias = (*it)[2].str();
        dependencies.emplace_back(path, path, ImportType::JAVASCRIPT, ModuleType::DIRECT);
        dependencies.back().alias = alias;
    }
    
    // 解析@Chtl导入
    std::regex chtlRegex(R"(@Chtl\s+from\s+["']([^"']+)["'](?:\s+as\s+(\w+))?)");
    std::sregex_iterator chtlBegin(content.begin(), content.end(), chtlRegex);
    std::sregex_iterator chtlEnd;
    
    for (auto it = chtlBegin; it != chtlEnd; ++it) {
        std::string path = (*it)[1].str();
        std::string alias = (*it)[2].str();
        dependencies.emplace_back(path, path, ImportType::CHTL, ModuleType::CMOD);
        dependencies.back().alias = alias;
    }
    
    // 解析@CJmod导入
    std::regex cjmodRegex(R"(@CJmod\s+from\s+["']([^"']+)["'](?:\s+as\s+(\w+))?)");
    std::sregex_iterator cjmodBegin(content.begin(), content.end(), cjmodRegex);
    std::sregex_iterator cjmodEnd;
    
    for (auto it = cjmodBegin; it != cjmodEnd; ++it) {
        std::string path = (*it)[1].str();
        std::string alias = (*it)[2].str();
        dependencies.emplace_back(path, path, ImportType::CJMOD, ModuleType::CJMOD);
        dependencies.back().alias = alias;
    }
    
    return dependencies;
}

std::string CHTLLoader::normalizePath(const std::string& path) {
    return FileSystem::normalize(path);
}

std::string CHTLLoader::joinPath(const std::string& base, const std::string& relative) {
    return FileSystem::join(base, relative);
}

bool CHTLLoader::isAbsolutePath(const std::string& path) {
    return !path.empty() && (path[0] == '/' || path[0] == '\\' || 
            (path.length() > 1 && path[1] == ':'));
}

std::string CHTLLoader::getDirectory(const std::string& path) {
    return FileSystem::getDirectory(path);
}

CHTLLoader::ModuleType CHTLLoader::detectModuleType(const std::string& path) {
    std::string extension = FileSystem::getExtension(path);
    
    if (extension == ".cmod") {
        return ModuleType::CMOD;
    } else if (extension == ".cjmod") {
        return ModuleType::CJMOD;
    } else if (extension == ".chtl") {
        return ModuleType::DIRECT;
    }
    
    return ModuleType::UNKNOWN;
}

CHTLLoader::ImportType CHTLLoader::detectImportType(const std::string& path) {
    std::string extension = FileSystem::getExtension(path);
    
    if (extension == ".html" || extension == ".htm") {
        return ImportType::HTML;
    } else if (extension == ".css") {
        return ImportType::STYLE;
    } else if (extension == ".js") {
        return ImportType::JAVASCRIPT;
    } else if (extension == ".chtl") {
        return ImportType::CHTL;
    } else if (extension == ".cjmod") {
        return ImportType::CJMOD;
    }
    
    return ImportType::UNKNOWN;
}

std::string CHTLLoader::findModuleInPath(const std::string& moduleName, const std::string& searchPath) {
    // 查找.cmod文件
    std::string cmodPath = joinPath(searchPath, moduleName + ".cmod");
    if (FileSystem::exists(cmodPath)) {
        return cmodPath;
    }
    
    // 查找.cjmod文件
    std::string cjmodPath = joinPath(searchPath, moduleName + ".cjmod");
    if (FileSystem::exists(cjmodPath)) {
        return cjmodPath;
    }
    
    // 查找目录
    std::string dirPath = joinPath(searchPath, moduleName);
    if (FileSystem::isDirectory(dirPath)) {
        // 查找目录中的主文件
        std::string mainFile = joinPath(dirPath, moduleName + ".chtl");
        if (FileSystem::exists(mainFile)) {
            return mainFile;
        }
    }
    
    return "";
}

bool CHTLLoader::hasCircularDependency(const std::string& filePath, 
                                      std::set<std::string>& visited, 
                                      std::set<std::string>& recursionStack) {
    if (recursionStack.find(filePath) != recursionStack.end()) {
        return true; // 发现循环依赖
    }
    
    if (visited.find(filePath) != visited.end()) {
        return false; // 已经访问过，无循环
    }
    
    visited.insert(filePath);
    recursionStack.insert(filePath);
    
    // 获取文件依赖
    auto it = loaded_files_.find(filePath);
    if (it != loaded_files_.end()) {
        for (const auto& dep : it->second.dependencies) {
            std::string resolvedPath = resolvePath(dep.path);
            if (!resolvedPath.empty() && hasCircularDependency(resolvedPath, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    recursionStack.erase(filePath);
    return false;
}

void CHTLLoader::buildDependencyChain(const std::string& filePath, 
                                     std::vector<std::string>& chain, 
                                     std::set<std::string>& visited) {
    if (visited.find(filePath) != visited.end()) {
        return; // 已经访问过
    }
    
    visited.insert(filePath);
    chain.push_back(filePath);
    
    // 获取文件依赖
    auto it = loaded_files_.find(filePath);
    if (it != loaded_files_.end()) {
        for (const auto& dep : it->second.dependencies) {
            std::string resolvedPath = resolvePath(dep.path);
            if (!resolvedPath.empty()) {
                buildDependencyChain(resolvedPath, chain, visited);
            }
        }
    }
}

bool CHTLLoader::isFileModified(const std::string& filePath) {
    if (!cache_enabled_) {
        return true;
    }
    
    auto it = file_timestamps_.find(filePath);
    if (it == file_timestamps_.end()) {
        return true; // 未缓存，认为已修改
    }
    
    time_t currentTime = FileSystem::getLastModified(filePath);
    return currentTime > it->second;
}

bool CHTLLoader::isModuleModified(const std::string& modulePath) {
    if (!cache_enabled_) {
        return true;
    }
    
    auto it = module_timestamps_.find(modulePath);
    if (it == module_timestamps_.end()) {
        return true; // 未缓存，认为已修改
    }
    
    time_t currentTime = FileSystem::getLastModified(modulePath);
    return currentTime > it->second;
}

void CHTLLoader::updateFileTimestamp(const std::string& filePath) {
    file_timestamps_[filePath] = FileSystem::getLastModified(filePath);
}

void CHTLLoader::updateModuleTimestamp(const std::string& modulePath) {
    module_timestamps_[modulePath] = FileSystem::getLastModified(modulePath);
}

void CHTLLoader::logError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "CHTLLoader Error";
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    ss << ": " << message;
    
    errors_.push_back(ss.str());
}

void CHTLLoader::validatePath(const std::string& path) {
    if (path.empty()) {
        reportError("Empty path provided");
    }
}

void CHTLLoader::validateModule(const std::string& moduleName) {
    if (moduleName.empty()) {
        reportError("Empty module name provided");
    }
}

} // namespace CHTL