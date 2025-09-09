#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>
#include <filesystem>

namespace CHTL {

/**
 * @brief 模块类型枚举
 */
enum class ModuleType {
    CMOD,           // CHTL 模块
    CJMOD,          // CHTL JS 模块
    HYBRID          // 混合模块
};

/**
 * @brief 模块信息
 */
class ModuleInfo {
public:
    ModuleInfo(const std::string& name, ModuleType type, const std::string& version = "1.0.0");
    ~ModuleInfo() = default;

    std::string getName() const { return m_name; }
    ModuleType getType() const { return m_type; }
    std::string getVersion() const { return m_version; }
    std::string getDescription() const { return m_description; }
    std::string getAuthor() const { return m_author; }
    std::string getLicense() const { return m_license; }
    
    void setDescription(const std::string& description) { m_description = description; }
    void setAuthor(const std::string& author) { m_author = author; }
    void setLicense(const std::string& license) { m_license = license; }
    
    // 依赖管理
    void addDependency(const std::string& moduleName, const std::string& version = "");
    std::vector<std::pair<std::string, std::string>> getDependencies() const;
    bool hasDependency(const std::string& moduleName) const;
    
    // 元数据管理
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;

private:
    std::string m_name;
    ModuleType m_type;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    std::string m_license;
    std::vector<std::pair<std::string, std::string>> m_dependencies;
    std::unordered_map<std::string, std::string> m_metadata;
};

/**
 * @brief CMOD 模块
 */
class CMODModule {
public:
    explicit CMODModule(std::shared_ptr<CHTLContext> context);
    ~CMODModule() = default;

    // 模块创建
    void createModule(const std::string& name, const std::string& src, const std::string& info);
    void createModuleFromFile(const std::string& filePath);
    
    // 模块解析
    std::shared_ptr<ModuleInfo> parseModuleInfo(const std::string& info) const;
    std::string parseModuleSource(const std::string& src) const;
    
    // 模块导出
    std::string exportModule(const std::string& name) const;
    std::string exportModuleToFile(const std::string& name, const std::string& filePath) const;
    
    // 模块管理
    void addModule(const std::string& name, std::shared_ptr<CMODModule> module);
    std::shared_ptr<CMODModule> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 模块验证
    bool validateModule(const std::string& name) const;
    bool validateModuleInfo(const std::string& info) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> m_modules;
    
    // 辅助方法
    std::string parseInfoBlock(const std::string& info) const;
    std::string parseExportBlock(const std::string& info) const;
    std::vector<std::string> parseDependencies(const std::string& info) const;
};

/**
 * @brief CJMOD 模块
 */
class CJMODModule {
public:
    explicit CJMODModule(std::shared_ptr<CHTLContext> context);
    ~CJMODModule() = default;

    // 模块创建
    void createModule(const std::string& name, const std::string& src, const std::string& info);
    void createModuleFromFile(const std::string& filePath);
    
    // 模块解析
    std::shared_ptr<ModuleInfo> parseModuleInfo(const std::string& info) const;
    std::string parseModuleSource(const std::string& src) const;
    
    // 模块导出
    std::string exportModule(const std::string& name) const;
    std::string exportModuleToFile(const std::string& name, const std::string& filePath) const;
    
    // 模块管理
    void addModule(const std::string& name, std::shared_ptr<CJMODModule> module);
    std::shared_ptr<CJMODModule> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 模块验证
    bool validateModule(const std::string& name) const;
    bool validateModuleInfo(const std::string& info) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> m_modules;
    
    // 辅助方法
    std::string parseInfoBlock(const std::string& info) const;
    std::vector<std::string> parseDependencies(const std::string& info) const;
};

/**
 * @brief 混合模块
 */
class HybridModule {
public:
    explicit HybridModule(std::shared_ptr<CHTLContext> context);
    ~HybridModule() = default;

    // 模块创建
    void createModule(const std::string& name, const std::string& cmodSrc, const std::string& cjmodSrc, const std::string& info);
    void createModuleFromFiles(const std::string& cmodPath, const std::string& cjmodPath);
    
    // 模块解析
    std::shared_ptr<ModuleInfo> parseModuleInfo(const std::string& info) const;
    std::string parseCMODSource(const std::string& src) const;
    std::string parseCJMODSource(const std::string& src) const;
    
    // 模块导出
    std::string exportModule(const std::string& name) const;
    std::string exportModuleToFiles(const std::string& name, const std::string& cmodPath, const std::string& cjmodPath) const;
    
    // 模块管理
    void addModule(const std::string& name, std::shared_ptr<HybridModule> module);
    std::shared_ptr<HybridModule> getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 模块验证
    bool validateModule(const std::string& name) const;
    bool validateCMODSource(const std::string& src) const;
    bool validateCJMODSource(const std::string& src) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<HybridModule>> m_modules;
    
    // 辅助方法
    std::string parseInfoBlock(const std::string& info) const;
    std::vector<std::string> parseDependencies(const std::string& info) const;
};

/**
 * @brief 模块管理器
 */
class ModuleManager {
public:
    explicit ModuleManager(std::shared_ptr<CHTLContext> context);
    ~ModuleManager() = default;

    // 模块注册
    void registerCMODModule(const std::string& name, std::shared_ptr<CMODModule> module);
    void registerCJMODModule(const std::string& name, std::shared_ptr<CJMODModule> module);
    void registerHybridModule(const std::string& name, std::shared_ptr<HybridModule> module);
    
    // 模块查找
    std::shared_ptr<CMODModule> getCMODModule(const std::string& name) const;
    std::shared_ptr<CJMODModule> getCJMODModule(const std::string& name) const;
    std::shared_ptr<HybridModule> getHybridModule(const std::string& name) const;
    
    // 模块类型检测
    ModuleType getModuleType(const std::string& name) const;
    bool isCMODModule(const std::string& name) const;
    bool isCJMODModule(const std::string& name) const;
    bool isHybridModule(const std::string& name) const;
    
    // 模块路径解析
    std::string resolveModulePath(const std::string& moduleName, const std::string& basePath) const;
    std::vector<std::string> findModules(const std::string& pattern, const std::string& basePath) const;
    
    // 模块依赖解析
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const;
    bool hasCircularDependency(const std::string& moduleName) const;
    
    // 模块验证
    bool validateAllModules() const;
    bool validateModule(const std::string& name) const;
    
    // 模块清理
    void clear();
    void removeModule(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> m_cmodModules;
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> m_cjmodModules;
    std::unordered_map<std::string, std::shared_ptr<HybridModule>> m_hybridModules;
    
    // 辅助方法
    std::string getModuleExtension(ModuleType type) const;
    bool isModuleFile(const std::string& filePath) const;
    std::vector<std::string> parseModuleDependencies(const std::string& moduleName) const;
};

/**
 * @brief 模块解析器
 */
class ModuleParser {
public:
    explicit ModuleParser(std::shared_ptr<CHTLContext> context);
    ~ModuleParser() = default;

    // 解析模块
    std::shared_ptr<CMODModule> parseCMODModule(const std::string& content) const;
    std::shared_ptr<CJMODModule> parseCJMODModule(const std::string& content) const;
    std::shared_ptr<HybridModule> parseHybridModule(const std::string& cmodContent, const std::string& cjmodContent) const;
    
    // 解析模块信息
    std::shared_ptr<ModuleInfo> parseModuleInfo(const std::string& infoBlock) const;
    std::string parseModuleSource(const std::string& srcBlock) const;
    
    // 解析模块文件
    std::shared_ptr<CMODModule> parseCMODFile(const std::string& filePath) const;
    std::shared_ptr<CJMODModule> parseCJMODFile(const std::string& filePath) const;
    std::shared_ptr<HybridModule> parseHybridFiles(const std::string& cmodPath, const std::string& cjmodPath) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string extractInfoBlock(const std::string& content) const;
    std::string extractSourceBlock(const std::string& content) const;
    std::string extractExportBlock(const std::string& content) const;
    std::vector<std::string> parseDependencyList(const std::string& depsStr) const;
};

/**
 * @brief 模块生成器
 */
class ModuleGenerator {
public:
    explicit ModuleGenerator(std::shared_ptr<CHTLContext> context);
    ~ModuleGenerator() = default;

    // 生成模块
    std::string generateCMODModule(std::shared_ptr<CMODModule> module) const;
    std::string generateCJMODModule(std::shared_ptr<CJMODModule> module) const;
    std::string generateHybridModule(std::shared_ptr<HybridModule> module) const;
    
    // 生成模块文件
    void generateCMODFile(std::shared_ptr<CMODModule> module, const std::string& filePath) const;
    void generateCJMODFile(std::shared_ptr<CJMODModule> module, const std::string& filePath) const;
    void generateHybridFiles(std::shared_ptr<HybridModule> module, const std::string& cmodPath, const std::string& cjmodPath) const;
    
    // 生成模块信息
    std::string generateModuleInfo(std::shared_ptr<ModuleInfo> info) const;
    std::string generateDependencies(const std::vector<std::pair<std::string, std::string>>& deps) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string generateInfoBlock(std::shared_ptr<ModuleInfo> info) const;
    std::string generateExportBlock(const std::vector<std::string>& exports) const;
    std::string generateSourceBlock(const std::string& source) const;
};

} // namespace CHTL