#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    HTML,           // HTML文件导入
    CSS,            // CSS文件导入
    JAVASCRIPT,     // JavaScript文件导入
    CHTL,           // CHTL文件导入
    CJMOD,          // CJMOD文件导入
    CONFIG          // 配置组导入
};

/**
 * @brief 导入模式枚举
 */
enum class ImportMode {
    EXACT,          // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配导入
};

/**
 * @brief 导入节点
 * 表示文件或模块导入
 */
class ImportNode : public BaseNode {
public:
    ImportNode(ImportType type, const std::string& path);
    virtual ~ImportNode();
    
    // 导入类型
    ImportType getImportType() const;
    
    // 导入路径
    void setImportPath(const std::string& path);
    std::string getImportPath() const;
    
    // 导入模式
    void setImportMode(ImportMode mode);
    ImportMode getImportMode() const;
    
    // 导入名称
    void setImportName(const std::string& name);
    std::string getImportName() const;
    bool hasImportName() const;
    
    // 别名
    void setAlias(const std::string& alias);
    std::string getAlias() const;
    bool hasAlias() const;
    
    // 命名空间
    void setNamespace(const std::string& namespaceName);
    std::string getNamespace() const;
    bool hasNamespace() const;
    
    // 目标名称（用于精确导入）
    void setTargetName(const std::string& name);
    std::string getTargetName() const;
    bool hasTargetName() const;
    
    // 目标类型（用于精确导入）
    void setTargetType(const std::string& type);
    std::string getTargetType() const;
    bool hasTargetType() const;
    
    // 通配符模式
    void setWildcardPattern(const std::string& pattern);
    std::string getWildcardPattern() const;
    bool hasWildcardPattern() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    ImportType m_importType;
    std::string m_importPath;
    ImportMode m_importMode;
    std::string m_importName;
    std::string m_alias;
    std::string m_namespace;
    std::string m_targetName;
    std::string m_targetType;
    std::string m_wildcardPattern;
};

} // namespace CHTL