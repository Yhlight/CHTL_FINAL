#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief 元素节点类
 * 
 * 表示 HTML 元素节点
 */
class ElementNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param tagName 标签名
     * @param line 行号
     * @param column 列号
     */
    ElementNode(const std::string& tagName, size_t line = 0, size_t column = 0);
    
    /**
     * @brief 析构函数
     */
    ~ElementNode() override = default;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者对象
     */
    void accept(Visitor& visitor) override;
    
    /**
     * @brief 生成代码
     * @return 生成的代码
     */
    std::string generate() override;
    
    /**
     * @brief 获取标签名
     * @return 标签名
     */
    const std::string& getTagName() const { return tagName_; }
    
    /**
     * @brief 设置标签名
     * @param tagName 标签名
     */
    void setTagName(const std::string& tagName) { tagName_ = tagName; }
    
    /**
     * @brief 检查是否为自闭合标签
     * @return 是否为自闭合标签
     */
    bool isSelfClosing() const { return isSelfClosing_; }
    
    /**
     * @brief 设置是否为自闭合标签
     * @param selfClosing 是否为自闭合标签
     */
    void setSelfClosing(bool selfClosing) { isSelfClosing_ = selfClosing; }
    
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
     * @brief 转换为字符串
     * @return 字符串表示
     */
    std::string toString() const override;

private:
    std::string tagName_;      ///< 标签名
    bool isSelfClosing_;       ///< 是否为自闭合标签
    
    /**
     * @brief 检查是否为自闭合标签
     * @param tagName 标签名
     * @return 是否为自闭合标签
     */
    bool isSelfClosingTag(const std::string& tagName) const;
    
    /**
     * @brief 检查是否为块级元素
     * @param tagName 标签名
     * @return 是否为块级元素
     */
    bool isBlockTag(const std::string& tagName) const;
    
    /**
     * @brief 检查是否为行内元素
     * @param tagName 标签名
     * @return 是否为行内元素
     */
    bool isInlineTag(const std::string& tagName) const;
    
    /**
     * @brief 检查是否为行内块元素
     * @param tagName 标签名
     * @return 是否为行内块元素
     */
    bool isInlineBlockTag(const std::string& tagName) const;
};

} // namespace CHTL