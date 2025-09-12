#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief HTML元素节点类
 * 
 * 表示HTML元素，如div、span、p等
 */
class ElementNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param tagName 标签名
     */
    explicit ElementNode(const std::string& tagName = "");
    
    /**
     * @brief 析构函数
     */
    ~ElementNode() override;
    
    /**
     * @brief 设置标签名
     * @param tagName 标签名
     */
    void setTagName(const std::string& tagName);
    
    /**
     * @brief 获取标签名
     * @return 标签名
     */
    const std::string& getTagName() const;
    
    /**
     * @brief 检查是否为自闭合标签
     * @return 是否为自闭合标签
     */
    bool isSelfClosing() const;
    
    /**
     * @brief 设置是否为自闭合标签
     * @param selfClosing 是否为自闭合标签
     */
    void setSelfClosing(bool selfClosing);
    
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
     * @brief 添加类名
     * @param className 类名
     */
    void addClass(const std::string& className);
    
    /**
     * @brief 移除类名
     * @param className 类名
     * @return 是否移除成功
     */
    bool removeClass(const std::string& className);
    
    /**
     * @brief 检查是否包含类名
     * @param className 类名
     * @return 是否包含
     */
    bool hasClass(const std::string& className) const;
    
    /**
     * @brief 获取所有类名
     * @return 类名列表
     */
    std::vector<std::string> getClasses() const;
    
    /**
     * @brief 设置ID
     * @param id ID值
     */
    void setId(const std::string& id);
    
    /**
     * @brief 获取ID
     * @return ID值
     */
    std::string getId() const;
    
    /**
     * @brief 检查是否有ID
     * @return 是否有ID
     */
    bool hasId() const;
    
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
    std::string m_tagName;
    bool m_selfClosing;
    
    /**
     * @brief 检查标签是否为自闭合标签
     * @param tagName 标签名
     * @return 是否为自闭合标签
     */
    static bool isSelfClosingTag(const std::string& tagName);
    
    /**
     * @brief 检查标签是否为块级元素
     * @param tagName 标签名
     * @return 是否为块级元素
     */
    static bool isBlockTag(const std::string& tagName);
    
    /**
     * @brief 检查标签是否为行内元素
     * @param tagName 标签名
     * @return 是否为行内元素
     */
    static bool isInlineTag(const std::string& tagName);
    
    /**
     * @brief 检查标签是否为行内块元素
     * @param tagName 标签名
     * @return 是否为行内块元素
     */
    static bool isInlineBlockTag(const std::string& tagName);
};

} // namespace CHTL