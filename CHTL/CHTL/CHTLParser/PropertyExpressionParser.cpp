#include "PropertyExpressionParser.hpp"
#include <sstream>
#include <stdexcept>

namespace CHTL {

// LiteralNode 实现
std::string LiteralNode::toString() const {
    return value_ + unit_;
}

std::string LiteralNode::toCSS() const {
    return value_ + unit_;
}

// PropertyReferenceNode 实现
std::string PropertyReferenceNode::toString() const {
    return selector_ + "." + property_;
}

std::string PropertyReferenceNode::toCSS() const {
    // 在CSS中，属性引用需要转换为calc()函数
    return "calc(var(--" + selector_ + "-" + property_ + "))";
}

// DynamicReferenceNode 实现
std::string DynamicReferenceNode::toString() const {
    return "{{" + variable_ + "}}->" + property_;
}

std::string DynamicReferenceNode::toCSS() const {
    // 动态引用在CSS中需要特殊处理
    return "var(--" + variable_ + "-" + property_ + ")";
}

// ArithmeticNode 实现
std::string ArithmeticNode::toString() const {
    std::string opStr;
    switch (operator_) {
        case Operator::ADD: opStr = "+"; break;
        case Operator::SUBTRACT: opStr = "-"; break;
        case Operator::MULTIPLY: opStr = "*"; break;
        case Operator::DIVIDE: opStr = "/"; break;
        case Operator::MODULO: opStr = "%"; break;
        case Operator::POWER: opStr = "**"; break;
    }
    return "(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
}

std::string ArithmeticNode::toCSS() const {
    std::string opStr;
    switch (operator_) {
        case Operator::ADD: opStr = "+"; break;
        case Operator::SUBTRACT: opStr = "-"; break;
        case Operator::MULTIPLY: opStr = "*"; break;
        case Operator::DIVIDE: opStr = "/"; break;
        case Operator::MODULO: opStr = "%"; break;
        case Operator::POWER: opStr = "**"; break;
    }
    return "calc(" + left_->toCSS() + " " + opStr + " " + right_->toCSS() + ")";
}

// ComparisonNode 实现
std::string ComparisonNode::toString() const {
    std::string opStr;
    switch (operator_) {
        case Operator::LESS_THAN: opStr = "<"; break;
        case Operator::GREATER_THAN: opStr = ">"; break;
        case Operator::LESS_EQUAL: opStr = "<="; break;
        case Operator::GREATER_EQUAL: opStr = ">="; break;
        case Operator::EQUAL: opStr = "=="; break;
        case Operator::NOT_EQUAL: opStr = "!="; break;
    }
    return "(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
}

std::string ComparisonNode::toCSS() const {
    // 比较运算在CSS中需要特殊处理，通常用于条件表达式
    return left_->toCSS() + " " + toString() + " " + right_->toCSS();
}

// LogicalNode 实现
std::string LogicalNode::toString() const {
    std::string opStr;
    switch (operator_) {
        case Operator::AND: opStr = "&&"; break;
        case Operator::OR: opStr = "||"; break;
        case Operator::NOT: opStr = "!"; break;
    }
    if (operator_ == Operator::NOT) {
        return "(" + opStr + left_->toString() + ")";
    } else {
        return "(" + left_->toString() + " " + opStr + " " + right_->toString() + ")";
    }
}

std::string LogicalNode::toCSS() const {
    // 逻辑运算在CSS中需要特殊处理
    return toString();
}

// ConditionalNode 实现
std::string ConditionalNode::toString() const {
    return "(" + condition_->toString() + " ? " + trueValue_->toString() + " : " + falseValue_->toString() + ")";
}

std::string ConditionalNode::toCSS() const {
    // 条件表达式在CSS中需要特殊处理
    return "(" + condition_->toCSS() + " ? " + trueValue_->toCSS() + " : " + falseValue_->toCSS() + ")";
}

// ChainedConditionalNode 实现
std::string ChainedConditionalNode::toString() const {
    std::string result;
    for (size_t i = 0; i < conditions_.size(); ++i) {
        if (i > 0) result += ", ";
        result += conditions_[i].first->toString() + " ? " + conditions_[i].second->toString();
    }
    if (defaultValue_) {
        result += " : " + defaultValue_->toString();
    }
    return "(" + result + ")";
}

std::string ChainedConditionalNode::toCSS() const {
    // 链式条件表达式在CSS中需要特殊处理
    return toString();
}

// PropertyExpressionParser 实现
PropertyExpressionParser::PropertyExpressionParser(const std::string& input)
    : lexer_(input) {
    nextToken();
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parse() {
    return parseExpression();
}

void PropertyExpressionParser::nextToken() {
    currentToken_ = lexer_.nextToken();
}

void PropertyExpressionParser::expectToken(TokenType expected) {
    if (currentToken_.getType() != expected) {
        throw std::runtime_error("Expected token type " + std::to_string(static_cast<int>(expected)) + 
                                " but got " + std::to_string(static_cast<int>(currentToken_.getType())));
    }
    nextToken();
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseExpression() {
    return parseLogicalOr();
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (currentToken_.getType() == TokenType::LOGICAL_OR) {
        nextToken();
        auto right = parseLogicalAnd();
        left = std::make_shared<LogicalNode>(LogicalNode::Operator::OR, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseLogicalAnd() {
    auto left = parseEquality();
    
    while (currentToken_.getType() == TokenType::LOGICAL_AND) {
        nextToken();
        auto right = parseEquality();
        left = std::make_shared<LogicalNode>(LogicalNode::Operator::AND, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseEquality() {
    auto left = parseRelational();
    
    while (currentToken_.getType() == TokenType::EQUAL_EQUAL || 
           currentToken_.getType() == TokenType::NOT_EQUAL) {
        auto op = getComparisonOperator(currentToken_.getType());
        nextToken();
        auto right = parseRelational();
        left = std::make_shared<ComparisonNode>(op, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseRelational() {
    auto left = parseAdditive();
    
    while (currentToken_.getType() == TokenType::LESS_THAN ||
           currentToken_.getType() == TokenType::GREATER_THAN ||
           currentToken_.getType() == TokenType::LESS_EQUAL ||
           currentToken_.getType() == TokenType::GREATER_EQUAL) {
        auto op = getComparisonOperator(currentToken_.getType());
        nextToken();
        auto right = parseAdditive();
        left = std::make_shared<ComparisonNode>(op, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseAdditive() {
    auto left = parseMultiplicative();
    
    while (currentToken_.getType() == TokenType::PLUS || 
           currentToken_.getType() == TokenType::MINUS) {
        auto op = getArithmeticOperator(currentToken_.getType());
        nextToken();
        auto right = parseMultiplicative();
        left = std::make_shared<ArithmeticNode>(op, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseMultiplicative() {
    auto left = parseUnary();
    
    while (currentToken_.getType() == TokenType::MULTIPLY || 
           currentToken_.getType() == TokenType::DIVIDE ||
           currentToken_.getType() == TokenType::MODULO) {
        auto op = getArithmeticOperator(currentToken_.getType());
        nextToken();
        auto right = parseUnary();
        left = std::make_shared<ArithmeticNode>(op, left, right);
    }
    
    return left;
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseUnary() {
    if (currentToken_.getType() == TokenType::LOGICAL_NOT) {
        nextToken();
        auto operand = parseUnary();
        return std::make_shared<LogicalNode>(LogicalNode::Operator::NOT, operand);
    }
    
    if (currentToken_.getType() == TokenType::MINUS) {
        nextToken();
        auto operand = parseUnary();
        return std::make_shared<ArithmeticNode>(ArithmeticNode::Operator::SUBTRACT, 
                                               std::make_shared<LiteralNode>("0"), operand);
    }
    
    return parsePrimary();
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parsePrimary() {
    if (currentToken_.getType() == TokenType::NUMBER) {
        auto value = currentToken_.getValue();
        nextToken();
        return std::make_shared<LiteralNode>(value);
    }
    
    if (currentToken_.getType() == TokenType::STRING_LITERAL) {
        auto value = currentToken_.getValue();
        nextToken();
        return std::make_shared<LiteralNode>(value);
    }
    
    if (currentToken_.getType() == TokenType::IDENTIFIER) {
        auto value = currentToken_.getValue();
        nextToken();
        
        // 检查是否是属性引用
        if (currentToken_.getType() == TokenType::DOT) {
            nextToken();
            if (currentToken_.getType() == TokenType::IDENTIFIER) {
                auto property = currentToken_.getValue();
                nextToken();
                return std::make_shared<PropertyReferenceNode>(value, property);
            }
        }
        
        // 检查是否是动态引用
        if (value.find("{{") != std::string::npos && value.find("}}") != std::string::npos) {
            auto arrowPos = value.find("->");
            if (arrowPos != std::string::npos) {
                auto variable = value.substr(0, arrowPos);
                auto property = value.substr(arrowPos + 2);
                return std::make_shared<DynamicReferenceNode>(variable, property);
            }
        }
        
        return std::make_shared<LiteralNode>(value);
    }
    
    if (currentToken_.getType() == TokenType::LEFT_PAREN) {
        nextToken();
        auto expr = parseExpression();
        expectToken(TokenType::RIGHT_PAREN);
        return expr;
    }
    
    throw std::runtime_error("Unexpected token: " + currentToken_.getValue());
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseConditional() {
    auto condition = parseExpression();
    expectToken(TokenType::QUESTION);
    auto trueValue = parseExpression();
    expectToken(TokenType::COLON);
    auto falseValue = parseExpression();
    return std::make_shared<ConditionalNode>(condition, trueValue, falseValue);
}

std::shared_ptr<PropertyExpressionNode> PropertyExpressionParser::parseChainedConditional() {
    std::vector<std::pair<std::shared_ptr<PropertyExpressionNode>, std::shared_ptr<PropertyExpressionNode>>> conditions;
    
    while (currentToken_.getType() != TokenType::COLON && currentToken_.getType() != TokenType::END_OF_FILE) {
        auto condition = parseExpression();
        expectToken(TokenType::QUESTION);
        auto value = parseExpression();
        conditions.push_back({condition, value});
        
        if (currentToken_.getType() == TokenType::COMMA) {
            nextToken();
        } else {
            break;
        }
    }
    
    std::shared_ptr<PropertyExpressionNode> defaultValue = nullptr;
    if (currentToken_.getType() == TokenType::COLON) {
        nextToken();
        defaultValue = parseExpression();
    }
    
    return std::make_shared<ChainedConditionalNode>(conditions, defaultValue);
}

bool PropertyExpressionParser::isArithmeticOperator(TokenType type) const {
    return type == TokenType::PLUS || type == TokenType::MINUS || 
           type == TokenType::MULTIPLY || type == TokenType::DIVIDE ||
           type == TokenType::MODULO || type == TokenType::POWER;
}

bool PropertyExpressionParser::isComparisonOperator(TokenType type) const {
    return type == TokenType::LESS_THAN || type == TokenType::GREATER_THAN ||
           type == TokenType::LESS_EQUAL || type == TokenType::GREATER_EQUAL ||
           type == TokenType::EQUAL_EQUAL || type == TokenType::NOT_EQUAL;
}

bool PropertyExpressionParser::isLogicalOperator(TokenType type) const {
    return type == TokenType::LOGICAL_AND || type == TokenType::LOGICAL_OR || type == TokenType::LOGICAL_NOT;
}

ArithmeticNode::Operator PropertyExpressionParser::getArithmeticOperator(TokenType type) const {
    switch (type) {
        case TokenType::PLUS: return ArithmeticNode::Operator::ADD;
        case TokenType::MINUS: return ArithmeticNode::Operator::SUBTRACT;
        case TokenType::MULTIPLY: return ArithmeticNode::Operator::MULTIPLY;
        case TokenType::DIVIDE: return ArithmeticNode::Operator::DIVIDE;
        case TokenType::MODULO: return ArithmeticNode::Operator::MODULO;
        case TokenType::POWER: return ArithmeticNode::Operator::POWER;
        default: throw std::runtime_error("Invalid arithmetic operator");
    }
}

ComparisonNode::Operator PropertyExpressionParser::getComparisonOperator(TokenType type) const {
    switch (type) {
        case TokenType::LESS_THAN: return ComparisonNode::Operator::LESS_THAN;
        case TokenType::GREATER_THAN: return ComparisonNode::Operator::GREATER_THAN;
        case TokenType::LESS_EQUAL: return ComparisonNode::Operator::LESS_EQUAL;
        case TokenType::GREATER_EQUAL: return ComparisonNode::Operator::GREATER_EQUAL;
        case TokenType::EQUAL_EQUAL: return ComparisonNode::Operator::EQUAL;
        case TokenType::NOT_EQUAL: return ComparisonNode::Operator::NOT_EQUAL;
        default: throw std::runtime_error("Invalid comparison operator");
    }
}

LogicalNode::Operator PropertyExpressionParser::getLogicalOperator(TokenType type) const {
    switch (type) {
        case TokenType::LOGICAL_AND: return LogicalNode::Operator::AND;
        case TokenType::LOGICAL_OR: return LogicalNode::Operator::OR;
        case TokenType::LOGICAL_NOT: return LogicalNode::Operator::NOT;
        default: throw std::runtime_error("Invalid logical operator");
    }
}

} // namespace CHTL