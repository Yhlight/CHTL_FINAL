#include "CHTL/ASTNode.h"
#include "CHTL/ASTVisitor.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace CHTL {

// ASTNode基类实现
ASTNode::ASTNode(ASTNodeType type, const std::string& name)
    : type_(type), name_(name), line_(0), column_(0) {
}

void ASTNode::addChild(std::shared_ptr<ASTNode> child) {
    children_.push_back(child);
}

void ASTNode::removeChild(std::shared_ptr<ASTNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
    }
}

void ASTNode::setAttribute(const std::string& key, const AttributeValue& value) {
    attributes_[key] = value;
}

AttributeValue ASTNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : AttributeValue(std::string(""));
}

bool ASTNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

std::string ASTNode::toString() const {
    std::stringstream ss;
    ss << "ASTNode(" << static_cast<int>(type_) << ", \"" << name_ << "\")";
    return ss.str();
}

std::string ASTNode::toTreeString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << "ASTNode(" << static_cast<int>(type_) << ", \"" << name_ << "\")";
    if (!text_.empty()) {
        ss << " text=\"" << text_ << "\"";
    }
    ss << "\n";
    
    for (const auto& child : children_) {
        ss << child->toTreeString(indent + 1);
    }
    
    return ss.str();
}

// ElementNode实现
ElementNode::ElementNode(const std::string& tagName)
    : ASTNode(ASTNodeType::ELEMENT, tagName), selfClosing_(false) {
}

void ElementNode::accept(ASTVisitor& visitor) {
    visitor.visitElement(*this);
}

bool ElementNode::isSelfClosing() const {
    return selfClosing_;
}

// TextNode实现
TextNode::TextNode(const std::string& content)
    : ASTNode(ASTNodeType::TEXT, "text") {
    setText(content);
}

void TextNode::accept(ASTVisitor& visitor) {
    visitor.visitText(*this);
}

// StyleNode实现
StyleNode::StyleNode()
    : ASTNode(ASTNodeType::STYLE_BLOCK, "style") {
}

void StyleNode::accept(ASTVisitor& visitor) {
    visitor.visitStyle(*this);
}

void StyleNode::addProperty(const std::string& property, const std::string& value) {
    properties_[property] = value;
}

// ScriptNode实现
ScriptNode::ScriptNode()
    : ASTNode(ASTNodeType::SCRIPT_BLOCK, "script") {
}

void ScriptNode::accept(ASTVisitor& visitor) {
    visitor.visitScript(*this);
}

// TemplateNode实现
TemplateNode::TemplateNode(const std::string& templateType, const std::string& templateName)
    : ASTNode(ASTNodeType::TEMPLATE, templateName), 
      templateType_(templateType), templateName_(templateName) {
}

void TemplateNode::accept(ASTVisitor& visitor) {
    visitor.visitTemplate(*this);
}

// CustomNode实现
CustomNode::CustomNode(const std::string& customType, const std::string& customName)
    : ASTNode(ASTNodeType::CUSTOM, customName),
      customType_(customType), customName_(customName) {
}

void CustomNode::accept(ASTVisitor& visitor) {
    visitor.visitCustom(*this);
}

// OriginNode实现
OriginNode::OriginNode(const std::string& originType, const std::string& originName)
    : ASTNode(ASTNodeType::ORIGIN, originName),
      originType_(originType), originName_(originName) {
}

void OriginNode::accept(ASTVisitor& visitor) {
    visitor.visitOrigin(*this);
}

// ImportNode实现
ImportNode::ImportNode(const std::string& importType, const std::string& importPath)
    : ASTNode(ASTNodeType::IMPORT, "import"),
      importType_(importType), importPath_(importPath) {
}

void ImportNode::accept(ASTVisitor& visitor) {
    visitor.visitImport(*this);
}

// NamespaceNode实现
NamespaceNode::NamespaceNode(const std::string& namespaceName)
    : ASTNode(ASTNodeType::NAMESPACE, namespaceName) {
}

void NamespaceNode::accept(ASTVisitor& visitor) {
    visitor.visitNamespace(*this);
}

// ConfigurationNode实现
ConfigurationNode::ConfigurationNode(const std::string& configName)
    : ASTNode(ASTNodeType::CONFIGURATION, configName) {
}

void ConfigurationNode::accept(ASTVisitor& visitor) {
    visitor.visitConfiguration(*this);
}

void ConfigurationNode::addConfigItem(const std::string& key, const std::string& value) {
    configItems_[key] = value;
}

// ConstraintNode实现
ConstraintNode::ConstraintNode()
    : ASTNode(ASTNodeType::CONSTRAINT, "constraint") {
}

void ConstraintNode::accept(ASTVisitor& visitor) {
    visitor.visitConstraint(*this);
}

void ConstraintNode::addException(const std::string& exception) {
    exceptions_.push_back(exception);
}

// UseNode实现
UseNode::UseNode(const std::string& useTarget)
    : ASTNode(ASTNodeType::USE, "use") {
    setText(useTarget);
}

void UseNode::accept(ASTVisitor& visitor) {
    visitor.visitUse(*this);
}

// StyleRuleNode实现
StyleRuleNode::StyleRuleNode()
    : ASTNode(ASTNodeType::STYLE_RULE, "style_rule") {
}

void StyleRuleNode::accept(ASTVisitor& visitor) {
    visitor.visitStyleRule(*this);
}

void StyleRuleNode::addSelector(const std::string& selector) {
    selectors_.push_back(selector);
}

void StyleRuleNode::addDeclaration(const std::string& property, const std::string& value) {
    declarations_[property] = value;
}

// ExpressionNode实现
ExpressionNode::ExpressionNode(const std::string& expression)
    : ASTNode(ASTNodeType::EXPRESSION, "expression"), expressionType_(ExpressionType::LITERAL) {
    setText(expression);
}

void ExpressionNode::accept(ASTVisitor& visitor) {
    visitor.visitExpression(*this);
}

// BinaryOpNode实现
BinaryOpNode::BinaryOpNode(const std::string& operator_, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
    : ASTNode(ASTNodeType::BINARY_OP, "binary_op"), operator_(TokenType::PLUS) {
    setText(operator_);
    addChild(left);
    addChild(right);
}

void BinaryOpNode::accept(ASTVisitor& visitor) {
    visitor.visitBinaryOp(*this);
}

std::shared_ptr<ASTNode> BinaryOpNode::getLeft() const {
    return children_.empty() ? nullptr : children_[0];
}

std::shared_ptr<ASTNode> BinaryOpNode::getRight() const {
    return children_.size() < 2 ? nullptr : children_[1];
}

// ConditionalNode实现
ConditionalNode::ConditionalNode(std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> trueExpr, std::shared_ptr<ASTNode> falseExpr)
    : ASTNode(ASTNodeType::CONDITIONAL, "conditional") {
    addChild(condition);
    addChild(trueExpr);
    addChild(falseExpr);
}

void ConditionalNode::accept(ASTVisitor& visitor) {
    visitor.visitConditional(*this);
}

std::shared_ptr<ASTNode> ConditionalNode::getCondition() const {
    return children_.empty() ? nullptr : children_[0];
}

std::shared_ptr<ASTNode> ConditionalNode::getTrueExpression() const {
    return children_.size() < 2 ? nullptr : children_[1];
}

std::shared_ptr<ASTNode> ConditionalNode::getFalseExpression() const {
    return children_.size() < 3 ? nullptr : children_[2];
}

// ReferenceNode实现
ReferenceNode::ReferenceNode(const std::string& selector, const std::string& property)
    : ASTNode(ASTNodeType::REFERENCE, "reference"), selector_(selector), property_(property) {
}

void ReferenceNode::accept(ASTVisitor& visitor) {
    visitor.visitReference(*this);
}

// SelectorNode实现
SelectorNode::SelectorNode(const std::string& selector)
    : ASTNode(ASTNodeType::SELECTOR, "selector"), selectorType_(SelectorType::TAG) {
    setText(selector);
}

void SelectorNode::accept(ASTVisitor& visitor) {
    visitor.visitSelector(*this);
}

// CHTLJSFunctionNode实现
CHTLJSFunctionNode::CHTLJSFunctionNode(const std::string& functionName)
    : ASTNode(ASTNodeType::CHTLJS_FUNCTION, functionName) {
}

void CHTLJSFunctionNode::accept(ASTVisitor& visitor) {
    visitor.visitCHTLJSFunction(*this);
}

void CHTLJSFunctionNode::addParameter(const std::string& param) {
    parameters_.push_back(param);
}

// CHTLJSVirNode实现
CHTLJSVirNode::CHTLJSVirNode(const std::string& virName)
    : ASTNode(ASTNodeType::CHTLJS_VIR, virName) {
}

void CHTLJSVirNode::accept(ASTVisitor& visitor) {
    visitor.visitCHTLJSVir(*this);
}

} // namespace CHTL