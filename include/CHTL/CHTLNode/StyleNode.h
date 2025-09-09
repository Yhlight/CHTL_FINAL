#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 样式属性类
 * 
 * 表示CSS样式属性
 */
class StyleProperty {
public:
    StyleProperty(const std::string& name, const std::string& value);
    ~StyleProperty() = default;

    /**
     * @brief 获取属性名
     * @return 属性名
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief 设置属性名
     * @param name 属性名
     */
    void setName(const std::string& name) { m_name = name; }

    /**
     * @brief 获取属性值
     * @return 属性值
     */
    const std::string& getValue() const { return m_value; }

    /**
     * @brief 设置属性值
     * @param value 属性值
     */
    void setValue(const std::string& value) { m_value = value; }

    /**
     * @brief 检查是否为重要属性
     * @return 是否为重要属性
     */
    bool isImportant() const { return m_important; }

    /**
     * @brief 设置是否为重要属性
     * @param important 是否为重要属性
     */
    void setImportant(bool important) { m_important = important; }

    /**
     * @brief 获取CSS字符串表示
     * @return CSS字符串
     */
    std::string toCSS() const;

private:
    std::string m_name;
    std::string m_value;
    bool m_important;
};

/**
 * @brief 样式选择器类
 * 
 * 表示CSS选择器
 */
class StyleSelector {
public:
    enum class Type {
        TAG,        // 标签选择器
        CLASS,      // 类选择器
        ID,         // ID选择器
        PSEUDO,     // 伪类选择器
        PSEUDO_ELEMENT, // 伪元素选择器
        DESCENDANT, // 后代选择器
        CHILD,      // 子选择器
        ADJACENT,   // 相邻选择器
        SIBLING,    // 兄弟选择器
        ATTRIBUTE,  // 属性选择器
        UNIVERSAL   // 通配符选择器
    };

    StyleSelector(Type type, const std::string& value);
    ~StyleSelector() = default;

    /**
     * @brief 获取选择器类型
     * @return 选择器类型
     */
    Type getType() const { return m_type; }

    /**
     * @brief 设置选择器类型
     * @param type 选择器类型
     */
    void setType(Type type) { m_type = type; }

    /**
     * @brief 获取选择器值
     * @return 选择器值
     */
    const std::string& getValue() const { return m_value; }

    /**
     * @brief 设置选择器值
     * @param value 选择器值
     */
    void setValue(const std::string& value) { m_value = value; }

    /**
     * @brief 获取CSS字符串表示
     * @return CSS字符串
     */
    std::string toCSS() const;

    /**
     * @brief 获取选择器类型字符串
     * @return 类型字符串
     */
    static std::string typeToString(Type type);

private:
    Type m_type;
    std::string m_value;
};

/**
 * @brief 样式规则类
 * 
 * 表示CSS规则
 */
class StyleRule {
public:
    StyleRule();
    ~StyleRule() = default;

    /**
     * @brief 添加选择器
     * @param selector 选择器
     */
    void addSelector(std::unique_ptr<StyleSelector> selector);

    /**
     * @brief 获取选择器列表
     * @return 选择器列表
     */
    const std::vector<std::unique_ptr<StyleSelector>>& getSelectors() const { return m_selectors; }

    /**
     * @brief 添加属性
     * @param property 属性
     */
    void addProperty(std::unique_ptr<StyleProperty> property);

    /**
     * @brief 获取属性列表
     * @return 属性列表
     */
    const std::vector<std::unique_ptr<StyleProperty>>& getProperties() const { return m_properties; }

    /**
     * @brief 获取CSS字符串表示
     * @return CSS字符串
     */
    std::string toCSS() const;

    /**
     * @brief 检查是否为空规则
     * @return 是否为空规则
     */
    bool isEmpty() const;

private:
    std::vector<std::unique_ptr<StyleSelector>> m_selectors;
    std::vector<std::unique_ptr<StyleProperty>> m_properties;
};

/**
 * @brief 样式节点类
 * 
 * 表示CHTL中的样式节点
 */
class StyleNode : public BaseNode {
public:
    StyleNode();
    ~StyleNode() = default;

    /**
     * @brief 添加样式规则
     * @param rule 样式规则
     */
    void addRule(std::unique_ptr<StyleRule> rule);

    /**
     * @brief 获取样式规则列表
     * @return 样式规则列表
     */
    const std::vector<std::unique_ptr<StyleRule>>& getRules() const { return m_rules; }

    /**
     * @brief 添加内联样式属性
     * @param name 属性名
     * @param value 属性值
     */
    void addInlineProperty(const std::string& name, const std::string& value);

    /**
     * @brief 获取内联样式属性
     * @param name 属性名
     * @return 属性值
     */
    std::string getInlineProperty(const std::string& name) const;

    /**
     * @brief 检查是否有内联样式属性
     * @param name 属性名
     * @return 是否有该属性
     */
    bool hasInlineProperty(const std::string& name) const;

    /**
     * @brief 移除内联样式属性
     * @param name 属性名
     * @return 是否成功移除
     */
    bool removeInlineProperty(const std::string& name);

    /**
     * @brief 获取所有内联样式属性
     * @return 内联样式属性映射
     */
    const std::unordered_map<std::string, std::string>& getInlineProperties() const { return m_inlineProperties; }

    /**
     * @brief 检查是否为局部样式块
     * @return 是否为局部样式块
     */
    bool isLocal() const { return m_isLocal; }

    /**
     * @brief 设置是否为局部样式块
     * @param isLocal 是否为局部样式块
     */
    void setLocal(bool isLocal) { m_isLocal = isLocal; }

    /**
     * @brief 检查是否为全局样式块
     * @return 是否为全局样式块
     */
    bool isGlobal() const { return !m_isLocal; }

    /**
     * @brief 设置是否为全局样式块
     * @param isGlobal 是否为全局样式块
     */
    void setGlobal(bool isGlobal) { m_isLocal = !isGlobal; }

    /**
     * @brief 获取CSS字符串表示
     * @return CSS字符串
     */
    std::string toCSS() const;

    /**
     * @brief 获取内联样式字符串
     * @return 内联样式字符串
     */
    std::string toInlineCSS() const;

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
     * @brief 检查样式属性名是否有效
     * @param name 属性名
     * @return 是否有效
     */
    static bool isValidPropertyName(const std::string& name);

    /**
     * @brief 检查样式属性值是否有效
     * @param value 属性值
     * @return 是否有效
     */
    static bool isValidPropertyValue(const std::string& value);

    /**
     * @brief 格式化CSS属性名
     * @param name 属性名
     * @return 格式化后的属性名
     */
    static std::string formatPropertyName(const std::string& name);

    /**
     * @brief 格式化CSS属性值
     * @param value 属性值
     * @return 格式化后的属性值
     */
    static std::string formatPropertyValue(const std::string& value);

private:
    std::vector<std::unique_ptr<StyleRule>> m_rules;
    std::unordered_map<std::string, std::string> m_inlineProperties;
    bool m_isLocal;

    /**
     * @brief 初始化CSS属性验证规则
     */
    static void initializeCSSProperties();
};

} // namespace CHTL