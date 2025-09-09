#pragma once

#include "BaseNode.hpp"
#include <string>

namespace CHTL {

/**
 * @brief 操作符节点
 * 
 * 表示各种操作符，如算术运算符、比较运算符、逻辑运算符等
 */
class OperatorNode : public BaseNode {
public:
    enum class OperatorType {
        // 算术运算符
        ADD,            // +
        SUBTRACT,       // -
        MULTIPLY,       // *
        DIVIDE,         // /
        MODULO,         // %
        POWER,          // **
        
        // 比较运算符
        EQUAL,          // ==
        NOT_EQUAL,      // !=
        LESS_THAN,      // <
        GREATER_THAN,   // >
        LESS_EQUAL,     // <=
        GREATER_EQUAL,  // >=
        
        // 逻辑运算符
        LOGICAL_AND,    // &&
        LOGICAL_OR,     // ||
        LOGICAL_NOT,    // !
        
        // 赋值运算符
        ASSIGN,         // =
        ADD_ASSIGN,     // +=
        SUBTRACT_ASSIGN,// -=
        MULTIPLY_ASSIGN,// *=
        DIVIDE_ASSIGN,  // /=
        MODULO_ASSIGN,  // %=
        
        // 其他运算符
        DOT,            // .
        ARROW,          // ->
        QUESTION,       // ?
        COLON,          // :
        COMMA,          // ,
        SEMICOLON,      // ;
        LEFT_PAREN,     // (
        RIGHT_PAREN,    // )
        LEFT_BRACKET,   // [
        RIGHT_BRACKET,  // ]
        LEFT_BRACE,     // {
        RIGHT_BRACE,    // }
        HASH,           // #
        AT,             // @
        DOUBLE_BRACE_LEFT,  // {{
        DOUBLE_BRACE_RIGHT, // }}
        
        // 特殊运算符
        RANGE,          // ..
        INCREMENT,      // ++
        DECREMENT,      // --
        NULL_COALESCE   // ??
    };
    
    OperatorNode(const std::string& symbol, OperatorType type, 
                 size_t line = 0, size_t column = 0);
    
    // 获取操作符类型
    OperatorType getOperatorType() const { return operatorType_; }
    
    // 设置操作符类型
    void setOperatorType(OperatorType type) { operatorType_ = type; }
    
    // 获取操作符符号
    const std::string& getSymbol() const { return getValue(); }
    
    // 设置操作符符号
    void setSymbol(const std::string& symbol) { setValue(symbol); }
    
    // 获取优先级
    int getPrecedence() const;
    
    // 获取结合性
    bool isLeftAssociative() const;
    
    // 检查是否为赋值运算符
    bool isAssignment() const;
    
    // 检查是否为算术运算符
    bool isArithmetic() const;
    
    // 检查是否为比较运算符
    bool isComparison() const;
    
    // 检查是否为逻辑运算符
    bool isLogical() const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    OperatorType operatorType_;
    
    // 获取操作符优先级
    int getOperatorPrecedence(OperatorType type) const;
    
    // 检查操作符结合性
    bool isOperatorLeftAssociative(OperatorType type) const;
};

} // namespace CHTL