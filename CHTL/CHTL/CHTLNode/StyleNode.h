#ifndef STYLE_NODE_H
#define STYLE_NODE_H

#include "BaseNode.h"
#include <vector>
#include <string>

namespace CHTL {

/**
 * 样式节点
 */
class StyleNode : public BaseNode {
public:
    enum class StyleType {
        INLINE,         // 内联样式
        CLASS,          // 类选择器
        ID,             // ID选择器
        PSEUDO_CLASS,   // 伪类选择器
        PSEUDO_ELEMENT, // 伪元素选择器
        GLOBAL          // 全局样式
    };

    StyleNode();
    virtual ~StyleNode() = default;
    
    // 获取/设置样式类型
    StyleType getStyleType() const { return m_styleType; }
    void setStyleType(StyleType type) { m_styleType = type; }
    
    // 获取/设置选择器
    const std::string& getSelector() const { return m_selector; }
    void setSelector(const std::string& selector) { m_selector = selector; }
    
    // 添加样式属性
    void addStyleProperty(const std::string& property, const std::string& value);
    
    // 获取样式属性
    const std::string& getStyleProperty(const std::string& property) const;
    
    // 获取所有样式属性
    const std::map<std::string, std::string>& getStyleProperties() const { return m_styleProperties; }
    
    // 检查是否为内联样式
    bool isInline() const { return m_styleType == StyleType::INLINE; }
    
    // 检查是否为选择器样式
    bool isSelector() const { 
        return m_styleType == StyleType::CLASS || 
               m_styleType == StyleType::ID || 
               m_styleType == StyleType::PSEUDO_CLASS || 
               m_styleType == StyleType::PSEUDO_ELEMENT; 
    }
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 转换为CSS字符串
    std::string toCSS() const;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;

private:
    StyleType m_styleType;
    std::string m_selector;
    std::map<std::string, std::string> m_styleProperties;
};

} // namespace CHTL

#endif // STYLE_NODE_H