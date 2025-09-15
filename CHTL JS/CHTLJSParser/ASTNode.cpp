#include "ASTNode.h"
#include <sstream>

namespace CHTLJS {

// ASTNode基类实现
ASTNode::ASTNode(NodeType type) : type(type) {
}

ASTNode::NodeType ASTNode::getType() const {
    return type;
}

// ProgramNode实现
ProgramNode::ProgramNode() : ASTNode(NodeType::PROGRAM) {
}

void ProgramNode::addStatement(std::shared_ptr<ASTNode> statement) {
    statements.push_back(statement);
}

std::vector<std::shared_ptr<ASTNode>> ProgramNode::getStatements() const {
    return statements;
}

std::string ProgramNode::toString() const {
    std::ostringstream oss;
    oss << "ProgramNode(" << statements.size() << " statements)";
    return oss.str();
}

std::string ProgramNode::generateCode() const {
    std::ostringstream oss;
    for (const auto& statement : statements) {
        oss << statement->generateCode() << "\n";
    }
    return oss.str();
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& name) 
    : ASTNode(NodeType::IDENTIFIER), name(name) {
}

std::string IdentifierNode::getName() const {
    return name;
}

std::string IdentifierNode::toString() const {
    return "IdentifierNode(" + name + ")";
}

std::string IdentifierNode::generateCode() const {
    return name;
}

// LiteralNode实现
LiteralNode::LiteralNode(const std::string& value, const std::string& type)
    : ASTNode(NodeType::LITERAL), value(value), literalType(type) {
}

std::string LiteralNode::getValue() const {
    return value;
}

std::string LiteralNode::getLiteralType() const {
    return literalType;
}

std::string LiteralNode::toString() const {
    return "LiteralNode(" + value + ", " + literalType + ")";
}

std::string LiteralNode::generateCode() const {
    if (literalType == "string") {
        return "\"" + value + "\"";
    }
    return value;
}

// ObjectLiteralNode实现
ObjectLiteralNode::ObjectLiteralNode() : ASTNode(NodeType::OBJECT_LITERAL) {
}

void ObjectLiteralNode::addProperty(const std::string& key, std::shared_ptr<ASTNode> value) {
    properties[key] = value;
}

std::map<std::string, std::shared_ptr<ASTNode>> ObjectLiteralNode::getProperties() const {
    return properties;
}

std::string ObjectLiteralNode::toString() const {
    std::ostringstream oss;
    oss << "ObjectLiteralNode(" << properties.size() << " properties)";
    return oss.str();
}

std::string ObjectLiteralNode::generateCode() const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : properties) {
        if (!first) oss << ", ";
        oss << pair.first << ": " << pair.second->generateCode();
        first = false;
    }
    oss << "}";
    return oss.str();
}

// FunctionCallNode实现
FunctionCallNode::FunctionCallNode(const std::string& functionName)
    : ASTNode(NodeType::FUNCTION_CALL), functionName(functionName) {
}

void FunctionCallNode::addArgument(std::shared_ptr<ASTNode> argument) {
    arguments.push_back(argument);
}

std::string FunctionCallNode::getFunctionName() const {
    return functionName;
}

std::vector<std::shared_ptr<ASTNode>> FunctionCallNode::getArguments() const {
    return arguments;
}

std::string FunctionCallNode::toString() const {
    return "FunctionCallNode(" + functionName + ", " + std::to_string(arguments.size()) + " args)";
}

std::string FunctionCallNode::generateCode() const {
    std::ostringstream oss;
    oss << functionName << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->generateCode();
    }
    oss << ")";
    return oss.str();
}

// VirtualObjectNode实现
VirtualObjectNode::VirtualObjectNode(const std::string& name, std::shared_ptr<ObjectLiteralNode> definition)
    : ASTNode(NodeType::VIRTUAL_OBJECT), name(name), definition(definition) {
}

std::string VirtualObjectNode::getName() const {
    return name;
}

std::shared_ptr<ObjectLiteralNode> VirtualObjectNode::getDefinition() const {
    return definition;
}

std::string VirtualObjectNode::toString() const {
    return "VirtualObjectNode(" + name + ")";
}

std::string VirtualObjectNode::generateCode() const {
    // 虚对象在编译时会被替换为实际的函数引用
    return "/* Virtual Object: " + name + " */";
}

// ListenNode实现
ListenNode::ListenNode(std::shared_ptr<ObjectLiteralNode> events)
    : ASTNode(NodeType::LISTEN_EXPRESSION), events(events) {
}

std::shared_ptr<ObjectLiteralNode> ListenNode::getEvents() const {
    return events;
}

std::string ListenNode::toString() const {
    return "ListenNode";
}

std::string ListenNode::generateCode() const {
    std::ostringstream oss;
    oss << "Listen(" << events->generateCode() << ")";
    return oss.str();
}

// AnimateNode实现
AnimateNode::AnimateNode(std::shared_ptr<ObjectLiteralNode> properties)
    : ASTNode(NodeType::ANIMATE_EXPRESSION), properties(properties) {
}

std::shared_ptr<ObjectLiteralNode> AnimateNode::getProperties() const {
    return properties;
}

std::string AnimateNode::toString() const {
    return "AnimateNode";
}

std::string AnimateNode::generateCode() const {
    std::ostringstream oss;
    oss << "Animate(" << properties->generateCode() << ")";
    return oss.str();
}

// RouterNode实现
RouterNode::RouterNode(std::shared_ptr<ObjectLiteralNode> properties)
    : ASTNode(NodeType::ROUTER_EXPRESSION), properties(properties) {
}

std::shared_ptr<ObjectLiteralNode> RouterNode::getProperties() const {
    return properties;
}

std::string RouterNode::toString() const {
    return "RouterNode";
}

std::string RouterNode::generateCode() const {
    std::ostringstream oss;
    oss << "Router(" << properties->generateCode() << ")";
    return oss.str();
}

// ScriptLoaderNode实现
ScriptLoaderNode::ScriptLoaderNode(std::shared_ptr<ObjectLiteralNode> properties)
    : ASTNode(NodeType::SCRIPT_LOADER_EXPRESSION), properties(properties) {
}

std::shared_ptr<ObjectLiteralNode> ScriptLoaderNode::getProperties() const {
    return properties;
}

std::string ScriptLoaderNode::toString() const {
    return "ScriptLoaderNode";
}

std::string ScriptLoaderNode::generateCode() const {
    std::ostringstream oss;
    oss << "ScriptLoader(" << properties->generateCode() << ")";
    return oss.str();
}

// DelegateNode实现
DelegateNode::DelegateNode(std::shared_ptr<ObjectLiteralNode> properties)
    : ASTNode(NodeType::DELEGATE_EXPRESSION), properties(properties) {
}

std::shared_ptr<ObjectLiteralNode> DelegateNode::getProperties() const {
    return properties;
}

std::string DelegateNode::toString() const {
    return "DelegateNode";
}

std::string DelegateNode::generateCode() const {
    std::ostringstream oss;
    oss << "Delegate(" << properties->generateCode() << ")";
    return oss.str();
}

// INeverAwayNode实现
INeverAwayNode::INeverAwayNode(std::shared_ptr<ObjectLiteralNode> functions)
    : ASTNode(NodeType::INEVERAWAY_EXPRESSION), functions(functions) {
}

std::shared_ptr<ObjectLiteralNode> INeverAwayNode::getFunctions() const {
    return functions;
}

std::string INeverAwayNode::toString() const {
    return "INeverAwayNode";
}

std::string INeverAwayNode::generateCode() const {
    std::ostringstream oss;
    oss << "iNeverAway(" << functions->generateCode() << ")";
    return oss.str();
}

// UtilExpressionNode实现
UtilExpressionNode::UtilExpressionNode(std::shared_ptr<ASTNode> condition,
                                     std::shared_ptr<ASTNode> changeBlock,
                                     std::shared_ptr<ASTNode> thenBlock)
    : ASTNode(NodeType::UTIL_EXPRESSION), condition(condition), 
      changeBlock(changeBlock), thenBlock(thenBlock) {
}

std::shared_ptr<ASTNode> UtilExpressionNode::getCondition() const {
    return condition;
}

std::shared_ptr<ASTNode> UtilExpressionNode::getChangeBlock() const {
    return changeBlock;
}

std::shared_ptr<ASTNode> UtilExpressionNode::getThenBlock() const {
    return thenBlock;
}

std::string UtilExpressionNode::toString() const {
    return "UtilExpressionNode";
}

std::string UtilExpressionNode::generateCode() const {
    std::ostringstream oss;
    oss << "util " << condition->generateCode() 
        << " -> change " << changeBlock->generateCode()
        << " -> then " << thenBlock->generateCode();
    return oss.str();
}

// SelectorNode实现
SelectorNode::SelectorNode(const std::string& selector)
    : ASTNode(NodeType::SELECTOR_EXPRESSION), selector(selector) {
}

std::string SelectorNode::getSelector() const {
    return selector;
}

std::string SelectorNode::toString() const {
    return "SelectorNode(" + selector + ")";
}

std::string SelectorNode::generateCode() const {
    return "{{" + selector + "}}";
}

// EventBindingNode实现
EventBindingNode::EventBindingNode(std::shared_ptr<SelectorNode> selector,
                                 const std::string& eventType,
                                 std::shared_ptr<ASTNode> handler)
    : ASTNode(NodeType::EVENT_BINDING), selector(selector), 
      eventType(eventType), handler(handler) {
}

std::shared_ptr<SelectorNode> EventBindingNode::getSelector() const {
    return selector;
}

std::string EventBindingNode::getEventType() const {
    return eventType;
}

std::shared_ptr<ASTNode> EventBindingNode::getHandler() const {
    return handler;
}

std::string EventBindingNode::toString() const {
    return "EventBindingNode(" + eventType + ")";
}

std::string EventBindingNode::generateCode() const {
    return selector->generateCode() + " &-> " + eventType + ": " + handler->generateCode();
}

// PropertyAccessNode实现
PropertyAccessNode::PropertyAccessNode(std::shared_ptr<ASTNode> object, const std::string& property)
    : ASTNode(NodeType::PROPERTY_ACCESS), object(object), property(property) {
}

std::shared_ptr<ASTNode> PropertyAccessNode::getObject() const {
    return object;
}

std::string PropertyAccessNode::getProperty() const {
    return property;
}

std::string PropertyAccessNode::toString() const {
    return "PropertyAccessNode(" + property + ")";
}

std::string PropertyAccessNode::generateCode() const {
    return object->generateCode() + "->" + property;
}

} // namespace CHTLJS