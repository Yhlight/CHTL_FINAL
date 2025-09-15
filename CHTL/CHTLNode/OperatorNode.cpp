#include "OperatorNode.h"
#include <sstream>

namespace CHTL {

// OperatorNode implementation
OperatorNode::OperatorNode(NodeType type)
    : CHTLBaseNode(type) {
}

OperatorNode::~OperatorNode() {
}

std::string OperatorNode::generateCode() const {
    return "Operator";
}

std::string OperatorNode::toString() const {
    std::stringstream ss;
    ss << "OperatorNode(" << static_cast<int>(getType()) << ")";
    return ss.str();
}

// DeleteNode implementation
DeleteNode::DeleteNode()
    : OperatorNode(NodeType::DELETE) {
}

DeleteNode::~DeleteNode() {
}

void DeleteNode::addTarget(const std::string& target) {
    targets_.push_back(target);
}

const std::vector<std::string>& DeleteNode::getTargets() const {
    return targets_;
}

std::string DeleteNode::generateCode() const {
    std::stringstream ss;
    ss << "delete ";
    
    for (size_t i = 0; i < targets_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets_[i];
    }
    
    return ss.str();
}

// InsertNode implementation
InsertNode::InsertNode()
    : OperatorNode(NodeType::INSERT) {
}

InsertNode::~InsertNode() {
}

void InsertNode::setPosition(const std::string& position) {
    position_ = position;
}

const std::string& InsertNode::getPosition() const {
    return position_;
}

void InsertNode::setTarget(const std::string& target) {
    target_ = target;
}

const std::string& InsertNode::getTarget() const {
    return target_;
}

void InsertNode::setContent(std::shared_ptr<CHTLBaseNode> content) {
    content_ = content;
}

std::shared_ptr<CHTLBaseNode> InsertNode::getContent() const {
    return content_;
}

std::string InsertNode::generateCode() const {
    std::stringstream ss;
    ss << "insert " << position_ << " " << target_;
    if (content_) {
        ss << " " << content_->generateCode();
    }
    return ss.str();
}

// UseNode implementation
UseNode::UseNode()
    : OperatorNode(NodeType::USE) {
}

UseNode::~UseNode() {
}

void UseNode::setTarget(const std::string& target) {
    target_ = target;
}

const std::string& UseNode::getTarget() const {
    return target_;
}

std::string UseNode::generateCode() const {
    return "use " + target_;
}

// SelectorNode implementation
SelectorNode::SelectorNode(const std::string& selector)
    : CHTLBaseNode(NodeType::SELECTOR), selector_(selector) {
}

SelectorNode::~SelectorNode() {
}

void SelectorNode::setSelector(const std::string& selector) {
    selector_ = selector;
}

const std::string& SelectorNode::getSelector() const {
    return selector_;
}

std::string SelectorNode::generateCode() const {
    return "{{" + selector_ + "}}";
}

std::string SelectorNode::toString() const {
    std::stringstream ss;
    ss << "SelectorNode(\"" << selector_ << "\")";
    return ss.str();
}

// ResponsiveValueNode implementation
ResponsiveValueNode::ResponsiveValueNode(const std::string& variableName)
    : CHTLBaseNode(NodeType::RESPONSIVE_VALUE), variable_name_(variableName) {
}

ResponsiveValueNode::~ResponsiveValueNode() {
}

void ResponsiveValueNode::setVariableName(const std::string& name) {
    variable_name_ = name;
}

const std::string& ResponsiveValueNode::getVariableName() const {
    return variable_name_;
}

std::string ResponsiveValueNode::generateCode() const {
    return "$" + variable_name_ + "$";
}

std::string ResponsiveValueNode::toString() const {
    std::stringstream ss;
    ss << "ResponsiveValueNode(\"" << variable_name_ << "\")";
    return ss.str();
}

// GeneratorCommentNode implementation
GeneratorCommentNode::GeneratorCommentNode(const std::string& comment)
    : CHTLBaseNode(NodeType::GENERATOR_COMMENT), comment_(comment) {
}

GeneratorCommentNode::~GeneratorCommentNode() {
}

void GeneratorCommentNode::setComment(const std::string& comment) {
    comment_ = comment;
}

const std::string& GeneratorCommentNode::getComment() const {
    return comment_;
}

void GeneratorCommentNode::setTargetLanguage(const std::string& language) {
    target_language_ = language;
}

const std::string& GeneratorCommentNode::getTargetLanguage() const {
    return target_language_;
}

std::string GeneratorCommentNode::generateCode() const {
    if (target_language_ == "html") {
        return "<!-- " + comment_ + " -->";
    } else if (target_language_ == "css") {
        return "/* " + comment_ + " */";
    } else if (target_language_ == "javascript") {
        return "// " + comment_ + "";
    } else {
        return "# " + comment_;
    }
}

std::string GeneratorCommentNode::toString() const {
    std::stringstream ss;
    ss << "GeneratorCommentNode(\"" << comment_ << "\")";
    return ss.str();
}

// ExpressionNode implementation
ExpressionNode::ExpressionNode(const std::string& expression)
    : CHTLBaseNode(NodeType::EXPRESSION), expression_(expression) {
}

ExpressionNode::~ExpressionNode() {
}

void ExpressionNode::setExpression(const std::string& expression) {
    expression_ = expression;
}

const std::string& ExpressionNode::getExpression() const {
    return expression_;
}

std::string ExpressionNode::generateCode() const {
    return expression_;
}

std::string ExpressionNode::toString() const {
    std::stringstream ss;
    ss << "ExpressionNode(\"" << expression_ << "\")";
    return ss.str();
}

// BlockNode implementation
BlockNode::BlockNode()
    : CHTLBaseNode(NodeType::BLOCK) {
}

BlockNode::~BlockNode() {
}

void BlockNode::addStatement(std::shared_ptr<CHTLBaseNode> statement) {
    if (statement) {
        statements_.push_back(statement);
    }
}

const std::vector<std::shared_ptr<CHTLBaseNode>>& BlockNode::getStatements() const {
    return statements_;
}

std::string BlockNode::generateCode() const {
    std::stringstream ss;
    ss << "{\n";
    
    for (const auto& statement : statements_) {
        ss << "    " << statement->generateCode() << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string BlockNode::toString() const {
    std::stringstream ss;
    ss << "BlockNode(" << statements_.size() << " statements)";
    return ss.str();
}

} // namespace CHTL