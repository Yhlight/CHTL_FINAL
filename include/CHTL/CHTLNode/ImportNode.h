#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    CHTL,           // CHTL文件
    HTML,           // HTML文件
    CSS,            // CSS文件
    JS,             // JavaScript文件
    CMOD,           // CHTL模块文件
    CJMOD,          // CHTL JS模块文件
    UNKNOWN         // 未知类型
};

/**
 * @brief 导入模式枚举
 */
enum class ImportMode {
    PRECISE,        // 精确导入
    TYPE,           // 类型导入
    WILDCARD,       // 通配符导入
    UNKNOWN         // 未知模式
};

/**
 * @brief 导入节点类
 * 
 * 表示CHTL导入语句，支持导入CHTL、HTML、CSS、JS文件
 */
class ImportNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param importType 导入类型
     * @param filePath 文件路径
     */
    ImportNode(ImportType importType, const std::string& filePath = "");
    
    /**
     * @brief 析构函数
     */
    ~ImportNode() override;
    
    /**
     * @brief 设置导入类型
     * @param importType 导入类型
     */
    void setImportType(ImportType importType);
    
    /**
     * @brief 获取导入类型
     * @return 导入类型
     */
    ImportType getImportType() const;
    
    /**
     * @brief 设置文件路径
     * @param filePath 文件路径
     */
    void setFilePath(const std::string& filePath);
    
    /**
     * @brief 获取文件路径
     * @return 文件路径
     */
    const std::string& getFilePath() const;
    
    /**
     * @brief 设置导入模式
     * @param mode 导入模式
     */
    void setImportMode(ImportMode mode);
    
    /**
     * @brief 获取导入模式
     * @return 导入模式
     */
    ImportMode getImportMode() const;
    
    /**
     * @brief 添加导入项
     * @param item 导入项
     */
    void addImportItem(const std::string& item);
    
    /**
     * @brief 获取导入项列表
     * @return 导入项列表
     */
    const std::vector<std::string>& getImportItems() const;
    
    /**
     * @brief 设置别名
     * @param alias 别名
     */
    void setAlias(const std::string& alias);
    
    /**
     * @brief 获取别名
     * @return 别名
     */
    const std::string& getAlias() const;
    
    /**
     * @brief 检查是否有别名
     * @return 是否有别名
     */
    bool hasAlias() const;
    
    /**
     * @brief 设置命名空间
     * @param ns 命名空间
     */
    void setNamespace(const std::string& ns);
    
    /**
     * @brief 获取命名空间
     * @return 命名空间
     */
    const std::string& getNamespace() const;
    
    /**
     * @brief 检查是否有命名空间
     * @return 是否有命名空间
     */
    bool hasNamespace() const;
    
    /**
     * @brief 设置导入内容
     * @param content 导入内容
     */
    void setImportContent(const std::string& content);
    
    /**
     * @brief 获取导入内容
     * @return 导入内容
     */
    const std::string& getImportContent() const;
    
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

private:
    ImportType m_importType;
    std::string m_filePath;
    ImportMode m_importMode;
    std::vector<std::string> m_importItems;
    std::string m_alias;
    std::string m_namespace;
    std::string m_importContent;
    
    /**
     * @brief 获取导入类型名称
     * @param type 导入类型
     * @return 类型名称
     */
    static std::string getImportTypeName(ImportType type);
    
    /**
     * @brief 获取导入模式名称
     * @param mode 导入模式
     * @return 模式名称
     */
    static std::string getImportModeName(ImportMode mode);
};

} // namespace CHTL