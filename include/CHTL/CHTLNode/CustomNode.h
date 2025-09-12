#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义类型枚举
 */
enum class CustomType {
    STYLE,          // 自定义样式组
    ELEMENT,        // 自定义元素
    VAR,            // 自定义变量组
    UNKNOWN         // 未知类型
};

/**
 * @brief 自定义节点类
 * 
 * 表示CHTL自定义，包括自定义样式组、元素、变量组
 */
class CustomNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 自定义类型
     * @param name 自定义名称
     */
    CustomNode(CustomType type, const std::string& name = "");
    
    /**
     * @brief 析构函数
     */
    ~CustomNode() override;
    
    /**
     * @brief 设置自定义类型
     * @param type 自定义类型
     */
    void setCustomType(CustomType type);
    
    /**
     * @brief 获取自定义类型
     * @return 自定义类型
     */
    CustomType getCustomType() const;
    
    /**
     * @brief 设置自定义名称
     * @param name 自定义名称
     */
    void setCustomName(const std::string& name);
    
    /**
     * @brief 获取自定义名称
     * @return 自定义名称
     */
    const std::string& getCustomName() const;
    
    /**
     * @brief 添加特例化操作
     * @param operation 操作类型
     * @param target 目标
     */
    void addSpecialization(const std::string& operation, const std::string& target);
    
    /**
     * @brief 获取特例化操作
     * @return 特例化操作列表
     */
    const std::vector<std::pair<std::string, std::string>>& getSpecializations() const;
    
    /**
     * @brief 设置继承的模板
     * @param parentTemplate 父模板名称
     */
    void setParentTemplate(const std::string& parentTemplate);
    
    /**
     * @brief 获取继承的模板
     * @return 父模板名称
     */
    const std::string& getParentTemplate() const;
    
    /**
     * @brief 检查是否继承自其他模板
     * @return 是否继承
     */
    bool hasParentTemplate() const;
    
    /**
     * @brief 设置自定义内容
     * @param content 自定义内容
     */
    void setCustomContent(const std::string& content);
    
    /**
     * @brief 获取自定义内容
     * @return 自定义内容
     */
    const std::string& getCustomContent() const;
    
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
    CustomType m_customType;
    std::string m_customName;
    std::vector<std::pair<std::string, std::string>> m_specializations;
    std::string m_parentTemplate;
    std::string m_customContent;
    
    /**
     * @brief 获取自定义类型名称
     * @param type 自定义类型
     * @return 类型名称
     */
    static std::string getCustomTypeName(CustomType type);
};

} // namespace CHTL