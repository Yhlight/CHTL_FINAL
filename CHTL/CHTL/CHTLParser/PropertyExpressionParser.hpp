#pragma once

#include "../CHTLLexer/PropertyExpressionLexer.hpp"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CSS属性表达式AST节点基类
 */
class PropertyExpressionNode {
public:
    enum class NodeType {
        LITERAL,           // 字面量 (数字、字符串)
        PROPERTY_REF,      // 属性引用 (选择器.属性)
        DYNAMIC_REF,       // 动态引用 ({{变量}}->属性)
        ARITHMETIC,        // 算术运算
        COMPARISON,        // 比较运算
        LOGICAL,           // 逻辑运算
        CONDITIONAL,       // 条件表达式
        CHAINED_CONDITIONAL // 链式条件表达式
    };
    
    PropertyExpressionNode(NodeType type) : type_(type) {}
    virtual ~PropertyExpressionNode() = default;
    
    NodeType getType() const { return type_; }
    virtual std::string toString() const = 0;
    virtual std::string toCSS() const = 0;
    
protected:
    NodeType type_;
};

/**
 * @brief 字面量节点
 */
class LiteralNode : public PropertyExpressionNode {
public:
    LiteralNode(const std::string& value, const std::string& unit = "")
        : PropertyExpressionNode(NodeType::LITERAL), value_(value), unit_(unit) {}
    
    const std::string& getValue() const { return value_; }
    const std::string& getUnit() const { return unit_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    std::string value_;
    std::string unit_;
};

/**
 * @brief 属性引用节点
 */
class PropertyReferenceNode : public PropertyExpressionNode {
public:
    PropertyReferenceNode(const std::string& selector, const std::string& property)
        : PropertyExpressionNode(NodeType::PROPERTY_REF), selector_(selector), property_(property) {}
    
    const std::string& getSelector() const { return selector_; }
    const std::string& getProperty() const { return property_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    std::string selector_;
    std::string property_;
};

/**
 * @brief 动态引用节点
 */
class DynamicReferenceNode : public PropertyExpressionNode {
public:
    DynamicReferenceNode(const std::string& variable, const std::string& property)
        : PropertyExpressionNode(NodeType::DYNAMIC_REF), variable_(variable), property_(property) {}
    
    const std::string& getVariable() const { return variable_; }
    const std::string& getProperty() const { return property_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    std::string variable_;
    std::string property_;
};

/**
 * @brief 算术运算节点
 */
class ArithmeticNode : public PropertyExpressionNode {
public:
    enum class Operator {
        ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, POWER
    };
    
    ArithmeticNode(Operator op, std::shared_ptr<PropertyExpressionNode> left, std::shared_ptr<PropertyExpressionNode> right)
        : PropertyExpressionNode(NodeType::ARITHMETIC), operator_(op), left_(left), right_(right) {}
    
    Operator getOperator() const { return operator_; }
    std::shared_ptr<PropertyExpressionNode> getLeft() const { return left_; }
    std::shared_ptr<PropertyExpressionNode> getRight() const { return right_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    Operator operator_;
    std::shared_ptr<PropertyExpressionNode> left_;
    std::shared_ptr<PropertyExpressionNode> right_;
};

/**
 * @brief 比较运算节点
 */
class ComparisonNode : public PropertyExpressionNode {
public:
    enum class Operator {
        LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL
    };
    
    ComparisonNode(Operator op, std::shared_ptr<PropertyExpressionNode> left, std::shared_ptr<PropertyExpressionNode> right)
        : PropertyExpressionNode(NodeType::COMPARISON), operator_(op), left_(left), right_(right) {}
    
    Operator getOperator() const { return operator_; }
    std::shared_ptr<PropertyExpressionNode> getLeft() const { return left_; }
    std::shared_ptr<PropertyExpressionNode> getRight() const { return right_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    Operator operator_;
    std::shared_ptr<PropertyExpressionNode> left_;
    std::shared_ptr<PropertyExpressionNode> right_;
};

/**
 * @brief 逻辑运算节点
 */
class LogicalNode : public PropertyExpressionNode {
public:
    enum class Operator {
        AND, OR, NOT
    };
    
    LogicalNode(Operator op, std::shared_ptr<PropertyExpressionNode> left, std::shared_ptr<PropertyExpressionNode> right = nullptr)
        : PropertyExpressionNode(NodeType::LOGICAL), operator_(op), left_(left), right_(right) {}
    
    Operator getOperator() const { return operator_; }
    std::shared_ptr<PropertyExpressionNode> getLeft() const { return left_; }
    std::shared_ptr<PropertyExpressionNode> getRight() const { return right_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    Operator operator_;
    std::shared_ptr<PropertyExpressionNode> left_;
    std::shared_ptr<PropertyExpressionNode> right_;
};

/**
 * @brief 条件表达式节点
 */
class ConditionalNode : public PropertyExpressionNode {
public:
    ConditionalNode(std::shared_ptr<PropertyExpressionNode> condition, 
                   std::shared_ptr<PropertyExpressionNode> trueValue, 
                   std::shared_ptr<PropertyExpressionNode> falseValue)
        : PropertyExpressionNode(NodeType::CONDITIONAL), condition_(condition), trueValue_(trueValue), falseValue_(falseValue) {}
    
    std::shared_ptr<PropertyExpressionNode> getCondition() const { return condition_; }
    std::shared_ptr<PropertyExpressionNode> getTrueValue() const { return trueValue_; }
    std::shared_ptr<PropertyExpressionNode> getFalseValue() const { return falseValue_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    std::shared_ptr<PropertyExpressionNode> condition_;
    std::shared_ptr<PropertyExpressionNode> trueValue_;
    std::shared_ptr<PropertyExpressionNode> falseValue_;
};

/**
 * @brief 链式条件表达式节点
 */
class ChainedConditionalNode : public PropertyExpressionNode {
public:
    ChainedConditionalNode(const std::vector<std::pair<std::shared_ptr<PropertyExpressionNode>, std::shared_ptr<PropertyExpressionNode>>>& conditions,
                          std::shared_ptr<PropertyExpressionNode> defaultValue)
        : PropertyExpressionNode(NodeType::CHAINED_CONDITIONAL), conditions_(conditions), defaultValue_(defaultValue) {}
    
    const std::vector<std::pair<std::shared_ptr<PropertyExpressionNode>, std::shared_ptr<PropertyExpressionNode>>>& getConditions() const { return conditions_; }
    std::shared_ptr<PropertyExpressionNode> getDefaultValue() const { return defaultValue_; }
    
    std::string toString() const override;
    std::string toCSS() const override;
    
private:
    std::vector<std::pair<std::shared_ptr<PropertyExpressionNode>, std::shared_ptr<PropertyExpressionNode>>> conditions_;
    std::shared_ptr<PropertyExpressionNode> defaultValue_;
};

/**
 * @brief CSS属性表达式解析器
 */
class PropertyExpressionParser {
public:
    PropertyExpressionParser(const std::string& input);
    
    std::shared_ptr<PropertyExpressionNode> parse();
    
private:
    PropertyExpressionLexer lexer_;
    Token currentToken_;
    
    void nextToken();
    void expectToken(TokenType expected);
    
    std::shared_ptr<PropertyExpressionNode> parseExpression();
    std::shared_ptr<PropertyExpressionNode> parseLogicalOr();
    std::shared_ptr<PropertyExpressionNode> parseLogicalAnd();
    std::shared_ptr<PropertyExpressionNode> parseEquality();
    std::shared_ptr<PropertyExpressionNode> parseRelational();
    std::shared_ptr<PropertyExpressionNode> parseAdditive();
    std::shared_ptr<PropertyExpressionNode> parseMultiplicative();
    std::shared_ptr<PropertyExpressionNode> parseUnary();
    std::shared_ptr<PropertyExpressionNode> parsePrimary();
    std::shared_ptr<PropertyExpressionNode> parseConditional();
    std::shared_ptr<PropertyExpressionNode> parseChainedConditional();
    
    // 辅助方法
    bool isArithmeticOperator(TokenType type) const;
    bool isComparisonOperator(TokenType type) const;
    bool isLogicalOperator(TokenType type) const;
    ArithmeticNode::Operator getArithmeticOperator(TokenType type) const;
    ComparisonNode::Operator getComparisonOperator(TokenType type) const;
    LogicalNode::Operator getLogicalOperator(TokenType type) const;
};

} // namespace CHTL