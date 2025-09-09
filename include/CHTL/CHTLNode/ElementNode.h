#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 元素节点类
 * 
 * 表示CHTL中的HTML元素节点
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName);
    ~ElementNode() = default;

    /**
     * @brief 获取标签名
     * @return 标签名
     */
    const std::string& getTagName() const { return m_tagName; }

    /**
     * @brief 设置标签名
     * @param tagName 标签名
     */
    void setTagName(const std::string& tagName) { m_tagName = tagName; }

    /**
     * @brief 添加属性
     * @param name 属性名
     * @param value 属性值
     */
    void addAttribute(const std::string& name, const std::string& value);

    /**
     * @brief 获取属性值
     * @param name 属性名
     * @return 属性值
     */
    std::string getAttribute(const std::string& name) const;

    /**
     * @brief 检查是否有属性
     * @param name 属性名
     * @return 是否有该属性
     */
    bool hasAttribute(const std::string& name) const;

    /**
     * @brief 移除属性
     * @param name 属性名
     * @return 是否成功移除
     */
    bool removeAttribute(const std::string& name);

    /**
     * @brief 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& getAttributes() const { return m_attributes; }

    /**
     * @brief 设置文本内容
     * @param text 文本内容
     */
    void setTextContent(const std::string& text);

    /**
     * @brief 获取文本内容
     * @return 文本内容
     */
    const std::string& getTextContent() const { return m_textContent; }

    /**
     * @brief 检查是否为自闭合标签
     * @return 是否为自闭合标签
     */
    bool isSelfClosing() const;

    /**
     * @brief 设置是否为自闭合标签
     * @param selfClosing 是否为自闭合标签
     */
    void setSelfClosing(bool selfClosing) { m_selfClosing = selfClosing; }

    /**
     * @brief 检查是否为块级元素
     * @return 是否为块级元素
     */
    bool isBlockElement() const;

    /**
     * @brief 检查是否为行内元素
     * @return 是否为行内元素
     */
    bool isInlineElement() const;

    /**
     * @brief 检查是否为行内块元素
     * @return 是否为行内块元素
     */
    bool isInlineBlockElement() const;

    /**
     * @brief 获取ID属性
     * @return ID属性值
     */
    std::string getId() const;

    /**
     * @brief 设置ID属性
     * @param id ID值
     */
    void setId(const std::string& id);

    /**
     * @brief 获取class属性
     * @return class属性值
     */
    std::string getClass() const;

    /**
     * @brief 设置class属性
     * @param className class值
     */
    void setClass(const std::string& className);

    /**
     * @brief 添加class
     * @param className 要添加的class
     */
    void addClass(const std::string& className);

    /**
     * @brief 移除class
     * @param className 要移除的class
     */
    void removeClass(const std::string& className);

    /**
     * @brief 检查是否有指定的class
     * @param className class名
     * @return 是否有该class
     */
    bool hasClass(const std::string& className) const;

    /**
     * @brief 获取所有class
     * @return class列表
     */
    std::vector<std::string> getClasses() const;

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
     * @brief 获取HTML表示
     * @return HTML字符串
     */
    std::string toHTML() const;

    /**
     * @brief 获取开始标签HTML
     * @return 开始标签HTML
     */
    std::string getStartTagHTML() const;

    /**
     * @brief 获取结束标签HTML
     * @return 结束标签HTML
     */
    std::string getEndTagHTML() const;

    /**
     * @brief 检查标签名是否为HTML5标签
     * @param tagName 标签名
     * @return 是否为HTML5标签
     */
    static bool isHTML5Tag(const std::string& tagName);

    /**
     * @brief 检查标签名是否为自闭合标签
     * @param tagName 标签名
     * @return 是否为自闭合标签
     */
    static bool isSelfClosingTag(const std::string& tagName);

    /**
     * @brief 检查标签名是否为块级元素
     * @param tagName 标签名
     * @return 是否为块级元素
     */
    static bool isBlockElement(const std::string& tagName);

    /**
     * @brief 检查标签名是否为行内元素
     * @param tagName 标签名
     * @return 是否为行内元素
     */
    static bool isInlineElement(const std::string& tagName);

    /**
     * @brief 检查标签名是否为行内块元素
     * @param tagName 标签名
     * @return 是否为行内块元素
     */
    static bool isInlineBlockElement(const std::string& tagName);

private:
    std::string m_tagName;
    std::unordered_map<std::string, std::string> m_attributes;
    std::string m_textContent;
    bool m_selfClosing;

    /**
     * @brief 初始化HTML5标签信息
     */
    static void initializeHTML5Tags();

    /**
     * @brief 格式化属性为HTML字符串
     * @return 属性HTML字符串
     */
    std::string formatAttributesHTML() const;

    /**
     * @brief 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    static std::string escapeHTML(const std::string& text);
};

} // namespace CHTL