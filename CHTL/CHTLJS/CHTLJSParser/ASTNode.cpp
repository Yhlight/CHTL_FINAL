#include "ASTNode.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

std::string ASTNode::toString() const {
    std::ostringstream oss;
    oss << "ASTNode{type=" << static_cast<int>(type) 
        << ", token=" << token.toString() << "}";
    return oss.str();
}

void ASTNode::addChild(std::shared_ptr<ASTNode> child) {
    if (child) {
        children.push_back(child);
    }
}

void ASTNode::setProperty(const std::string& key, std::shared_ptr<ASTNode> value) {
    if (value) {
        properties[key] = value;
    }
}

std::shared_ptr<ASTNode> ASTNode::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return nullptr;
}

bool ASTNode::hasChildren() const {
    return !children.empty();
}

size_t ASTNode::getChildCount() const {
    return children.size();
}

std::shared_ptr<ASTNode> ASTNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

bool ASTNode::hasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

std::vector<std::string> ASTNode::getPropertyKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : properties) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ProgramNode::toCode() const {
    std::ostringstream oss;
    for (const auto& child : children) {
        if (child) {
            oss << child->toCode();
            if (child->type != ASTNodeType::BLOCK_STATEMENT) {
                oss << ";";
            }
            oss << "\n";
        }
    }
    return oss.str();
}

std::string VariableDeclarationNode::toCode() const {
    std::ostringstream oss;
    oss << variableType << " " << name;
    if (initializer) {
        oss << " = " << initializer->toCode();
    }
    return oss.str();
}

std::string FunctionDeclarationNode::toCode() const {
    std::ostringstream oss;
    oss << "function " << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i];
    }
    oss << ") ";
    if (body) {
        oss << body->toCode();
    }
    return oss.str();
}

std::string BinaryExpressionNode::toCode() const {
    std::ostringstream oss;
    if (left) oss << left->toCode();
    
    switch (operatorType) {
        case TokenType::PLUS: oss << " + "; break;
        case TokenType::MINUS: oss << " - "; break;
        case TokenType::MULTIPLY: oss << " * "; break;
        case TokenType::DIVIDE: oss << " / "; break;
        case TokenType::MODULO: oss << " % "; break;
        case TokenType::POWER: oss << " ** "; break;
        case TokenType::EQUAL: oss << " == "; break;
        case TokenType::NOT_EQUAL: oss << " != "; break;
        case TokenType::LESS: oss << " < "; break;
        case TokenType::GREATER: oss << " > "; break;
        case TokenType::LESS_EQUAL: oss << " <= "; break;
        case TokenType::GREATER_EQUAL: oss << " >= "; break;
        case TokenType::AND: oss << " && "; break;
        case TokenType::OR: oss << " || "; break;
        default: oss << " ? "; break;
    }
    
    if (right) oss << right->toCode();
    return oss.str();
}

std::string CallExpressionNode::toCode() const {
    std::ostringstream oss;
    if (callee) oss << callee->toCode();
    oss << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        if (arguments[i]) oss << arguments[i]->toCode();
    }
    oss << ")";
    return oss.str();
}

std::string MemberExpressionNode::toCode() const {
    std::ostringstream oss;
    if (object) oss << object->toCode();
    if (computed) {
        oss << "[";
        if (property) oss << property->toCode();
        oss << "]";
    } else {
        oss << ".";
        if (property) oss << property->toCode();
    }
    return oss.str();
}

std::string LiteralNode::toCode() const {
    switch (literalType) {
        case TokenType::STRING:
            return "\"" + value + "\"";
        case TokenType::BOOLEAN:
            return value;
        default:
            return value;
    }
}

std::string IdentifierNode::toCode() const {
    return name;
}

std::string VirDeclarationNode::toCode() const {
    std::ostringstream oss;
    oss << "const " << name << " = ";
    if (value) {
        oss << value->toCode();
    }
    return oss.str();
}

std::string ListenExpressionNode::toCode() const {
    std::ostringstream oss;
    oss << "Listen({";
    bool first = true;
    for (const auto& pair : events) {
        if (!first) oss << ", ";
        oss << pair.first << ": ";
        if (pair.second) oss << pair.second->toCode();
        first = false;
    }
    oss << "})";
    return oss.str();
}

std::string AnimateExpressionNode::toCode() const {
    std::ostringstream oss;
    oss << "Animate({";
    
    if (target) {
        oss << "target: " << target->toCode() << ", ";
    }
    if (duration) {
        oss << "duration: " << duration->toCode() << ", ";
    }
    if (easing) {
        oss << "easing: " << easing->toCode() << ", ";
    }
    if (begin) {
        oss << "begin: " << begin->toCode() << ", ";
    }
    if (!when.empty()) {
        oss << "when: [";
        for (size_t i = 0; i < when.size(); ++i) {
            if (i > 0) oss << ", ";
            if (when[i]) oss << when[i]->toCode();
        }
        oss << "], ";
    }
    if (end) {
        oss << "end: " << end->toCode() << ", ";
    }
    if (loop) {
        oss << "loop: " << loop->toCode() << ", ";
    }
    if (direction) {
        oss << "direction: " << direction->toCode() << ", ";
    }
    if (delay) {
        oss << "delay: " << delay->toCode() << ", ";
    }
    if (callback) {
        oss << "callback: " << callback->toCode() << ", ";
    }
    
    oss << "})";
    return oss.str();
}

std::string SelectorExpressionNode::toCode() const {
    return "{{" + selector + "}}";
}

std::string ReactiveValueExpressionNode::toCode() const {
    return "$" + variableName + "$";
}

} // namespace CHTLJS