#include "ExpressionNode.h"
#include <sstream>
#include <regex>
#include <cmath>

namespace CHTL {

ExpressionNode::ExpressionNode(ExpressionType type, const std::string& expression) 
    : BaseNode(NodeType::TEXT), m_expressionType(type) {
    setExpression(expression);
}

std::string ExpressionNode::evaluate() const {
    switch (m_expressionType) {
        case ExpressionType::ARITHMETIC:
            return evaluateArithmetic();
        case ExpressionType::LOGICAL:
            return evaluateLogical();
        case ExpressionType::CONDITIONAL:
            return evaluateConditional();
        case ExpressionType::REFERENCE:
            return evaluateReference();
        default:
            return m_value;
    }
}

std::shared_ptr<BaseNode> ExpressionNode::clone() const {
    auto cloned = std::make_shared<ExpressionNode>(m_expressionType, m_value);
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ExpressionNode::toString() const {
    return evaluate();
}

void ExpressionNode::accept(NodeVisitor& visitor) {
    visitor.visitExpression(*this);
}

std::string ExpressionNode::evaluateArithmetic() const {
    std::string expr = m_value;
    
    // 简单的算术表达式计算
    // 支持 + - * / % ** 运算符
    
    // 处理幂运算 **
    std::regex powerRegex(R"((\d+(?:\.\d+)?)\s*\*\*\s*(\d+(?:\.\d+)?))");
    std::smatch match;
    while (std::regex_search(expr, match, powerRegex)) {
        double base = std::stod(match[1].str());
        double exponent = std::stod(match[2].str());
        double result = std::pow(base, exponent);
        expr = match.prefix().str() + std::to_string(result) + match.suffix().str();
    }
    
    // 处理乘除模运算
    std::regex mulDivModRegex(R"((\d+(?:\.\d+)?)\s*([*/%])\s*(\d+(?:\.\d+)?))");
    while (std::regex_search(expr, match, mulDivModRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        double result = 0;
        
        if (match[2].str() == "*") {
            result = left * right;
        } else if (match[2].str() == "/") {
            result = right != 0 ? left / right : 0;
        } else if (match[2].str() == "%") {
            result = std::fmod(left, right);
        }
        
        expr = match.prefix().str() + std::to_string(result) + match.suffix().str();
    }
    
    // 处理加减运算
    std::regex addSubRegex(R"((\d+(?:\.\d+)?)\s*([+-])\s*(\d+(?:\.\d+)?))");
    while (std::regex_search(expr, match, addSubRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        double result = 0;
        
        if (match[2].str() == "+") {
            result = left + right;
        } else if (match[2].str() == "-") {
            result = left - right;
        }
        
        expr = match.prefix().str() + std::to_string(result) + match.suffix().str();
    }
    
    return expr;
}

std::string ExpressionNode::evaluateLogical() const {
    // 简单的逻辑表达式计算
    // 支持 && || 运算符
    std::string expr = m_value;
    
    // 处理逻辑与
    std::regex andRegex(R"((\w+)\s*&&\s*(\w+))");
    std::smatch match;
    while (std::regex_search(expr, match, andRegex)) {
        bool left = match[1].str() == "true" || match[1].str() != "0";
        bool right = match[2].str() == "true" || match[2].str() != "0";
        bool result = left && right;
        expr = match.prefix().str() + (result ? "true" : "false") + match.suffix().str();
    }
    
    // 处理逻辑或
    std::regex orRegex(R"((\w+)\s*\|\|\s*(\w+))");
    while (std::regex_search(expr, match, orRegex)) {
        bool left = match[1].str() == "true" || match[1].str() != "0";
        bool right = match[2].str() == "true" || match[2].str() != "0";
        bool result = left || right;
        expr = match.prefix().str() + (result ? "true" : "false") + match.suffix().str();
    }
    
    return expr;
}

std::string ExpressionNode::evaluateConditional() const {
    // 简单的条件表达式计算
    // 支持 ? : 三元运算符
    std::string expr = m_value;
    
    std::regex conditionalRegex(R"((\w+)\s*\?\s*([^:]+)\s*:\s*([^?]+))");
    std::smatch match;
    while (std::regex_search(expr, match, conditionalRegex)) {
        bool condition = match[1].str() == "true" || match[1].str() != "0";
        std::string trueValue = match[2].str();
        std::string falseValue = match[3].str();
        
        // 去除空白字符
        trueValue.erase(0, trueValue.find_first_not_of(" \t"));
        trueValue.erase(trueValue.find_last_not_of(" \t") + 1);
        falseValue.erase(0, falseValue.find_first_not_of(" \t"));
        falseValue.erase(falseValue.find_last_not_of(" \t") + 1);
        
        std::string result = condition ? trueValue : falseValue;
        expr = match.prefix().str() + result + match.suffix().str();
    }
    
    return expr;
}

std::string ExpressionNode::evaluateReference() const {
    // 引用表达式计算
    // 支持 CSS选择器.属性 语法
    return m_value; // 暂时返回原值，后续实现引用解析
}

} // namespace CHTL