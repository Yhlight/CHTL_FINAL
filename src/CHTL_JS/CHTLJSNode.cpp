#include "CHTL_JS/CHTLJSNode.h"
#include "CHTL_JS/CHTLJSVisitor.h"
#include <algorithm>

namespace CHTL_JS {

// CHTLJSNode 实现
CHTLJSNode::CHTLJSNode(CHTLJSNodeType type, const Position& pos)
    : type_(type), position_(pos) {
}

void CHTLJSNode::addChild(std::shared_ptr<CHTLJSNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void CHTLJSNode::removeChild(std::shared_ptr<CHTLJSNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void CHTLJSNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

std::shared_ptr<CHTLJSNode> CHTLJSNode::clone() const {
    auto cloned = std::make_shared<CHTLJSNode>(type_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// IdentifierNode 实现
IdentifierNode::IdentifierNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Identifier, pos), name_(name) {
}

void IdentifierNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> IdentifierNode::clone() const {
    auto cloned = std::make_shared<IdentifierNode>(name_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// LiteralNode 实现
LiteralNode::LiteralNode(const LiteralValue& value, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Literal, pos), value_(value) {
}

void LiteralNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> LiteralNode::clone() const {
    auto cloned = std::make_shared<LiteralNode>(value_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ExpressionNode 实现
ExpressionNode::ExpressionNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Expression, pos) {
}

void ExpressionNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ExpressionNode::clone() const {
    auto cloned = std::make_shared<ExpressionNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// StatementNode 实现
StatementNode::StatementNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Statement, pos) {
}

void StatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> StatementNode::clone() const {
    auto cloned = std::make_shared<StatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// BlockNode 实现
BlockNode::BlockNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Block, pos) {
}

void BlockNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> BlockNode::clone() const {
    auto cloned = std::make_shared<BlockNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// FunctionNode 实现
FunctionNode::FunctionNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Function, pos), name_(name) {
}

void FunctionNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> FunctionNode::clone() const {
    auto cloned = std::make_shared<FunctionNode>(name_, position_);
    cloned->text_ = text_;
    cloned->parameters_ = parameters_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// FunctionCallNode 实现
FunctionCallNode::FunctionCallNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::FunctionCall, pos), name_(name) {
}

void FunctionCallNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> FunctionCallNode::clone() const {
    auto cloned = std::make_shared<FunctionCallNode>(name_, position_);
    cloned->text_ = text_;
    cloned->arguments_ = arguments_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ArrowFunctionNode 实现
ArrowFunctionNode::ArrowFunctionNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::ArrowFunction, pos) {
}

void ArrowFunctionNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ArrowFunctionNode::clone() const {
    auto cloned = std::make_shared<ArrowFunctionNode>(position_);
    cloned->text_ = text_;
    cloned->parameters_ = parameters_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ObjectNode 实现
ObjectNode::ObjectNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Object, pos) {
}

void ObjectNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ObjectNode::clone() const {
    auto cloned = std::make_shared<ObjectNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ObjectPropertyNode 实现
ObjectPropertyNode::ObjectPropertyNode(const String& key, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::ObjectProperty, pos), key_(key) {
}

void ObjectPropertyNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ObjectPropertyNode::clone() const {
    auto cloned = std::make_shared<ObjectPropertyNode>(key_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ArrayNode 实现
ArrayNode::ArrayNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Array, pos) {
}

void ArrayNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ArrayNode::clone() const {
    auto cloned = std::make_shared<ArrayNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ArrayElementNode 实现
ArrayElementNode::ArrayElementNode(int index, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::ArrayElement, pos), index_(index) {
}

void ArrayElementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ArrayElementNode::clone() const {
    auto cloned = std::make_shared<ArrayElementNode>(index_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ControlFlowNode 实现
ControlFlowNode::ControlFlowNode(CHTLJSNodeType type, const Position& pos)
    : CHTLJSNode(type, pos) {
}

// IfStatementNode 实现
IfStatementNode::IfStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::IfStatement, pos) {
}

void IfStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> IfStatementNode::clone() const {
    auto cloned = std::make_shared<IfStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ForStatementNode 实现
ForStatementNode::ForStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::ForStatement, pos) {
}

void ForStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ForStatementNode::clone() const {
    auto cloned = std::make_shared<ForStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// WhileStatementNode 实现
WhileStatementNode::WhileStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::WhileStatement, pos) {
}

void WhileStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> WhileStatementNode::clone() const {
    auto cloned = std::make_shared<WhileStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// SwitchStatementNode 实现
SwitchStatementNode::SwitchStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::SwitchStatement, pos) {
}

void SwitchStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> SwitchStatementNode::clone() const {
    auto cloned = std::make_shared<SwitchStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// CaseStatementNode 实现
CaseStatementNode::CaseStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::CaseStatement, pos) {
}

void CaseStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> CaseStatementNode::clone() const {
    auto cloned = std::make_shared<CaseStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// BreakStatementNode 实现
BreakStatementNode::BreakStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::BreakStatement, pos) {
}

void BreakStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> BreakStatementNode::clone() const {
    auto cloned = std::make_shared<BreakStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ContinueStatementNode 实现
ContinueStatementNode::ContinueStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::ContinueStatement, pos) {
}

void ContinueStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ContinueStatementNode::clone() const {
    auto cloned = std::make_shared<ContinueStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ReturnStatementNode 实现
ReturnStatementNode::ReturnStatementNode(const Position& pos)
    : ControlFlowNode(CHTLJSNodeType::ReturnStatement, pos) {
}

void ReturnStatementNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ReturnStatementNode::clone() const {
    auto cloned = std::make_shared<ReturnStatementNode>(position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// VariableDeclarationNode 实现
VariableDeclarationNode::VariableDeclarationNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::VariableDeclaration, pos), name_(name) {
}

void VariableDeclarationNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> VariableDeclarationNode::clone() const {
    auto cloned = std::make_shared<VariableDeclarationNode>(name_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// VariableAssignmentNode 实现
VariableAssignmentNode::VariableAssignmentNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::VariableAssignment, pos), name_(name) {
}

void VariableAssignmentNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> VariableAssignmentNode::clone() const {
    auto cloned = std::make_shared<VariableAssignmentNode>(name_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// OperatorNode 实现
OperatorNode::OperatorNode(OperatorType type, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Operator, pos), operatorType_(type) {
}

void OperatorNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> OperatorNode::clone() const {
    auto cloned = std::make_shared<OperatorNode>(operatorType_, position_);
    cloned->text_ = text_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// VirtualObjectNode 实现
VirtualObjectNode::VirtualObjectNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::VirtualObject, pos), name_(name) {
}

void VirtualObjectNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> VirtualObjectNode::clone() const {
    auto cloned = std::make_shared<VirtualObjectNode>(name_, position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ListenNode 实现
ListenNode::ListenNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Listen, pos) {
}

void ListenNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> ListenNode::clone() const {
    auto cloned = std::make_shared<ListenNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// DelegateNode 实现
DelegateNode::DelegateNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Delegate, pos) {
}

void DelegateNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> DelegateNode::clone() const {
    auto cloned = std::make_shared<DelegateNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// AnimateNode 实现
AnimateNode::AnimateNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Animate, pos) {
}

void AnimateNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> AnimateNode::clone() const {
    auto cloned = std::make_shared<AnimateNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// RouterNode 实现
RouterNode::RouterNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Router, pos) {
}

void RouterNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> RouterNode::clone() const {
    auto cloned = std::make_shared<RouterNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// FileLoaderNode 实现
FileLoaderNode::FileLoaderNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::FileLoader, pos) {
}

void FileLoaderNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> FileLoaderNode::clone() const {
    auto cloned = std::make_shared<FileLoaderNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// UtilNode 实现
UtilNode::UtilNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Util, pos) {
}

void UtilNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> UtilNode::clone() const {
    auto cloned = std::make_shared<UtilNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// PrintNode 实现
PrintNode::PrintNode(const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::Print, pos) {
}

void PrintNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> PrintNode::clone() const {
    auto cloned = std::make_shared<PrintNode>(position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// INeverAwayNode 实现
INeverAwayNode::INeverAwayNode(const String& name, const Position& pos)
    : CHTLJSNode(CHTLJSNodeType::INeverAway, pos), name_(name) {
}

void INeverAwayNode::accept(CHTLJSVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLJSNode> INeverAwayNode::clone() const {
    auto cloned = std::make_shared<INeverAwayNode>(name_, position_);
    cloned->text_ = text_;
    cloned->config_ = config_;
    
    // 克隆子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

} // namespace CHTL_JS