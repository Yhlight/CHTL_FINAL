#ifndef CHTLJSDYNAMICEXPRESSIONS_H
#define CHTLJSDYNAMICEXPRESSIONS_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Dynamic expression types
enum class DynamicExpressionType {
    Literal,
    Variable,
    BinaryOperation,
    UnaryOperation,
    FunctionCall,
    PropertyAccess,
    ArrayAccess,
    Conditional,
    Comparison,
    Logical
};

// Dynamic expression node
struct DynamicExpressionNode {
    DynamicExpressionType type;
    std::string value;
    std::vector<std::shared_ptr<DynamicExpressionNode>> children;
    std::map<std::string, std::string> metadata;
    
    DynamicExpressionNode(DynamicExpressionType t, const std::string& v = "")
        : type(t), value(v) {}
};

// Dynamic expression context
struct DynamicExpressionContext {
    std::map<std::string, std::string> variables;
    std::map<std::string, std::string> properties;
    std::map<std::string, std::string> functions;
    std::string current_element;
    std::string current_attribute;
    
    DynamicExpressionContext() = default;
};

// Dynamic expression evaluator
class CHTLDynamicExpressionEvaluator {
public:
    CHTLDynamicExpressionEvaluator();
    ~CHTLDynamicExpressionEvaluator();
    
    // Expression parsing
    std::shared_ptr<DynamicExpressionNode> parseExpression(const std::string& expression) const;
    bool isValidExpression(const std::string& expression) const;
    
    // Expression evaluation
    std::string evaluateExpression(const std::shared_ptr<DynamicExpressionNode>& node, const DynamicExpressionContext& context);
    std::string evaluateExpression(const std::string& expression, const DynamicExpressionContext& context);
    
    // Context management
    void setContextVariable(const std::string& name, const std::string& value);
    void setContextProperty(const std::string& name, const std::string& value);
    void setContextFunction(const std::string& name, const std::string& value);
    void clearContext();
    
    // Expression optimization
    std::shared_ptr<DynamicExpressionNode> optimizeExpression(const std::shared_ptr<DynamicExpressionNode>& node);
    std::string optimizeExpressionString(const std::string& expression);
    
    // Expression validation
    bool validateExpression(const std::shared_ptr<DynamicExpressionNode>& node) const;
    std::vector<std::string> getExpressionErrors(const std::shared_ptr<DynamicExpressionNode>& node) const;
    
    // Expression utilities
    std::string generateExpressionId(const std::string& expression);
    std::vector<std::string> extractVariables(const std::shared_ptr<DynamicExpressionNode>& node) const;
    std::vector<std::string> extractFunctions(const std::shared_ptr<DynamicExpressionNode>& node) const;
    
private:
    DynamicExpressionContext context;
    
    // Parsing helpers
    std::shared_ptr<DynamicExpressionNode> parseBinaryExpression(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parseUnaryExpression(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parsePrimaryExpression(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parseFunctionCall(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parsePropertyAccess(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parseArrayAccess(const std::string& expression, size_t& pos) const;
    std::shared_ptr<DynamicExpressionNode> parseConditionalExpression(const std::string& expression, size_t& pos) const;
    
    // Evaluation helpers
    std::string evaluateBinaryOperation(const std::string& op, const std::string& left, const std::string& right);
    std::string evaluateUnaryOperation(const std::string& op, const std::string& operand);
    std::string evaluateFunctionCall(const std::string& func, const std::vector<std::string>& args);
    std::string evaluatePropertyAccess(const std::string& object, const std::string& property);
    std::string evaluateArrayAccess(const std::string& array, const std::string& index);
    std::string evaluateConditional(const std::string& condition, const std::string& true_value, const std::string& false_value);
    
    // Utility helpers
    bool isOperator(const std::string& token) const;
    bool isFunction(const std::string& token) const;
    bool isVariable(const std::string& token) const;
    int getOperatorPrecedence(const std::string& op) const;
    bool isLeftAssociative(const std::string& op) const;
    std::string trimWhitespace(const std::string& str) const;
    std::vector<std::string> tokenizeExpression(const std::string& expression) const;
    bool isNumeric(const std::string& str) const;
    bool isString(const std::string& str) const;
    std::string unquoteString(const std::string& str) const;
};

// Dynamic attribute condition processor
class CHTLDynamicAttributeProcessor {
public:
    CHTLDynamicAttributeProcessor();
    ~CHTLDynamicAttributeProcessor();
    
    // Attribute processing
    std::string processAttributeCondition(const std::string& attribute, const std::string& condition, const DynamicExpressionContext& context);
    std::string processAttributeExpression(const std::string& attribute, const std::string& expression, const DynamicExpressionContext& context);
    
    // Condition evaluation
    bool evaluateCondition(const std::string& condition, const DynamicExpressionContext& context);
    std::string evaluateExpression(const std::string& expression, const DynamicExpressionContext& context);
    
    // Attribute manipulation
    std::string setAttributeValue(const std::string& attribute, const std::string& value);
    std::string removeAttribute(const std::string& attribute);
    std::string toggleAttribute(const std::string& attribute, bool condition);
    
    // Context management
    void setContext(const DynamicExpressionContext& ctx);
    DynamicExpressionContext getContext() const;
    
    // Attribute utilities
    std::vector<std::string> extractAttributeConditions(const std::string& attribute) const;
    std::vector<std::string> extractAttributeExpressions(const std::string& attribute) const;
    bool hasAttributeCondition(const std::string& attribute) const;
    bool hasAttributeExpression(const std::string& attribute) const;
    
private:
    CHTLDynamicExpressionEvaluator evaluator;
    DynamicExpressionContext context;
    
    // Processing helpers
    std::string parseAttributeCondition(const std::string& condition) const;
    std::string parseAttributeExpression(const std::string& expression) const;
    bool isConditionalAttribute(const std::string& attribute) const;
    bool isExpressionAttribute(const std::string& attribute) const;
    std::string extractConditionFromAttribute(const std::string& attribute) const;
    std::string extractExpressionFromAttribute(const std::string& attribute) const;
};

// Dynamic expression manager
class CHTLDynamicExpressionManager {
public:
    CHTLDynamicExpressionManager();
    ~CHTLDynamicExpressionManager();
    
    // Expression management
    bool registerExpression(const std::string& name, const std::string& expression);
    bool unregisterExpression(const std::string& name);
    std::string getExpression(const std::string& name) const;
    bool hasExpression(const std::string& name) const;
    
    // Expression evaluation
    std::string evaluateExpression(const std::string& name, const DynamicExpressionContext& context);
    std::string evaluateExpressionString(const std::string& expression, const DynamicExpressionContext& context);
    
    // Context management
    void setGlobalContext(const DynamicExpressionContext& context);
    DynamicExpressionContext getGlobalContext() const;
    void updateGlobalContext(const DynamicExpressionContext& context);
    
    // Expression utilities
    std::vector<std::string> getRegisteredExpressions() const;
    std::vector<std::string> getExpressionDependencies(const std::string& name) const;
    bool validateAllExpressions() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Expression optimization
    void optimizeAllExpressions();
    void optimizeExpression(const std::string& name);
    
private:
    std::map<std::string, std::string> registered_expressions;
    std::map<std::string, std::shared_ptr<DynamicExpressionNode>> expression_nodes;
    DynamicExpressionContext global_context;
    CHTLDynamicExpressionEvaluator evaluator;
    
    // Management helpers
    void updateExpressionNode(const std::string& name, const std::string& expression);
    std::vector<std::string> findExpressionDependencies(const std::string& expression) const;
    bool hasCircularDependency(const std::string& name, const std::vector<std::string>& visited) const;
    void clearExpressionCache();
};

} // namespace CHTL

#endif // CHTLJSDYNAMICEXPRESSIONS_H