#ifndef CHTL_OPERATORNODE_H
#define CHTL_OPERATORNODE_H

#include "CHTLBaseNode.h"

namespace CHTL {

/**
 * 操作符节点基类
 */
class OperatorNode : public CHTLBaseNode {
public:
    OperatorNode(NodeType type);
    ~OperatorNode();
    
    std::string generateCode() const override;
    std::string toString() const override;
};

/**
 * Delete操作节点
 */
class DeleteNode : public OperatorNode {
public:
    DeleteNode();
    ~DeleteNode();
    
    void addTarget(const std::string& target);
    const std::vector<std::string>& getTargets() const;
    
    std::string generateCode() const override;

private:
    std::vector<std::string> targets_;
};

/**
 * Insert操作节点
 */
class InsertNode : public OperatorNode {
public:
    InsertNode();
    ~InsertNode();
    
    void setPosition(const std::string& position);
    const std::string& getPosition() const;
    
    void setTarget(const std::string& target);
    const std::string& getTarget() const;
    
    void setContent(std::shared_ptr<CHTLBaseNode> content);
    std::shared_ptr<CHTLBaseNode> getContent() const;
    
    std::string generateCode() const override;

private:
    std::string position_;
    std::string target_;
    std::shared_ptr<CHTLBaseNode> content_;
};

/**
 * Use操作节点
 */
class UseNode : public OperatorNode {
public:
    UseNode();
    ~UseNode();
    
    void setTarget(const std::string& target);
    const std::string& getTarget() const;
    
    std::string generateCode() const override;

private:
    std::string target_;
};

/**
 * 选择器节点
 */
class SelectorNode : public CHTLBaseNode {
public:
    SelectorNode(const std::string& selector);
    ~SelectorNode();
    
    void setSelector(const std::string& selector);
    const std::string& getSelector() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string selector_;
};

/**
 * 响应式值节点
 */
class ResponsiveValueNode : public CHTLBaseNode {
public:
    ResponsiveValueNode(const std::string& variableName);
    ~ResponsiveValueNode();
    
    void setVariableName(const std::string& name);
    const std::string& getVariableName() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string variable_name_;
};

/**
 * 生成器注释节点
 */
class GeneratorCommentNode : public CHTLBaseNode {
public:
    GeneratorCommentNode(const std::string& comment);
    ~GeneratorCommentNode();
    
    void setComment(const std::string& comment);
    const std::string& getComment() const;
    
    void setTargetLanguage(const std::string& language);
    const std::string& getTargetLanguage() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string comment_;
    std::string target_language_;
};

/**
 * 表达式节点
 */
class ExpressionNode : public CHTLBaseNode {
public:
    ExpressionNode(const std::string& expression);
    ~ExpressionNode();
    
    void setExpression(const std::string& expression);
    const std::string& getExpression() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::string expression_;
};

/**
 * 块节点
 */
class BlockNode : public CHTLBaseNode {
public:
    BlockNode();
    ~BlockNode();
    
    void addStatement(std::shared_ptr<CHTLBaseNode> statement);
    const std::vector<std::shared_ptr<CHTLBaseNode>>& getStatements() const;
    
    std::string generateCode() const override;
    std::string toString() const override;

private:
    std::vector<std::shared_ptr<CHTLBaseNode>> statements_;
};

} // namespace CHTL

#endif // CHTL_OPERATORNODE_H