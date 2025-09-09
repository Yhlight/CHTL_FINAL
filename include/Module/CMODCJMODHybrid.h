#ifndef CMOD_CJMOD_HYBRID_H
#define CMOD_CJMOD_HYBRID_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * CMOD + CJMOD 混合模块系统
 * 支持 src + info 结构，允许同时提供组件和 CHTL JS 语法扩展
 */
class CMODCJMODHybrid {
public:
    /**
     * 构造函数
     * @param moduleName 模块名称
     */
    explicit CMODCJMODHybrid(const std::string& moduleName);
    
    /**
     * 析构函数
     */
    ~CMODCJMODHybrid() = default;
    
    /**
     * 设置模块信息
     * @param name 模块名称
     * @param version 版本号
     * @param description 描述
     * @param author 作者
     * @param license 许可证
     * @param dependencies 依赖
     * @param category 分类
     * @param minCHTLVersion 最小 CHTL 版本
     * @param maxCHTLVersion 最大 CHTL 版本
     */
    void setModuleInfo(const std::string& name,
                      const std::string& version,
                      const std::string& description,
                      const std::string& author,
                      const std::string& license,
                      const std::string& dependencies,
                      const std::string& category,
                      const std::string& minCHTLVersion,
                      const std::string& maxCHTLVersion);
    
    /**
     * 添加 CMOD 子模块
     * @param submoduleName 子模块名称
     * @param srcFiles 源文件列表
     * @param infoFile 信息文件
     */
    void addCMODSubmodule(const std::string& submoduleName,
                         const std::vector<std::string>& srcFiles,
                         const std::string& infoFile);
    
    /**
     * 添加 CJMOD 子模块
     * @param submoduleName 子模块名称
     * @param srcFiles 源文件列表
     * @param infoFile 信息文件
     */
    void addCJMODSubmodule(const std::string& submoduleName,
                          const std::vector<std::string>& srcFiles,
                          const std::string& infoFile);
    
    /**
     * 设置主模块文件
     * @param cmodFile CMOD 主文件
     * @param cjmodFile CJMOD 主文件
     */
    void setMainFiles(const std::string& cmodFile, const std::string& cjmodFile);
    
    /**
     * 生成模块结构
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool generateModuleStructure(const std::string& outputPath);
    
    /**
     * 生成 CMOD 信息文件
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool generateCMODInfoFile(const std::string& outputPath);
    
    /**
     * 生成 CJMOD 信息文件
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool generateCJMODInfoFile(const std::string& outputPath);
    
    /**
     * 验证模块结构
     * @return 是否有效
     */
    bool validate() const;
    
    /**
     * 获取模块名称
     * @return 模块名称
     */
    const std::string& getModuleName() const { return m_moduleName; }
    
    /**
     * 获取 CMOD 子模块列表
     * @return CMOD 子模块列表
     */
    const std::vector<std::string>& getCMODSubmodules() const { return m_cmodSubmodules; }
    
    /**
     * 获取 CJMOD 子模块列表
     * @return CJMOD 子模块列表
     */
    const std::vector<std::string>& getCJMODSubmodules() const { return m_cjmodSubmodules; }
    
    /**
     * 设置调试模式
     * @param enabled 是否启用调试
     */
    void setDebugMode(bool enabled);

private:
    std::string m_moduleName;
    
    // 模块信息
    std::string m_name;
    std::string m_version;
    std::string m_description;
    std::string m_author;
    std::string m_license;
    std::string m_dependencies;
    std::string m_category;
    std::string m_minCHTLVersion;
    std::string m_maxCHTLVersion;
    
    // 主文件
    std::string m_cmodMainFile;
    std::string m_cjmodMainFile;
    
    // 子模块
    std::vector<std::string> m_cmodSubmodules;
    std::vector<std::string> m_cjmodSubmodules;
    
    // 子模块文件映射
    std::map<std::string, std::vector<std::string>> m_cmodSubmoduleFiles;
    std::map<std::string, std::vector<std::string>> m_cjmodSubmoduleFiles;
    std::map<std::string, std::string> m_cmodSubmoduleInfoFiles;
    std::map<std::string, std::string> m_cjmodSubmoduleInfoFiles;
    
    bool m_debugMode;
    
    /**
     * 创建目录结构
     * @param basePath 基础路径
     * @return 是否成功
     */
    bool createDirectoryStructure(const std::string& basePath);
    
    /**
     * 生成 CMOD 导出信息
     * @return 导出信息字符串
     */
    std::string generateCMODExportInfo() const;
    
    /**
     * 生成 CJMOD 导出信息
     * @return 导出信息字符串
     */
    std::string generateCJMODExportInfo() const;
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    void debugOutput(const std::string& message) const;
};

/**
 * 混合模块管理器
 * 负责管理多个混合模块
 */
class HybridModuleManager {
public:
    /**
     * 构造函数
     */
    HybridModuleManager();
    
    /**
     * 析构函数
     */
    ~HybridModuleManager() = default;
    
    /**
     * 添加混合模块
     * @param module 混合模块
     */
    void addModule(std::shared_ptr<CMODCJMODHybrid> module);
    
    /**
     * 获取模块
     * @param moduleName 模块名称
     * @return 模块指针
     */
    std::shared_ptr<CMODCJMODHybrid> getModule(const std::string& moduleName);
    
    /**
     * 生成所有模块
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool generateAllModules(const std::string& outputPath);
    
    /**
     * 验证所有模块
     * @return 是否全部有效
     */
    bool validateAllModules() const;
    
    /**
     * 设置调试模式
     * @param enabled 是否启用调试
     */
    void setDebugMode(bool enabled);

private:
    std::map<std::string, std::shared_ptr<CMODCJMODHybrid>> m_modules;
    bool m_debugMode;
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    void debugOutput(const std::string& message) const;
};

} // namespace CHTL

#endif // CMOD_CJMOD_HYBRID_H