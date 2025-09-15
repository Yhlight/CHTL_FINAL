#ifndef CHTLJSSPECIALNODE_H
#define CHTLJSSPECIALNODE_H

#include "CHTLJSBaseNode.h"

namespace CHTLJS {

/**
 * 选择器节点
 * 表示{{选择器}}语法
 */
class SelectorNode : public CHTLJSBaseNode {
public:
    SelectorNode(const std::string& selector);
    
    void setSelector(const std::string& selector);
    const std::string& getSelector() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string selector_;
};

/**
 * 响应式值节点
 * 表示$JS变量名$语法
 */
class ResponsiveValueNode : public CHTLJSBaseNode {
public:
    ResponsiveValueNode(const std::string& variableName);
    
    void setVariableName(const std::string& name);
    const std::string& getVariableName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string variable_name_;
};

/**
 * 表达式节点
 * 表示各种表达式
 */
class ExpressionNode : public CHTLJSBaseNode {
public:
    ExpressionNode(const std::string& expression);
    
    void setExpression(const std::string& expression);
    const std::string& getExpression() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string expression_;
};

/**
 * 语句块节点
 * 表示代码块
 */
class BlockNode : public CHTLJSBaseNode {
public:
    BlockNode();
    
    void addStatement(std::shared_ptr<CHTLJSBaseNode> statement);
    const std::vector<std::shared_ptr<CHTLJSBaseNode>>& getStatements() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::vector<std::shared_ptr<CHTLJSBaseNode>> statements_;
};

} // namespace CHTLJS

#endif // CHTLJSSPECIALNODE_H