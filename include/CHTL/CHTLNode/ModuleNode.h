#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 模块类型枚举
 */
enum class ModuleType {
    CMOD,           // CHTL模块
    CJMOD,          // CHTL JS模块
    HTML,           // HTML模块
    CSS,            // CSS模块
    JS,             // JavaScript模块
    UNKNOWN         // 未知类型
};

/**
 * @brief 模块节点类
 * 
 * 表示CHTL模块，支持CMOD和CJMOD模块
 */
class ModuleNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 模块类型
     * @param name 模块名称
     */
    ModuleNode(ModuleType type, const std::string& name = "");
    
    /**
     * @brief 析构函数
     */
    ~ModuleNode() override;
    
    /**
     * @brief 设置模块类型
     * @param type 模块类型
     */
    void setModuleType(ModuleType type);
    
    /**
     * @brief 获取模块类型
     * @return 模块类型
     */
    ModuleType getModuleType() const;
    
    /**
     * @brief 设置模块名称
     * @param name 模块名称
     */
    void setModuleName(const std::string& name);
    
    /**
     * @brief 获取模块名称
     * @return 模块名称
     */
    const std::string& getModuleName() const;
    
    /**
     * @brief 设置模块版本
     * @param version 模块版本
     */
    void setModuleVersion(const std::string& version);
    
    /**
     * @brief 获取模块版本
     * @return 模块版本
     */
    const std::string& getModuleVersion() const;
    
    /**
     * @brief 设置模块路径
     * @param path 模块路径
     */
    void setModulePath(const std::string& path);
    
    /**
     * @brief 获取模块路径
     * @return 模块路径
     */
    const std::string& getModulePath() const;
    
    /**
     * @brief 设置模块描述
     * @param description 模块描述
     */
    void setModuleDescription(const std::string& description);
    
    /**
     * @brief 获取模块描述
     * @return 模块描述
     */
    const std::string& getModuleDescription() const;
    
    /**
     * @brief 添加模块依赖
     * @param dependency 依赖名称
     * @param version 依赖版本
     */
    void addModuleDependency(const std::string& dependency, const std::string& version = "");
    
    /**
     * @brief 获取模块依赖
     * @param dependency 依赖名称
     * @return 依赖版本
     */
    std::string getModuleDependency(const std::string& dependency) const;
    
    /**
     * @brief 检查是否有模块依赖
     * @param dependency 依赖名称
     * @return 是否有依赖
     */
    bool hasModuleDependency(const std::string& dependency) const;
    
    /**
     * @brief 获取所有模块依赖
     * @return 模块依赖映射
     */
    const std::unordered_map<std::string, std::string>& getModuleDependencies() const;
    
    /**
     * @brief 设置模块内容
     * @param content 模块内容
     */
    void setModuleContent(const std::string& content);
    
    /**
     * @brief 获取模块内容
     * @return 模块内容
     */
    const std::string& getModuleContent() const;
    
    /**
     * @brief 设置模块导出
     * @param exportName 导出名称
     * @param exportValue 导出值
     */
    void setModuleExport(const std::string& exportName, const std::string& exportValue);
    
    /**
     * @brief 获取模块导出
     * @param exportName 导出名称
     * @return 导出值
     */
    std::string getModuleExport(const std::string& exportName) const;
    
    /**
     * @brief 检查是否有模块导出
     * @param exportName 导出名称
     * @return 是否有导出
     */
    bool hasModuleExport(const std::string& exportName) const;
    
    /**
     * @brief 获取所有模块导出
     * @return 模块导出映射
     */
    const std::unordered_map<std::string, std::string>& getModuleExports() const;
    
    /**
     * @brief 设置是否启用
     * @param enabled 是否启用
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief 是否启用
     * @return 是否启用
     */
    bool isEnabled() const;
    
    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override {}
    
    /**
     * @brief 获取模块类型名称
     * @param type 模块类型
     * @return 类型名称
     */
    static std::string getModuleTypeName(ModuleType type);

private:
    ModuleType m_moduleType;
    std::string m_moduleName;
    std::string m_moduleVersion;
    std::string m_modulePath;
    std::string m_moduleDescription;
    std::unordered_map<std::string, std::string> m_moduleDependencies;
    std::string m_moduleContent;
    std::unordered_map<std::string, std::string> m_moduleExports;
    bool m_enabled;
};

} // namespace CHTL