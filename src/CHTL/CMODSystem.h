#ifndef CMOD_SYSTEM_H
#define CMOD_SYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CMOD模块信息
 */
struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
};

/**
 * CMOD模块
 */
class CMODModule {
public:
    CMODInfo info;
    std::map<std::string, std::string> exports;
    std::vector<std::string> sourceFiles;
    
    CMODModule(const CMODInfo& moduleInfo) : info(moduleInfo) {}
    
    /**
     * 添加导出
     * @param name 导出名
     * @param value 导出值
     */
    void addExport(const std::string& name, const std::string& value);
    
    /**
     * 添加源文件
     * @param filePath 文件路径
     */
    void addSourceFile(const std::string& filePath);
    
    /**
     * 验证模块
     * @return 是否有效
     */
    bool validate() const;
};

/**
 * CMOD系统管理器
 */
class CMODSystem {
private:
    std::map<std::string, std::unique_ptr<CMODModule>> modules;

public:
    CMODSystem();
    ~CMODSystem();
    
    /**
     * 注册CMOD模块
     * @param module 模块
     */
    void registerModule(std::unique_ptr<CMODModule> module);
    
    /**
     * 获取CMOD模块
     * @param name 模块名
     * @return 模块指针
     */
    CMODModule* getModule(const std::string& name);
    
    /**
     * 获取所有模块
     * @return 模块映射
     */
    const std::map<std::string, std::unique_ptr<CMODModule>>& getAllModules() const;
    
    /**
     * 处理CMOD导入
     * @param moduleName 模块名
     * @return 导入结果
     */
    std::string processImport(const std::string& moduleName);
    
    /**
     * 验证CMOD模块
     * @param moduleName 模块名
     * @return 是否有效
     */
    bool validateModule(const std::string& moduleName);
    
    /**
     * 清空所有模块
     */
    void clear();

private:
    /**
     * 解析模块信息
     * @param info 信息字符串
     * @return 模块信息
     */
    CMODInfo parseModuleInfo(const std::string& info);
    
    /**
     * 处理模块依赖
     * @param moduleName 模块名
     * @return 依赖列表
     */
    std::vector<std::string> resolveDependencies(const std::string& moduleName);
    
    /**
     * 检测循环依赖
     * @param moduleName 模块名
     * @return 是否有循环依赖
     */
    bool detectCircularDependency(const std::string& moduleName);
    
    /**
     * 检查版本兼容性
     * @param moduleName 模块名
     * @return 是否兼容
     */
    bool checkVersionCompatibility(const std::string& moduleName);
};

} // namespace CHTL

#endif // CMOD_SYSTEM_H