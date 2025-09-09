#include "OperatorNode.hpp"
#include <sstream>
#include <unordered_map>

namespace CHTL {

OperatorNode::OperatorNode(const std::string& symbol, OperatorType type, 
                           size_t line, size_t column)
    : BaseNode(NodeType::OPERATOR, line, column), operatorType_(type) {
    setValue(symbol);
}

int OperatorNode::getPrecedence() const {
    return getOperatorPrecedence(operatorType_);
}

bool OperatorNode::isLeftAssociative() const {
    return isOperatorLeftAssociative(operatorType_);
}

bool OperatorNode::isAssignment() const {
    switch (operatorType_) {
        case OperatorType::ASSIGN:
        case OperatorType::ADD_ASSIGN:
        case OperatorType::SUBTRACT_ASSIGN:
        case OperatorType::MULTIPLY_ASSIGN:
        case OperatorType::DIVIDE_ASSIGN:
        case OperatorType::MODULO_ASSIGN:
            return true;
        default:
            return false;
    }
}

bool OperatorNode::isArithmetic() const {
    switch (operatorType_) {
        case OperatorType::ADD:
        case OperatorType::SUBTRACT:
        case OperatorType::MULTIPLY:
        case OperatorType::DIVIDE:
        case OperatorType::MODULO:
        case OperatorType::POWER:
        case OperatorType::INCREMENT:
        case OperatorType::DECREMENT:
            return true;
        default:
            return false;
    }
}

bool OperatorNode::isComparison() const {
    switch (operatorType_) {
        case OperatorType::EQUAL:
        case OperatorType::NOT_EQUAL:
        case OperatorType::LESS_THAN:
        case OperatorType::GREATER_THAN:
        case OperatorType::LESS_EQUAL:
        case OperatorType::GREATER_EQUAL:
            return true;
        default:
            return false;
    }
}

bool OperatorNode::isLogical() const {
    switch (operatorType_) {
        case OperatorType::LOGICAL_AND:
        case OperatorType::LOGICAL_OR:
        case OperatorType::LOGICAL_NOT:
            return true;
        default:
            return false;
    }
}

std::string OperatorNode::toHTML() const {
    // 操作符节点通常不直接生成HTML，但可以用于调试
    return "";
}

std::string OperatorNode::toString() const {
    std::ostringstream oss;
    std::string typeStr;
    switch (operatorType_) {
        case OperatorType::ADD: typeStr = "ADD"; break;
        case OperatorType::SUBTRACT: typeStr = "SUBTRACT"; break;
        case OperatorType::MULTIPLY: typeStr = "MULTIPLY"; break;
        case OperatorType::DIVIDE: typeStr = "DIVIDE"; break;
        case OperatorType::MODULO: typeStr = "MODULO"; break;
        case OperatorType::POWER: typeStr = "POWER"; break;
        case OperatorType::EQUAL: typeStr = "EQUAL"; break;
        case OperatorType::NOT_EQUAL: typeStr = "NOT_EQUAL"; break;
        case OperatorType::LESS_THAN: typeStr = "LESS_THAN"; break;
        case OperatorType::GREATER_THAN: typeStr = "GREATER_THAN"; break;
        case OperatorType::LESS_EQUAL: typeStr = "LESS_EQUAL"; break;
        case OperatorType::GREATER_EQUAL: typeStr = "GREATER_EQUAL"; break;
        case OperatorType::LOGICAL_AND: typeStr = "LOGICAL_AND"; break;
        case OperatorType::LOGICAL_OR: typeStr = "LOGICAL_OR"; break;
        case OperatorType::LOGICAL_NOT: typeStr = "LOGICAL_NOT"; break;
        case OperatorType::ASSIGN: typeStr = "ASSIGN"; break;
        case OperatorType::ADD_ASSIGN: typeStr = "ADD_ASSIGN"; break;
        case OperatorType::SUBTRACT_ASSIGN: typeStr = "SUBTRACT_ASSIGN"; break;
        case OperatorType::MULTIPLY_ASSIGN: typeStr = "MULTIPLY_ASSIGN"; break;
        case OperatorType::DIVIDE_ASSIGN: typeStr = "DIVIDE_ASSIGN"; break;
        case OperatorType::MODULO_ASSIGN: typeStr = "MODULO_ASSIGN"; break;
        case OperatorType::DOT: typeStr = "DOT"; break;
        case OperatorType::ARROW: typeStr = "ARROW"; break;
        case OperatorType::QUESTION: typeStr = "QUESTION"; break;
        case OperatorType::COLON: typeStr = "COLON"; break;
        case OperatorType::COMMA: typeStr = "COMMA"; break;
        case OperatorType::SEMICOLON: typeStr = "SEMICOLON"; break;
        case OperatorType::LEFT_PAREN: typeStr = "LEFT_PAREN"; break;
        case OperatorType::RIGHT_PAREN: typeStr = "RIGHT_PAREN"; break;
        case OperatorType::LEFT_BRACKET: typeStr = "LEFT_BRACKET"; break;
        case OperatorType::RIGHT_BRACKET: typeStr = "RIGHT_BRACKET"; break;
        case OperatorType::LEFT_BRACE: typeStr = "LEFT_BRACE"; break;
        case OperatorType::RIGHT_BRACE: typeStr = "RIGHT_BRACE"; break;
        case OperatorType::HASH: typeStr = "HASH"; break;
        case OperatorType::AT: typeStr = "AT"; break;
        case OperatorType::DOUBLE_BRACE_LEFT: typeStr = "DOUBLE_BRACE_LEFT"; break;
        case OperatorType::DOUBLE_BRACE_RIGHT: typeStr = "DOUBLE_BRACE_RIGHT"; break;
        case OperatorType::RANGE: typeStr = "RANGE"; break;
        case OperatorType::INCREMENT: typeStr = "INCREMENT"; break;
        case OperatorType::DECREMENT: typeStr = "DECREMENT"; break;
        case OperatorType::NULL_COALESCE: typeStr = "NULL_COALESCE"; break;
    }
    
    oss << "OperatorNode(type=" << typeStr 
        << ", symbol=\"" << getValue() 
        << "\", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

int OperatorNode::getOperatorPrecedence(OperatorType type) const {
    static const std::unordered_map<OperatorType, int> precedence = {
        // 最高优先级
        {OperatorType::LEFT_PAREN, 20},
        {OperatorType::RIGHT_PAREN, 20},
        {OperatorType::LEFT_BRACKET, 20},
        {OperatorType::RIGHT_BRACKET, 20},
        {OperatorType::DOT, 19},
        {OperatorType::ARROW, 19},
        
        // 一元运算符
        {OperatorType::LOGICAL_NOT, 18},
        {OperatorType::INCREMENT, 18},
        {OperatorType::DECREMENT, 18},
        
        // 幂运算
        {OperatorType::POWER, 17},
        
        // 乘除模运算
        {OperatorType::MULTIPLY, 16},
        {OperatorType::DIVIDE, 16},
        {OperatorType::MODULO, 16},
        
        // 加减运算
        {OperatorType::ADD, 15},
        {OperatorType::SUBTRACT, 15},
        
        // 比较运算
        {OperatorType::LESS_THAN, 14},
        {OperatorType::GREATER_THAN, 14},
        {OperatorType::LESS_EQUAL, 14},
        {OperatorType::GREATER_EQUAL, 14},
        
        // 相等比较
        {OperatorType::EQUAL, 13},
        {OperatorType::NOT_EQUAL, 13},
        
        // 逻辑与
        {OperatorType::LOGICAL_AND, 12},
        
        // 逻辑或
        {OperatorType::LOGICAL_OR, 11},
        
        // 三元运算符
        {OperatorType::QUESTION, 10},
        {OperatorType::COLON, 10},
        
        // 空合并运算符
        {OperatorType::NULL_COALESCE, 9},
        
        // 赋值运算符
        {OperatorType::ASSIGN, 8},
        {OperatorType::ADD_ASSIGN, 8},
        {OperatorType::SUBTRACT_ASSIGN, 8},
        {OperatorType::MULTIPLY_ASSIGN, 8},
        {OperatorType::DIVIDE_ASSIGN, 8},
        {OperatorType::MODULO_ASSIGN, 8},
        
        // 逗号运算符
        {OperatorType::COMMA, 7},
        
        // 分号
        {OperatorType::SEMICOLON, 6},
        
        // 其他符号
        {OperatorType::HASH, 5},
        {OperatorType::AT, 5},
        {OperatorType::DOUBLE_BRACE_LEFT, 5},
        {OperatorType::DOUBLE_BRACE_RIGHT, 5},
        {OperatorType::RANGE, 5},
        {OperatorType::LEFT_BRACE, 5},
        {OperatorType::RIGHT_BRACE, 5}
    };
    
    auto it = precedence.find(type);
    return (it != precedence.end()) ? it->second : 0;
}

bool OperatorNode::isOperatorLeftAssociative(OperatorType type) const {
    switch (type) {
        case OperatorType::ADD:
        case OperatorType::SUBTRACT:
        case OperatorType::MULTIPLY:
        case OperatorType::DIVIDE:
        case OperatorType::MODULO:
        case OperatorType::EQUAL:
        case OperatorType::NOT_EQUAL:
        case OperatorType::LESS_THAN:
        case OperatorType::GREATER_THAN:
        case OperatorType::LESS_EQUAL:
        case OperatorType::GREATER_EQUAL:
        case OperatorType::LOGICAL_AND:
        case OperatorType::LOGICAL_OR:
        case OperatorType::ASSIGN:
        case OperatorType::ADD_ASSIGN:
        case OperatorType::SUBTRACT_ASSIGN:
        case OperatorType::MULTIPLY_ASSIGN:
        case OperatorType::DIVIDE_ASSIGN:
        case OperatorType::MODULO_ASSIGN:
        case OperatorType::COMMA:
            return true;
            
        case OperatorType::POWER:
        case OperatorType::QUESTION:
        case OperatorType::COLON:
        case OperatorType::LOGICAL_NOT:
        case OperatorType::INCREMENT:
        case OperatorType::DECREMENT:
            return false;
            
        default:
            return true;
    }
}

} // namespace CHTL