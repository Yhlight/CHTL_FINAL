#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // @Style 样式组模板
    ELEMENT,    // @Element 元素模板
    VAR         // @Var 变量组模板
};

/**
 * @brief 模板节点类
 * 
 * 表示CHTL中的模板节点
 */
class TemplateNode : public BaseNode {
public:
    TemplateNode(TemplateType type, const std::string& name);
    ~TemplateNode() = default;

    /**
     * @brief 获取模板类型
     * @return 模板类型
     */
    TemplateType getTemplateType() const { return m_templateType; }

    /**
     * @brief 设置模板类型
     * @param type 模板类型
     */
    void setTemplateType(TemplateType type) { m_templateType = type; }

    /**
     * @brief 获取模板名称
     * @return 模板名称
     */
    const std::string& getTemplateName() const { return m_templateName; }

    /**
     * @brief 设置模板名称
     * @param name 模板名称
     */
    void setTemplateName(const std::string& name) { m_templateName = name; }

    /**
     * @brief 添加继承的模板
     * @param templateName 要继承的模板名称
     */
    void addInheritance(const std::string& templateName);

    /**
     * @brief 获取继承的模板列表
     * @return 继承的模板列表
     */
    const std::vector<std::string>& getInheritances() const { return m_inheritances; }

    /**
     * @brief 检查是否继承指定模板
     * @param templateName 模板名称
     * @return 是否继承
     */
    bool inheritsFrom(const std::string& templateName) const;

    /**
     * @brief 添加属性
     * @param name 属性名
     * @param value 属性值
     */
    void addProperty(const std::string& name, const std::string& value);

    /**
     * @brief 获取属性值
     * @param name 属性名
     * @return 属性值
     */
    std::string getProperty(const std::string& name) const;

    /**
     * @brief 检查是否有属性
     * @param name 属性名
     * @return 是否有该属性
     */
    bool hasProperty(const std::string& name) const;

    /**
     * @brief 移除属性
     * @param name 属性名
     * @return 是否成功移除
     */
    bool removeProperty(const std::string& name);

    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& getProperties() const { return m_properties; }

    /**
     * @brief 检查是否为样式组模板
     * @return 是否为样式组模板
     */
    bool isStyleTemplate() const { return m_templateType == TemplateType::STYLE; }

    /**
     * @brief 检查是否为元素模板
     * @return 是否为元素模板
     */
    bool isElementTemplate() const { return m_templateType == TemplateType::ELEMENT; }

    /**
     * @brief 检查是否为变量组模板
     * @return 是否为变量组模板
     */
    bool isVarTemplate() const { return m_templateType == TemplateType::VAR; }

    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<BaseNode> clone() const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor* visitor) override;

    /**
     * @brief 获取节点字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;

    /**
     * @brief 获取模板类型字符串
     * @return 类型字符串
     */
    static std::string templateTypeToString(TemplateType type);

    /**
     * @brief 从字符串解析模板类型
     * @param str 字符串
     * @return 模板类型
     */
    static TemplateType parseTemplateType(const std::string& str);

private:
    TemplateType m_templateType;
    std::string m_templateName;
    std::vector<std::string> m_inheritances;
    std::unordered_map<std::string, std::string> m_properties;
};

} // namespace CHTL