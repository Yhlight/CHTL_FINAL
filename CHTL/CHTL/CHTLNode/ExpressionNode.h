#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

/**
 * 表达式节点
 */
class ExpressionNode : public BaseNode {
public:
    enum class ExpressionType {
        ARITHMETIC,     // 算术表达式
        LOGICAL,        // 逻辑表达式
        CONDITIONAL,    // 条件表达式
        REFERENCE       // 引用表达式
    };

    ExpressionNode(ExpressionType type, const std::string& expression);
    virtual ~ExpressionNode() = default;
    
    // 获取/设置表达式类型
    ExpressionType getExpressionType() const { return m_expressionType; }
    void setExpressionType(ExpressionType type) { m_expressionType = type; }
    
    // 获取/设置表达式内容
    const std::string& getExpression() const { return m_value; }
    void setExpression(const std::string& expression) { m_value = expression; }
    
    // 计算表达式值
    std::string evaluate() const;
    
    // 克隆节点
    std::shared_ptr<BaseNode> clone() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
    // 接受访问者
    void accept(NodeVisitor& visitor) override;

private:
    ExpressionType m_expressionType;
    
    // 计算算术表达式
    std::string evaluateArithmetic() const;
    
    // 计算逻辑表达式
    std::string evaluateLogical() const;
    
    // 计算条件表达式
    std::string evaluateConditional() const;
    
    // 计算引用表达式
    std::string evaluateReference() const;
};

} // namespace CHTL

#endif // EXPRESSION_NODE_H