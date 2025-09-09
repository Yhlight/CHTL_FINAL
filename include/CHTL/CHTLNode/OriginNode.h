#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 原始嵌入类型枚举
 */
enum class OriginType {
    HTML,           // HTML代码
    CSS,            // CSS代码
    JAVASCRIPT,     // JavaScript代码
    CUSTOM          // 自定义类型
};

/**
 * @brief 原始嵌入节点
 * 表示原始代码嵌入，不会被CHTL处理
 */
class OriginNode : public BaseNode {
public:
    OriginNode(OriginType type, const std::string& name = "");
    virtual ~OriginNode();
    
    // 原始类型
    OriginType getOriginType() const;
    
    // 原始内容
    void setOriginContent(const std::string& content);
    std::string getOriginContent() const;
    
    // 原始名称（用于命名原始嵌入）
    void setOriginName(const std::string& name);
    std::string getOriginName() const;
    bool hasOriginName() const;
    
    // 自定义类型名称
    void setCustomTypeName(const std::string& typeName);
    std::string getCustomTypeName() const;
    bool hasCustomTypeName() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    OriginType m_originType;
    std::string m_originContent;
    std::string m_originName;
    std::string m_customTypeName;
};

} // namespace CHTL