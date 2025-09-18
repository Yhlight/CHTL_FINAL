#pragma once

#include "BaseNode.h"

namespace CHTL {

// 操作符节点类
class OperatorNode : public BaseNode {
private:
    String operatorType_;
    StringList operands_;
    
public:
    OperatorNode(const String& type, const std::vector<String>& targets = {}, const SourceLocation& loc = {})
        : BaseNode(NodeType::Operator, loc), operatorType_(type) {}
    
    const String& getOperatorType() const { return operatorType_; }
    void setOperatorType(const String& type) { operatorType_ = type; }
    
    const StringList& getOperands() const { return operands_; }
    void addOperand(const String& operand) { operands_.push_back(operand); }
    void clearOperands() { operands_.clear(); }
    
    String toString() const override;
    String toHTML() const override;
    bool isValid() const override;
    void accept(NodeVisitor& visitor) override;
    
    static OperatorNodePtr create(const String& type, const SourceLocation& loc = {});
};

} // namespace CHTL