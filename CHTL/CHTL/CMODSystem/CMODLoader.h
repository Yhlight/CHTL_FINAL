#pragma once

#include "CMODModule.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <functional>
#include <future>
#include <mutex>

namespace CHTL {

// 加载策略枚举
enum class LoadStrategy {
    SYNC = 0,       // 同步加载
    ASYNC,          // 异步加载
    LAZY,           // 延迟加载
    EAGER,          // 急切加载
    PARALLEL,       // 并行加载
    SEQUENTIAL      // 顺序加载
};

// 加载选项
struct LoadOptions {
    LoadStrategy strategy = LoadStrategy::SYNC;
    bool recursive = true;          // 递归加载依赖
    bool validate = true;           // 验证模块
    bool cache = true;              // 缓存模块
    int timeout = 30000;            // 超时时间(ms)
    int maxDepth = -1;              // 最大加载深度
    bool ignoreErrors = false;      // 忽略错误
    std::vector<std::string> searchPaths; // 搜索路径
    std::map<std::string, std::any> metadata; // 元数据
};

// 加载结果
struct LoadResult {
    bool success;
    std::shared_ptr<CMODModule> module;
    std::string error;
    std::vector<std::string> warnings;
    std::chrono::milliseconds loadTime;
    
    LoadResult() : success(false), loadTime(0) {}
    LoadResult(bool success, std::shared_ptr<CMODModule> module = nullptr, const std::string& error = "")
        : success(success), module(module), error(error), loadTime(0) {}
};

// 模块缓存
class ModuleCache {
public:
    ModuleCache();
    ~ModuleCache();
    
    // 缓存管理
    void put(const std::string& key, std::shared_ptr<CMODModule> module);
    std::shared_ptr<CMODModule> get(const std::string& key) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void clear();
    
    // 缓存统计
    size_t size() const;
    std::vector<std::string> keys() const;
    std::vector<std::shared_ptr<CMODModule>> values() const;
    
    // 缓存控制
    void setMaxSize(size_t maxSize);
    size_t getMaxSize() const;
    void setTTL(std::chrono::milliseconds ttl);
    std::chrono::milliseconds getTTL() const;
    
    // 缓存验证
    bool isValid(const std::string& key) const;
    void validate();
    
private:
    struct CacheEntry {
        std::shared_ptr<CMODModule> module;
        std::chrono::steady_clock::time_point timestamp;
        std::chrono::milliseconds ttl;
        
        CacheEntry(std::shared_ptr<CMODModule> module, std::chrono::milliseconds ttl)
            : module(module), timestamp(std::chrono::steady_clock::now()), ttl(ttl) {}
        
        bool isExpired() const {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
            return elapsed >= ttl;
        }
    };
    
    std::map<std::string, CacheEntry> cache;
    size_t maxSize;
    std::chrono::milliseconds defaultTTL;
    mutable std::mutex mutex;
    
    void evictLRU();
    void evictExpired();
};

// CMOD加载器
class CMODLoader {
public:
    CMODLoader();
    ~CMODLoader();
    
    // 加载器注册
    void registerLoader(std::shared_ptr<IModuleLoader> loader);
    void unregisterLoader(std::shared_ptr<IModuleLoader> loader);
    void clearLoaders();
    std::vector<std::shared_ptr<IModuleLoader>> getLoaders() const;
    
    // 搜索路径管理
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    void clearSearchPaths();
    std::vector<std::string> getSearchPaths() const;
    
    // 模块加载
    LoadResult load(const std::string& path, const LoadOptions& options = LoadOptions{});
    LoadResult load(const std::string& path, LoadStrategy strategy = LoadStrategy::SYNC);
    std::future<LoadResult> loadAsync(const std::string& path, const LoadOptions& options = LoadOptions{});
    
    // 批量加载
    std::vector<LoadResult> loadAll(const std::vector<std::string>& paths, const LoadOptions& options = LoadOptions{});
    std::future<std::vector<LoadResult>> loadAllAsync(const std::vector<std::string>& paths, const LoadOptions& options = LoadOptions{});
    
    // 模块管理
    void unload(const std::string& path);
    void unload(std::shared_ptr<CMODModule> module);
    void unloadAll();
    
    std::shared_ptr<CMODModule> getModule(const std::string& path) const;
    std::vector<std::shared_ptr<CMODModule>> getAllModules() const;
    std::vector<std::shared_ptr<CMODModule>> getModulesByType(ModuleType type) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(std::shared_ptr<CMODModule> module) const;
    std::vector<std::string> resolveDependencies(const std::string& path) const;
    bool checkDependencies(std::shared_ptr<CMODModule> module) const;
    
    // 路径解析
    std::string resolvePath(const std::string& path) const;
    std::string resolveModulePath(const std::string& name, const std::string& version = "") const;
    std::vector<std::string> findModules(const std::string& pattern) const;
    
    // 缓存管理
    void setCache(std::shared_ptr<ModuleCache> cache);
    std::shared_ptr<ModuleCache> getCache() const;
    void clearCache();
    
    // 状态管理
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setDebug(bool debug);
    bool isDebug() const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> validate() const;
    
    // 统计
    size_t getLoadedModuleCount() const;
    size_t getCacheSize() const;
    std::chrono::milliseconds getTotalLoadTime() const;
    std::chrono::milliseconds getAverageLoadTime() const;
    
    // 重置
    void reset();
    
    // 转换
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
private:
    std::vector<std::shared_ptr<IModuleLoader>> loaders;
    std::vector<std::string> searchPaths;
    std::map<std::string, std::shared_ptr<CMODModule>> loadedModules;
    std::shared_ptr<ModuleCache> cache;
    
    bool enabled;
    bool debug;
    
    int totalLoadCount;
    std::chrono::milliseconds totalLoadTime;
    
    mutable std::mutex mutex;
    
    // 辅助方法
    bool validateLoader() const;
    bool validateSearchPaths() const;
    bool validateModules() const;
    
    std::shared_ptr<IModuleLoader> findLoader(const std::string& path) const;
    std::string findModuleFile(const std::string& name, const std::string& version = "") const;
    std::string findModuleFile(const std::string& path) const;
    
    LoadResult loadModule(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleSync(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleAsync(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleLazy(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleEager(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleParallel(const std::string& path, const LoadOptions& options);
    LoadResult loadModuleSequential(const std::string& path, const LoadOptions& options);
    
    void loadDependencies(std::shared_ptr<CMODModule> module, const LoadOptions& options);
    void loadDependenciesRecursive(std::shared_ptr<CMODModule> module, const LoadOptions& options, int depth);
    
    std::string generateJSON() const;
    std::string generateXML() const;
    std::string generateYAML() const;
    
    std::string formatLoader() const;
    std::string minifyLoader() const;
    std::string beautifyLoader() const;
    
    void updateStats(std::chrono::milliseconds loadTime);
};

// 默认模块加载器
class DefaultCMODLoader : public IModuleLoader {
public:
    DefaultCMODLoader();
    ~DefaultCMODLoader();
    
    bool canLoad(const std::string& path) override;
    std::shared_ptr<CMODModule> load(const std::string& path) override;
    std::vector<std::string> getSupportedExtensions() override;
    
private:
    std::vector<std::string> supportedExtensions;
    std::shared_ptr<CMODModule> loadFromFile(const std::string& path);
    std::shared_ptr<CMODModule> loadFromJSON(const std::string& content);
    std::shared_ptr<CMODModule> loadFromXML(const std::string& content);
    std::shared_ptr<CMODModule> loadFromYAML(const std::string& content);
};

} // namespace CHTL