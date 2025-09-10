#include "CMODLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <thread>

namespace CHTL {

// ModuleCache 实现
ModuleCache::ModuleCache() : maxSize(1000), defaultTTL(std::chrono::minutes(30)) {
}

ModuleCache::~ModuleCache() = default;

void ModuleCache::put(const std::string& key, std::shared_ptr<CMODModule> module) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (cache.size() >= maxSize) {
        evictLRU();
    }
    
    cache[key] = CacheEntry(module, defaultTTL);
}

std::shared_ptr<CMODModule> ModuleCache::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        if (it->second.isExpired()) {
            // 在const方法中不能修改cache，所以这里不能erase
            return nullptr;
        }
        return it->second.module;
    }
    
    return nullptr;
}

bool ModuleCache::has(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        if (it->second.isExpired()) {
            cache.erase(it);
            return false;
        }
        return true;
    }
    
    return false;
}

void ModuleCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    cache.erase(key);
}

void ModuleCache::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    cache.clear();
}

size_t ModuleCache::size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return cache.size();
}

std::vector<std::string> ModuleCache::keys() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> keys;
    for (const auto& pair : cache) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::shared_ptr<CMODModule>> ModuleCache::values() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> values;
    for (const auto& pair : cache) {
        values.push_back(pair.second.module);
    }
    return values;
}

void ModuleCache::setMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex);
    this->maxSize = maxSize;
    while (cache.size() > maxSize) {
        evictLRU();
    }
}

size_t ModuleCache::getMaxSize() const {
    return maxSize;
}

void ModuleCache::setTTL(std::chrono::milliseconds ttl) {
    std::lock_guard<std::mutex> lock(mutex);
    defaultTTL = ttl;
}

std::chrono::milliseconds ModuleCache::getTTL() const {
    return defaultTTL;
}

bool ModuleCache::isValid(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        return !it->second.isExpired();
    }
    
    return false;
}

void ModuleCache::validate() {
    std::lock_guard<std::mutex> lock(mutex);
    evictExpired();
}

void ModuleCache::evictLRU() {
    if (cache.empty()) return;
    
    auto oldest = cache.begin();
    for (auto it = cache.begin(); it != cache.end(); ++it) {
        if (it->second.timestamp < oldest->second.timestamp) {
            oldest = it;
        }
    }
    cache.erase(oldest);
}

void ModuleCache::evictExpired() {
    for (auto it = cache.begin(); it != cache.end();) {
        if (it->second.isExpired()) {
            it = cache.erase(it);
        } else {
            ++it;
        }
    }
}

// CMODLoader 实现
CMODLoader::CMODLoader() : enabled(true), debug(false), totalLoadCount(0), totalLoadTime(0) {
    cache = std::make_shared<ModuleCache>();
}

CMODLoader::~CMODLoader() = default;

// 加载器注册实现
void CMODLoader::registerLoader(std::shared_ptr<IModuleLoader> loader) {
    if (loader) {
        std::lock_guard<std::mutex> lock(mutex);
        loaders.push_back(loader);
    }
}

void CMODLoader::unregisterLoader(std::shared_ptr<IModuleLoader> loader) {
    if (loader) {
        std::lock_guard<std::mutex> lock(mutex);
        loaders.erase(std::remove(loaders.begin(), loaders.end(), loader), loaders.end());
    }
}

void CMODLoader::clearLoaders() {
    std::lock_guard<std::mutex> lock(mutex);
    loaders.clear();
}

std::vector<std::shared_ptr<IModuleLoader>> CMODLoader::getLoaders() const {
    std::lock_guard<std::mutex> lock(mutex);
    return loaders;
}

// 搜索路径管理实现
void CMODLoader::addSearchPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
    }
}

void CMODLoader::removeSearchPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);
    searchPaths.erase(std::remove(searchPaths.begin(), searchPaths.end(), path), searchPaths.end());
}

void CMODLoader::clearSearchPaths() {
    std::lock_guard<std::mutex> lock(mutex);
    searchPaths.clear();
}

std::vector<std::string> CMODLoader::getSearchPaths() const {
    std::lock_guard<std::mutex> lock(mutex);
    return searchPaths;
}

// 模块加载实现
LoadResult CMODLoader::load(const std::string& path, const LoadOptions& options) {
    if (!enabled) {
        return LoadResult(false, nullptr, "Loader is disabled");
    }
    
    // 检查缓存
    if (options.cache && cache) {
        auto cached = cache->get(path);
        if (cached) {
            return LoadResult(true, cached);
        }
    }
    
    return loadModule(path, options);
}

LoadResult CMODLoader::load(const std::string& path, LoadStrategy strategy) {
    LoadOptions options;
    options.strategy = strategy;
    return load(path, options);
}

std::future<LoadResult> CMODLoader::loadAsync(const std::string& path, const LoadOptions& options) {
    return std::async(std::launch::async, [this, path, options]() {
        return load(path, options);
    });
}

// 批量加载实现
std::vector<LoadResult> CMODLoader::loadAll(const std::vector<std::string>& paths, const LoadOptions& options) {
    std::vector<LoadResult> results;
    
    if (options.strategy == LoadStrategy::PARALLEL) {
        std::vector<std::future<LoadResult>> futures;
        for (const auto& path : paths) {
            futures.push_back(loadAsync(path, options));
        }
        
        for (auto& future : futures) {
            results.push_back(future.get());
        }
    } else {
        for (const auto& path : paths) {
            results.push_back(load(path, options));
        }
    }
    
    return results;
}

std::future<std::vector<LoadResult>> CMODLoader::loadAllAsync(const std::vector<std::string>& paths, const LoadOptions& options) {
    return std::async(std::launch::async, [this, paths, options]() {
        return loadAll(paths, options);
    });
}

// 模块管理实现
void CMODLoader::unload(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);
    loadedModules.erase(path);
    
    if (cache) {
        cache->remove(path);
    }
}

void CMODLoader::unload(std::shared_ptr<CMODModule> module) {
    if (module) {
        unload(module->getPath());
    }
}

void CMODLoader::unloadAll() {
    std::lock_guard<std::mutex> lock(mutex);
    loadedModules.clear();
    
    if (cache) {
        cache->clear();
    }
}

std::shared_ptr<CMODModule> CMODLoader::getModule(const std::string& path) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = loadedModules.find(path);
    return it != loadedModules.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<CMODModule>> CMODLoader::getAllModules() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> modules;
    for (const auto& pair : loadedModules) {
        modules.push_back(pair.second);
    }
    return modules;
}

std::vector<std::shared_ptr<CMODModule>> CMODLoader::getModulesByType(ModuleType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<CMODModule>> modules;
    for (const auto& pair : loadedModules) {
        if (pair.second && pair.second->getType() == type) {
            modules.push_back(pair.second);
        }
    }
    return modules;
}

// 依赖解析实现
std::vector<std::string> CMODLoader::resolveDependencies(std::shared_ptr<CMODModule> module) const {
    std::vector<std::string> dependencies;
    
    if (!module) return dependencies;
    
    for (const auto& dep : module->getDependencies()) {
        if (dep.required) {
            std::string path = resolveModulePath(dep.name, dep.version);
            if (!path.empty()) {
                dependencies.push_back(path);
            }
        }
    }
    
    return dependencies;
}

std::vector<std::string> CMODLoader::resolveDependencies(const std::string& path) const {
    auto module = getModule(path);
    return resolveDependencies(module);
}

bool CMODLoader::checkDependencies(std::shared_ptr<CMODModule> module) const {
    if (!module) return false;
    
    for (const auto& dep : module->getRequiredDependencies()) {
        std::string path = resolveModulePath(dep.name, dep.version);
        if (path.empty() || !getModule(path)) {
            return false;
        }
    }
    
    return true;
}

// 路径解析实现
std::string CMODLoader::resolvePath(const std::string& path) const {
    if (std::filesystem::exists(path)) {
        return std::filesystem::absolute(path).string();
    }
    
    for (const auto& searchPath : searchPaths) {
        std::string fullPath = searchPath + "/" + path;
        if (std::filesystem::exists(fullPath)) {
            return std::filesystem::absolute(fullPath).string();
        }
    }
    
    return "";
}

std::string CMODLoader::resolveModulePath(const std::string& name, const std::string& version) const {
    // 简化的模块路径解析
    for (const auto& searchPath : searchPaths) {
        std::string modulePath = searchPath + "/" + name;
        if (std::filesystem::exists(modulePath)) {
            return modulePath;
        }
    }
    
    return "";
}

std::vector<std::string> CMODLoader::findModules(const std::string& pattern) const {
    std::vector<std::string> modules;
    
    for (const auto& searchPath : searchPaths) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                if (entry.is_directory()) {
                    std::string name = entry.path().filename().string();
                    if (pattern.empty() || name.find(pattern) != std::string::npos) {
                        modules.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error&) {
            // 忽略文件系统错误
        }
    }
    
    return modules;
}

// 缓存管理实现
void CMODLoader::setCache(std::shared_ptr<ModuleCache> cache) {
    std::lock_guard<std::mutex> lock(mutex);
    this->cache = cache;
}

std::shared_ptr<ModuleCache> CMODLoader::getCache() const {
    std::lock_guard<std::mutex> lock(mutex);
    return cache;
}

void CMODLoader::clearCache() {
    if (cache) {
        cache->clear();
    }
}

// 状态管理实现
void CMODLoader::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    this->enabled = enabled;
}

bool CMODLoader::isEnabled() const {
    std::lock_guard<std::mutex> lock(mutex);
    return enabled;
}

void CMODLoader::setDebug(bool debug) {
    std::lock_guard<std::mutex> lock(mutex);
    this->debug = debug;
}

bool CMODLoader::isDebug() const {
    std::lock_guard<std::mutex> lock(mutex);
    return debug;
}

// 验证实现
bool CMODLoader::isValid() const {
    return validateLoader() && validateSearchPaths() && validateModules();
}

std::vector<std::string> CMODLoader::validate() const {
    std::vector<std::string> errors;
    
    if (!validateLoader()) {
        errors.push_back("Loader validation failed");
    }
    
    if (!validateSearchPaths()) {
        errors.push_back("Search paths validation failed");
    }
    
    if (!validateModules()) {
        errors.push_back("Modules validation failed");
    }
    
    return errors;
}

// 统计实现
size_t CMODLoader::getLoadedModuleCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return loadedModules.size();
}

size_t CMODLoader::getCacheSize() const {
    return cache ? cache->size() : 0;
}

std::chrono::milliseconds CMODLoader::getTotalLoadTime() const {
    std::lock_guard<std::mutex> lock(mutex);
    return totalLoadTime;
}

std::chrono::milliseconds CMODLoader::getAverageLoadTime() const {
    std::lock_guard<std::mutex> lock(mutex);
    return totalLoadCount > 0 ? 
        std::chrono::milliseconds(totalLoadTime.count() / totalLoadCount) : 
        std::chrono::milliseconds(0);
}

// 重置实现
void CMODLoader::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    loaders.clear();
    searchPaths.clear();
    loadedModules.clear();
    if (cache) {
        cache->clear();
    }
    enabled = true;
    debug = false;
    totalLoadCount = 0;
    totalLoadTime = std::chrono::milliseconds(0);
}

// 转换实现
std::string CMODLoader::toJSON() const {
    return generateJSON();
}

std::string CMODLoader::toXML() const {
    return generateXML();
}

std::string CMODLoader::toYAML() const {
    return generateYAML();
}

std::string CMODLoader::toString() const {
    return "CMODLoader";
}

std::string CMODLoader::toDebugString() const {
    std::ostringstream oss;
    oss << "CMODLoader{enabled=" << enabled
        << ", debug=" << debug
        << ", loadedModuleCount=" << getLoadedModuleCount()
        << ", cacheSize=" << getCacheSize()
        << ", totalLoadCount=" << totalLoadCount << "}";
    return oss.str();
}

// 格式化实现
std::string CMODLoader::format() const {
    return formatLoader();
}

std::string CMODLoader::minify() const {
    return minifyLoader();
}

std::string CMODLoader::beautify() const {
    return beautifyLoader();
}

// 辅助方法实现
bool CMODLoader::validateLoader() const {
    return !loaders.empty();
}

bool CMODLoader::validateSearchPaths() const {
    for (const auto& path : searchPaths) {
        if (!std::filesystem::exists(path)) {
            return false;
        }
    }
    return true;
}

bool CMODLoader::validateModules() const {
    for (const auto& pair : loadedModules) {
        if (!pair.second || !pair.second->isValid()) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<IModuleLoader> CMODLoader::findLoader(const std::string& path) const {
    for (const auto& loader : loaders) {
        if (loader && loader->canLoad(path)) {
            return loader;
        }
    }
    return nullptr;
}

std::string CMODLoader::findModuleFile(const std::string& name, const std::string& version) const {
    return resolveModulePath(name, version);
}

std::string CMODLoader::findModuleFile(const std::string& path) const {
    return resolvePath(path);
}

LoadResult CMODLoader::loadModule(const std::string& path, const LoadOptions& options) {
    switch (options.strategy) {
        case LoadStrategy::SYNC:
            return loadModuleSync(path, options);
        case LoadStrategy::ASYNC:
            return loadModuleAsync(path, options);
        case LoadStrategy::LAZY:
            return loadModuleLazy(path, options);
        case LoadStrategy::EAGER:
            return loadModuleEager(path, options);
        case LoadStrategy::PARALLEL:
            return loadModuleParallel(path, options);
        case LoadStrategy::SEQUENTIAL:
            return loadModuleSequential(path, options);
        default:
            return LoadResult(false, nullptr, "Unknown load strategy");
    }
}

LoadResult CMODLoader::loadModuleSync(const std::string& path, const LoadOptions& options) {
    auto start = std::chrono::steady_clock::now();
    
    std::string resolvedPath = findModuleFile(path, "");
    if (resolvedPath.empty()) {
        return LoadResult(false, nullptr, "Module not found: " + path);
    }
    
    auto loader = findLoader(resolvedPath);
    if (!loader) {
        return LoadResult(false, nullptr, "No loader found for: " + resolvedPath);
    }
    
    auto module = loader->load(resolvedPath);
    if (!module) {
        return LoadResult(false, nullptr, "Failed to load module: " + resolvedPath);
    }
    
    module->setPath(resolvedPath);
    
    if (options.validate && !module->isValid()) {
        return LoadResult(false, module, "Module validation failed");
    }
    
    if (options.recursive) {
        loadDependencies(module, options);
    }
    
    {
        std::lock_guard<std::mutex> lock(mutex);
        loadedModules[resolvedPath] = module;
    }
    
    if (options.cache && cache) {
        cache->put(resolvedPath, module);
    }
    
    auto end = std::chrono::steady_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    updateStats(loadTime);
    
    return LoadResult(true, module);
}

LoadResult CMODLoader::loadModuleAsync(const std::string& path, const LoadOptions& options) {
    // 简化的异步加载实现
    return loadModuleSync(path, options);
}

LoadResult CMODLoader::loadModuleLazy(const std::string& path, const LoadOptions& options) {
    // 简化的延迟加载实现
    return loadModuleSync(path, options);
}

LoadResult CMODLoader::loadModuleEager(const std::string& path, const LoadOptions& options) {
    // 简化的急切加载实现
    return loadModuleSync(path, options);
}

LoadResult CMODLoader::loadModuleParallel(const std::string& path, const LoadOptions& options) {
    // 简化的并行加载实现
    return loadModuleSync(path, options);
}

LoadResult CMODLoader::loadModuleSequential(const std::string& path, const LoadOptions& options) {
    // 简化的顺序加载实现
    return loadModuleSync(path, options);
}

void CMODLoader::loadDependencies(std::shared_ptr<CMODModule> module, const LoadOptions& options) {
    loadDependenciesRecursive(module, options, 0);
}

void CMODLoader::loadDependenciesRecursive(std::shared_ptr<CMODModule> module, const LoadOptions& options, int depth) {
    if (options.maxDepth > 0 && depth >= options.maxDepth) {
        return;
    }
    
    for (const auto& dep : module->getRequiredDependencies()) {
        std::string depPath = resolveModulePath(dep.name, dep.version);
        if (!depPath.empty() && !getModule(depPath)) {
            auto result = load(depPath, options);
            if (result.success && result.module) {
                loadDependenciesRecursive(result.module, options, depth + 1);
            }
        }
    }
}

std::string CMODLoader::generateJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"enabled\": " << (enabled ? "true" : "false") << ",\n";
    oss << "  \"debug\": " << (debug ? "true" : "false") << ",\n";
    oss << "  \"loadedModuleCount\": " << getLoadedModuleCount() << ",\n";
    oss << "  \"cacheSize\": " << getCacheSize() << ",\n";
    oss << "  \"totalLoadCount\": " << totalLoadCount << "\n";
    oss << "}";
    return oss.str();
}

std::string CMODLoader::generateXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<loader>\n";
    oss << "  <enabled>" << (enabled ? "true" : "false") << "</enabled>\n";
    oss << "  <debug>" << (debug ? "true" : "false") << "</debug>\n";
    oss << "  <loadedModuleCount>" << getLoadedModuleCount() << "</loadedModuleCount>\n";
    oss << "  <cacheSize>" << getCacheSize() << "</cacheSize>\n";
    oss << "  <totalLoadCount>" << totalLoadCount << "</totalLoadCount>\n";
    oss << "</loader>";
    return oss.str();
}

std::string CMODLoader::generateYAML() const {
    std::ostringstream oss;
    oss << "enabled: " << (enabled ? "true" : "false") << "\n";
    oss << "debug: " << (debug ? "true" : "false") << "\n";
    oss << "loadedModuleCount: " << getLoadedModuleCount() << "\n";
    oss << "cacheSize: " << getCacheSize() << "\n";
    oss << "totalLoadCount: " << totalLoadCount << "\n";
    return oss.str();
}

std::string CMODLoader::formatLoader() const {
    return toString();
}

std::string CMODLoader::minifyLoader() const {
    return toString();
}

std::string CMODLoader::beautifyLoader() const {
    return toString();
}

void CMODLoader::updateStats(std::chrono::milliseconds loadTime) {
    totalLoadCount++;
    totalLoadTime += loadTime;
}

// DefaultCMODLoader 实现
DefaultCMODLoader::DefaultCMODLoader() {
    supportedExtensions = {".chtl", ".json", ".xml", ".yaml", ".yml"};
}

DefaultCMODLoader::~DefaultCMODLoader() = default;

bool DefaultCMODLoader::canLoad(const std::string& path) {
    std::filesystem::path filePath(path);
    std::string extension = filePath.extension().string();
    
    return std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
}

std::shared_ptr<CMODModule> DefaultCMODLoader::load(const std::string& path) {
    if (!canLoad(path)) {
        return nullptr;
    }
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    std::filesystem::path filePath(path);
    std::string extension = filePath.extension().string();
    
    if (extension == ".json") {
        return loadFromJSON(content);
    } else if (extension == ".xml") {
        return loadFromXML(content);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(content);
    } else {
        return loadFromFile(path);
    }
}

std::vector<std::string> DefaultCMODLoader::getSupportedExtensions() {
    return supportedExtensions;
}

std::shared_ptr<CMODModule> DefaultCMODLoader::loadFromFile(const std::string& path) {
    // 简化的文件加载实现
    auto module = std::make_shared<CMODModule>();
    module->setPath(path);
    module->setName(std::filesystem::path(path).stem().string());
    module->setVersion("1.0.0");
    module->setType(ModuleType::CHTL);
    return module;
}

std::shared_ptr<CMODModule> DefaultCMODLoader::loadFromJSON(const std::string& content) {
    // 简化的JSON加载实现
    auto module = std::make_shared<CMODModule>();
    module->setName("json-module");
    module->setVersion("1.0.0");
    module->setType(ModuleType::CHTL);
    return module;
}

std::shared_ptr<CMODModule> DefaultCMODLoader::loadFromXML(const std::string& content) {
    // 简化的XML加载实现
    auto module = std::make_shared<CMODModule>();
    module->setName("xml-module");
    module->setVersion("1.0.0");
    module->setType(ModuleType::CHTL);
    return module;
}

std::shared_ptr<CMODModule> DefaultCMODLoader::loadFromYAML(const std::string& content) {
    // 简化的YAML加载实现
    auto module = std::make_shared<CMODModule>();
    module->setName("yaml-module");
    module->setVersion("1.0.0");
    module->setType(ModuleType::CHTL);
    return module;
}

} // namespace CHTL