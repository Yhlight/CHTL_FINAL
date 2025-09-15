#pragma once

#include "CHTL/CHTLNode.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * 模块类型
 */
enum class ModuleType {
    CMOD,           // CHTL模块
    CJMOD,          // CHTL JS模块
    HYBRID          // 混合模块（CMOD + CJMOD）
};

/**
 * 模块信息
 */
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::string source_path;
    std::string info_path;
    
    ModuleInfo() = default;
    ModuleInfo(const std::string& n, const std::string& v = "1.0.0")
        : name(n), version(v) {}
};

/**
 * 模块导出项
 */
struct ModuleExport {
    std::string name;
    std::string type;        // "template", "custom", "origin", "configuration"
    std::string description;
    std::shared_ptr<CHTLNode> node;
    
    ModuleExport() = default;
    ModuleExport(const std::string& n, const std::string& t, std::shared_ptr<CHTLNode> node)
        : name(n), type(t), node(node) {}
};

/**
 * CMOD模块
 */
class CMODModule {
public:
    CMODModule(const std::string& name);
    ~CMODModule();
    
    // 模块信息
    ModuleInfo& getInfo();
    const ModuleInfo& getInfo() const;
    void setInfo(const ModuleInfo& info);
    
    // 导出项管理
    void addExport(const ModuleExport& exportItem);
    void removeExport(const std::string& name);
    std::vector<ModuleExport> getExports() const;
    std::shared_ptr<CHTLNode> getExport(const std::string& name) const;
    bool hasExport(const std::string& name) const;
    
    // 模板导出
    void addTemplateExport(std::shared_ptr<TemplateNode> templateNode);
    std::vector<std::shared_ptr<TemplateNode>> getTemplateExports() const;
    
    // 自定义导出
    void addCustomExport(std::shared_ptr<CustomNode> customNode);
    std::vector<std::shared_ptr<CustomNode>> getCustomExports() const;
    
    // 原始嵌入导出
    void addOriginExport(const std::string& name, const std::string& content, const std::string& type);
    std::vector<std::pair<std::string, std::string>> getOriginExports(const std::string& type) const;
    
    // 配置导出
    void addConfigurationExport(const std::string& name, const std::string& content);
    std::vector<std::pair<std::string, std::string>> getConfigurationExports() const;
    
    // 文件操作
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool isValid() const;
    
    // 验证
    bool validate() const;
    
    // 清理
    void clear();
    
    // 文件生成
    std::string generateInfoBlock() const;
    std::string generateExportBlock() const;
    std::string generateTemplateBlock() const;
    std::string generateCustomBlock() const;
    std::string generateOriginBlock() const;
    std::string generateConfigurationBlock() const;

private:
    ModuleInfo info_;
    std::vector<ModuleExport> exports_;
    std::vector<std::shared_ptr<TemplateNode>> template_exports_;
    std::vector<std::shared_ptr<CustomNode>> custom_exports_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> origin_exports_;
    std::vector<std::pair<std::string, std::string>> configuration_exports_;
    
    // 文件解析
    bool parseInfoBlock(const std::string& content);
    bool parseExportBlock(const std::string& content);
    bool parseTemplateBlock(const std::string& content);
    bool parseCustomBlock(const std::string& content);
    bool parseOriginBlock(const std::string& content);
    bool parseConfigurationBlock(const std::string& content);
};

/**
 * CJMOD模块
 */
class CJMODModule {
public:
    CJMODModule(const std::string& name);
    ~CJMODModule();
    
    // 模块信息
    ModuleInfo& getInfo();
    const ModuleInfo& getInfo() const;
    void setInfo(const ModuleInfo& info);
    
    // 函数导出
    void addFunctionExport(const std::string& name, const std::string& syntax, const std::string& implementation);
    void removeFunctionExport(const std::string& name);
    std::vector<std::tuple<std::string, std::string, std::string>> getFunctionExports() const;
    bool hasFunctionExport(const std::string& name) const;
    
    // 虚拟对象导出
    void addVirtualObjectExport(const std::string& name, const std::string& implementation);
    void removeVirtualObjectExport(const std::string& name);
    std::vector<std::pair<std::string, std::string>> getVirtualObjectExports() const;
    bool hasVirtualObjectExport(const std::string& name) const;
    
    // 文件操作
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool isValid() const;
    
    // 验证
    bool validate() const;
    
    // 清理
    void clear();
    
    // 文件生成
    std::string generateInfoBlock() const;
    std::string generateFunctionBlock() const;
    std::string generateVirtualObjectBlock() const;

private:
    ModuleInfo info_;
    std::vector<std::tuple<std::string, std::string, std::string>> function_exports_;
    std::vector<std::pair<std::string, std::string>> virtual_object_exports_;
    
    // 文件解析
    bool parseInfoBlock(const std::string& content);
    bool parseFunctionBlock(const std::string& content);
    bool parseVirtualObjectBlock(const std::string& content);
};

/**
 * 混合模块（CMOD + CJMOD）
 */
class HybridModule {
public:
    HybridModule(const std::string& name);
    ~HybridModule();
    
    // 模块信息
    ModuleInfo& getInfo();
    const ModuleInfo& getInfo() const;
    void setInfo(const ModuleInfo& info);
    
    // CMOD部分
    CMODModule& getCMODPart();
    const CMODModule& getCMODPart() const;
    
    // CJMOD部分
    CJMODModule& getCJMODPart();
    const CJMODModule& getCJMODPart() const;
    
    // 文件操作
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool isValid() const;
    
    // 验证
    bool validate() const;
    
    // 清理
    void clear();

private:
    ModuleInfo info_;
    CMODModule cmod_part_;
    CJMODModule cjmod_part_;
    
    // 文件解析
    bool parseHybridFile(const std::string& content);
    bool parseCMODSection(const std::string& content);
    bool parseCJMODSection(const std::string& content);
    
    // 文件生成
    std::string generateHybridFile() const;
};

/**
 * 模块管理器
 */
class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();
    
    // 模块注册
    void registerModule(std::shared_ptr<CMODModule> module);
    void registerModule(std::shared_ptr<CJMODModule> module);
    void registerModule(std::shared_ptr<HybridModule> module);
    
    // 模块查询
    std::shared_ptr<CMODModule> getCMODModule(const std::string& name) const;
    std::shared_ptr<CJMODModule> getCJMODModule(const std::string& name) const;
    std::shared_ptr<HybridModule> getHybridModule(const std::string& name) const;
    
    // 模块加载
    bool loadModule(const std::string& name, const std::string& path);
    bool loadCMODModule(const std::string& name, const std::string& path);
    bool loadCJMODModule(const std::string& name, const std::string& path);
    bool loadHybridModule(const std::string& name, const std::string& path);
    
    // 模块搜索
    std::vector<std::string> findModules(const std::string& pattern) const;
    std::vector<std::string> findCMODModules() const;
    std::vector<std::string> findCJMODModules() const;
    std::vector<std::string> findHybridModules() const;
    
    // 模块路径管理
    void addModulePath(const std::string& path);
    void removeModulePath(const std::string& path);
    std::vector<std::string> getModulePaths() const;
    
    // 依赖解析
    bool resolveDependencies(const std::string& moduleName);
    std::vector<std::string> getDependencies(const std::string& moduleName) const;
    
    // 模块验证
    bool validateModule(const std::string& moduleName) const;
    std::vector<std::string> getModuleErrors(const std::string& moduleName) const;
    
    // 清理
    void clear();
    void unloadModule(const std::string& name);

private:
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> cmod_modules_;
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> cjmod_modules_;
    std::unordered_map<std::string, std::shared_ptr<HybridModule>> hybrid_modules_;
    std::vector<std::string> module_paths_;
    
    // 模块搜索
    std::string findModuleFile(const std::string& name, const std::string& extension) const;
    std::vector<std::string> searchModuleFiles(const std::string& pattern) const;
    
    // 依赖解析
    bool resolveModuleDependencies(const std::string& moduleName, std::unordered_set<std::string>& visited);
    std::vector<std::string> getModuleDependencies(const std::string& moduleName) const;
    
    // 文件系统操作
    bool fileExists(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    std::vector<std::string> listFiles(const std::string& directory) const;
    std::vector<std::string> findFilesByPattern(const std::string& directory, const std::string& pattern) const;
};

/**
 * 模块打包器
 */
class ModulePackager {
public:
    ModulePackager();
    ~ModulePackager();
    
    // 模块打包
    bool packageModule(const std::string& moduleName, const std::string& outputPath);
    bool packageCMODModule(const CMODModule& module, const std::string& outputPath);
    bool packageCJMODModule(const CJMODModule& module, const std::string& outputPath);
    bool packageHybridModule(const HybridModule& module, const std::string& outputPath);
    
    // 模块解包
    bool unpackModule(const std::string& packagePath, const std::string& outputPath);
    bool unpackCMODModule(const std::string& packagePath, const std::string& outputPath);
    bool unpackCJMODModule(const std::string& packagePath, const std::string& outputPath);
    bool unpackHybridModule(const std::string& packagePath, const std::string& outputPath);
    
    // 模块验证
    bool validatePackage(const std::string& packagePath);
    std::vector<std::string> getPackageErrors(const std::string& packagePath) const;
    
    // 模块信息
    ModuleInfo getPackageInfo(const std::string& packagePath) const;
    std::vector<std::string> getPackageContents(const std::string& packagePath) const;

private:
    // 文件操作
    bool createPackage(const std::string& packagePath, const std::string& content);
    std::string readPackage(const std::string& packagePath) const;
    bool extractPackage(const std::string& packagePath, const std::string& outputPath);
    
    // 压缩和解压
    std::string compressData(const std::string& data) const;
    std::string decompressData(const std::string& compressedData) const;
    
    // 元数据
    std::string generatePackageMetadata(const ModuleInfo& info) const;
    ModuleInfo parsePackageMetadata(const std::string& metadata) const;
};

} // namespace CHTL