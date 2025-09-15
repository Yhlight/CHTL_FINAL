#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 模板类型枚举
 */
enum class TemplateType {
    STYLE,          // 样式组模板
    ELEMENT,        // 元素模板
    VAR,            // 变量组模板
    UNKNOWN         // 未知类型
};

/**
 * @brief 模板节点类
 * 
 * 表示CHTL模板，包括样式组模板、元素模板、变量组模板
 */
class TemplateNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 模板类型
     * @param name 模板名称
     */
    TemplateNode(TemplateType type, const std::string& name = "");
    
    /**
     * @brief 析构函数
     */
    ~TemplateNode() override;
    
    /**
     * @brief 设置模板类型
     * @param type 模板类型
     */
    void setTemplateType(TemplateType type);
    
    /**
     * @brief 获取模板类型
     * @return 模板类型
     */
    TemplateType getTemplateType() const;
    
    /**
     * @brief 设置模板名称
     * @param name 模板名称
     */
    void setTemplateName(const std::string& name);
    
    /**
     * @brief 获取模板名称
     * @return 模板名称
     */
    const std::string& getTemplateName() const;
    
    /**
     * @brief 添加模板参数
     * @param paramName 参数名
     * @param defaultValue 默认值
     */
    void addParameter(const std::string& paramName, const std::string& defaultValue = "");
    
    /**
     * @brief 获取模板参数
     * @param paramName 参数名
     * @return 参数值
     */
    std::string getParameter(const std::string& paramName) const;
    
    /**
     * @brief 检查是否有参数
     * @param paramName 参数名
     * @return 是否有参数
     */
    bool hasParameter(const std::string& paramName) const;
    
    /**
     * @brief 获取所有参数
     * @return 参数映射
     */
    const std::unordered_map<std::string, std::string>& getParameters() const;
    
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
     * @brief 设置模板内容
     * @param content 模板内容
     */
    void setTemplateContent(const std::string& content);
    
    /**
     * @brief 获取模板内容
     * @return 模板内容
     */
    const std::string& getTemplateContent() const;
    
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
    TemplateType m_templateType;
    std::string m_templateName;
    std::unordered_map<std::string, std::string> m_parameters;
    std::string m_parentTemplate;
    std::string m_templateContent;
    
    /**
     * @brief 获取模板类型名称
     * @param type 模板类型
     * @return 类型名称
     */
    static std::string getTemplateTypeName(TemplateType type);
};

} // namespace CHTL