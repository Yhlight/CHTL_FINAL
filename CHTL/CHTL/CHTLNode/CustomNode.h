#ifndef CUSTOM_NODE_H
#define CUSTOM_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * 自定义节点
 */
class CustomNode : public BaseNode {
public:
    enum class CustomType {
        STYLE,          // 自定义样式组
        ELEMENT,        // 自定义元素
        VAR             // 自定义变量组
    };

    CustomNode(CustomType type, const std::string& name);
    virtual ~CustomNode() = default;
    
    // 获取/设置自定义类型
    CustomType getCustomType() const { return m_customType; }
    void setCustomType(CustomType type) { m_customType = type; }
    
    // 获取/设置自定义名称
    const std::string& getCustomName() const { return m_customName; }
    void setCustomName(const std::string& name) { m_customName = name; }
    
    // 检查是否为自定义样式组
    bool isStyleCustom() const { return m_customType == CustomType::STYLE; }
    
    // 检查是否为自定义元素
    bool isElementCustom() const { return m_customType == CustomType::ELEMENT; }
    
    // 检查是否为自定义变量组
    bool isVarCustom() const { return m_customType == CustomType::VAR; }
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;

private:
    CustomType m_customType;
    std::string m_customName;
};

} // namespace CHTL

#endif // CUSTOM_NODE_H