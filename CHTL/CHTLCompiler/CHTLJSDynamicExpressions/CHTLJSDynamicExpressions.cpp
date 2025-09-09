#include "CHTLJSDynamicExpressions.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>

namespace CHTL {

// CHTLDynamicExpressionEvaluator implementation
CHTLDynamicExpressionEvaluator::CHTLDynamicExpressionEvaluator() {
}

CHTLDynamicExpressionEvaluator::~CHTLDynamicExpressionEvaluator() {
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseExpression(const std::string& expression) const {
    std::string trimmed = trimWhitespace(expression);
    if (trimmed.empty()) {
        return nullptr;
    }
    
    size_t pos = 0;
    return parseBinaryExpression(trimmed, pos);
}

bool CHTLDynamicExpressionEvaluator::isValidExpression(const std::string& expression) const {
    try {
        auto node = parseExpression(expression);
        return node != nullptr && validateExpression(node);
    } catch (...) {
        return false;
    }
}

std::string CHTLDynamicExpressionEvaluator::evaluateExpression(const std::shared_ptr<DynamicExpressionNode>& node, const DynamicExpressionContext& context) {
    if (!node) {
        return "";
    }
    
    switch (node->type) {
        case DynamicExpressionType::Literal:
            return node->value;
            
        case DynamicExpressionType::Variable: {
            auto it = context.variables.find(node->value);
            if (it != context.variables.end()) {
                return it->second;
            }
            return "";
        }
        
        case DynamicExpressionType::BinaryOperation: {
            if (node->children.size() >= 2) {
                std::string left = evaluateExpression(node->children[0], context);
                std::string right = evaluateExpression(node->children[1], context);
                return evaluateBinaryOperation(node->value, left, right);
            }
            return "";
        }
        
        case DynamicExpressionType::UnaryOperation: {
            if (!node->children.empty()) {
                std::string operand = evaluateExpression(node->children[0], context);
                return evaluateUnaryOperation(node->value, operand);
            }
            return "";
        }
        
        case DynamicExpressionType::FunctionCall: {
            std::vector<std::string> args;
            for (size_t i = 1; i < node->children.size(); ++i) {
                args.push_back(evaluateExpression(node->children[i], context));
            }
            return evaluateFunctionCall(node->value, args);
        }
        
        case DynamicExpressionType::PropertyAccess: {
            if (node->children.size() >= 2) {
                std::string object = evaluateExpression(node->children[0], context);
                std::string property = evaluateExpression(node->children[1], context);
                return evaluatePropertyAccess(object, property);
            }
            return "";
        }
        
        case DynamicExpressionType::ArrayAccess: {
            if (node->children.size() >= 2) {
                std::string array = evaluateExpression(node->children[0], context);
                std::string index = evaluateExpression(node->children[1], context);
                return evaluateArrayAccess(array, index);
            }
            return "";
        }
        
        case DynamicExpressionType::Conditional: {
            if (node->children.size() >= 3) {
                std::string condition = evaluateExpression(node->children[0], context);
                std::string true_value = evaluateExpression(node->children[1], context);
                std::string false_value = evaluateExpression(node->children[2], context);
                return evaluateConditional(condition, true_value, false_value);
            }
            return "";
        }
        
        default:
            return "";
    }
}

std::string CHTLDynamicExpressionEvaluator::evaluateExpression(const std::string& expression, const DynamicExpressionContext& context) {
    auto node = parseExpression(expression);
    return evaluateExpression(node, context);
}

void CHTLDynamicExpressionEvaluator::setContextVariable(const std::string& name, const std::string& value) {
    context.variables[name] = value;
}

void CHTLDynamicExpressionEvaluator::setContextProperty(const std::string& name, const std::string& value) {
    context.properties[name] = value;
}

void CHTLDynamicExpressionEvaluator::setContextFunction(const std::string& name, const std::string& value) {
    context.functions[name] = value;
}

void CHTLDynamicExpressionEvaluator::clearContext() {
    context.variables.clear();
    context.properties.clear();
    context.functions.clear();
    context.current_element.clear();
    context.current_attribute.clear();
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::optimizeExpression(const std::shared_ptr<DynamicExpressionNode>& node) {
    if (!node) {
        return nullptr;
    }
    
    // Simple optimization - return the node as is for now
    return node;
}

std::string CHTLDynamicExpressionEvaluator::optimizeExpressionString(const std::string& expression) {
    auto node = parseExpression(expression);
    auto optimized = optimizeExpression(node);
    // For now, return the original expression
    return expression;
}

bool CHTLDynamicExpressionEvaluator::validateExpression(const std::shared_ptr<DynamicExpressionNode>& node) const {
    if (!node) {
        return false;
    }
    
    // Basic validation
    switch (node->type) {
        case DynamicExpressionType::BinaryOperation:
            return node->children.size() >= 2;
        case DynamicExpressionType::UnaryOperation:
            return node->children.size() >= 1;
        case DynamicExpressionType::FunctionCall:
            return node->children.size() >= 1;
        case DynamicExpressionType::PropertyAccess:
            return node->children.size() >= 2;
        case DynamicExpressionType::ArrayAccess:
            return node->children.size() >= 2;
        case DynamicExpressionType::Conditional:
            return node->children.size() >= 3;
        default:
            return true;
    }
}

std::vector<std::string> CHTLDynamicExpressionEvaluator::getExpressionErrors(const std::shared_ptr<DynamicExpressionNode>& node) const {
    std::vector<std::string> errors;
    
    if (!node) {
        errors.push_back("Null expression node");
        return errors;
    }
    
    if (!validateExpression(node)) {
        errors.push_back("Invalid expression structure");
    }
    
    return errors;
}

std::string CHTLDynamicExpressionEvaluator::generateExpressionId(const std::string& expression) {
    // Simple hash-based ID generation
    std::hash<std::string> hasher;
    size_t hash = hasher(expression);
    return "expr_" + std::to_string(hash);
}

std::vector<std::string> CHTLDynamicExpressionEvaluator::extractVariables(const std::shared_ptr<DynamicExpressionNode>& node) const {
    std::vector<std::string> variables;
    
    if (!node) {
        return variables;
    }
    
    if (node->type == DynamicExpressionType::Variable) {
        variables.push_back(node->value);
    }
    
    for (const auto& child : node->children) {
        auto child_vars = extractVariables(child);
        variables.insert(variables.end(), child_vars.begin(), child_vars.end());
    }
    
    return variables;
}

std::vector<std::string> CHTLDynamicExpressionEvaluator::extractFunctions(const std::shared_ptr<DynamicExpressionNode>& node) const {
    std::vector<std::string> functions;
    
    if (!node) {
        return functions;
    }
    
    if (node->type == DynamicExpressionType::FunctionCall) {
        functions.push_back(node->value);
    }
    
    for (const auto& child : node->children) {
        auto child_funcs = extractFunctions(child);
        functions.insert(functions.end(), child_funcs.begin(), child_funcs.end());
    }
    
    return functions;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseBinaryExpression(const std::string& expression, size_t& pos) const {
    // Simplified binary expression parsing
    auto left = parseUnaryExpression(expression, pos);
    
    while (pos < expression.length()) {
        // Skip whitespace
        while (pos < expression.length() && std::isspace(expression[pos])) {
            pos++;
        }
        
        if (pos >= expression.length()) {
            break;
        }
        
        // Check for operators
        std::string op;
        if (expression[pos] == '+' || expression[pos] == '-' || expression[pos] == '*' || expression[pos] == '/') {
            op = std::string(1, expression[pos++]);
        } else if (pos + 1 < expression.length()) {
            std::string two_char = expression.substr(pos, 2);
            if (two_char == "==" || two_char == "!=" || two_char == "<=" || two_char == ">=") {
                op = two_char;
                pos += 2;
            }
        }
        
        if (op.empty()) {
            break;
        }
        
        auto right = parseUnaryExpression(expression, pos);
        if (!right) {
            break;
        }
        
        auto binary_node = std::make_shared<DynamicExpressionNode>(DynamicExpressionType::BinaryOperation, op);
        binary_node->children.push_back(left);
        binary_node->children.push_back(right);
        left = binary_node;
    }
    
    return left;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseUnaryExpression(const std::string& expression, size_t& pos) const {
    // Skip whitespace
    while (pos < expression.length() && std::isspace(expression[pos])) {
        pos++;
    }
    
    if (pos >= expression.length()) {
        return nullptr;
    }
    
    // Check for unary operators
    if (expression[pos] == '+' || expression[pos] == '-' || expression[pos] == '!') {
        std::string op = std::string(1, expression[pos++]);
        auto operand = parsePrimaryExpression(expression, pos);
        if (operand) {
            auto unary_node = std::make_shared<DynamicExpressionNode>(DynamicExpressionType::UnaryOperation, op);
            unary_node->children.push_back(operand);
            return unary_node;
        }
    }
    
    return parsePrimaryExpression(expression, pos);
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parsePrimaryExpression(const std::string& expression, size_t& pos) const {
    // Skip whitespace
    while (pos < expression.length() && std::isspace(expression[pos])) {
        pos++;
    }
    
    if (pos >= expression.length()) {
        return nullptr;
    }
    
    // Check for literals
    if (expression[pos] == '"' || expression[pos] == '\'') {
        char quote = expression[pos++];
        std::string value;
        while (pos < expression.length() && expression[pos] != quote) {
            value += expression[pos++];
        }
        if (pos < expression.length()) {
            pos++; // Skip closing quote
        }
        return std::make_shared<DynamicExpressionNode>(DynamicExpressionType::Literal, value);
    }
    
    // Check for numbers
    if (std::isdigit(expression[pos]) || expression[pos] == '.') {
        std::string value;
        while (pos < expression.length() && (std::isdigit(expression[pos]) || expression[pos] == '.')) {
            value += expression[pos++];
        }
        return std::make_shared<DynamicExpressionNode>(DynamicExpressionType::Literal, value);
    }
    
    // Check for variables or function calls
    if (std::isalpha(expression[pos]) || expression[pos] == '_') {
        std::string identifier;
        while (pos < expression.length() && (std::isalnum(expression[pos]) || expression[pos] == '_')) {
            identifier += expression[pos++];
        }
        
        // Check if it's a function call
        while (pos < expression.length() && std::isspace(expression[pos])) {
            pos++;
        }
        
        if (pos < expression.length() && expression[pos] == '(') {
            return parseFunctionCall(expression, pos);
        }
        
        return std::make_shared<DynamicExpressionNode>(DynamicExpressionType::Variable, identifier);
    }
    
    // Check for parentheses
    if (expression[pos] == '(') {
        pos++; // Skip opening parenthesis
        auto result = parseBinaryExpression(expression, pos);
        while (pos < expression.length() && expression[pos] != ')') {
            pos++;
        }
        if (pos < expression.length()) {
            pos++; // Skip closing parenthesis
        }
        return result;
    }
    
    return nullptr;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseFunctionCall(const std::string& /* expression */, size_t& /* pos */) const {
    // This is a simplified implementation
    return nullptr;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parsePropertyAccess(const std::string& /* expression */, size_t& /* pos */) const {
    // This is a simplified implementation
    return nullptr;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseArrayAccess(const std::string& /* expression */, size_t& /* pos */) const {
    // This is a simplified implementation
    return nullptr;
}

std::shared_ptr<DynamicExpressionNode> CHTLDynamicExpressionEvaluator::parseConditionalExpression(const std::string& /* expression */, size_t& /* pos */) const {
    // This is a simplified implementation
    return nullptr;
}

std::string CHTLDynamicExpressionEvaluator::evaluateBinaryOperation(const std::string& op, const std::string& left, const std::string& right) {
    if (op == "+") {
        if (isNumeric(left) && isNumeric(right)) {
            return std::to_string(std::stod(left) + std::stod(right));
        }
        return left + right;
    } else if (op == "-") {
        if (isNumeric(left) && isNumeric(right)) {
            return std::to_string(std::stod(left) - std::stod(right));
        }
    } else if (op == "*") {
        if (isNumeric(left) && isNumeric(right)) {
            return std::to_string(std::stod(left) * std::stod(right));
        }
    } else if (op == "/") {
        if (isNumeric(left) && isNumeric(right)) {
            double right_val = std::stod(right);
            if (right_val != 0) {
                return std::to_string(std::stod(left) / right_val);
            }
        }
    } else if (op == "==") {
        return (left == right) ? "true" : "false";
    } else if (op == "!=") {
        return (left != right) ? "true" : "false";
    } else if (op == "<=") {
        if (isNumeric(left) && isNumeric(right)) {
            return (std::stod(left) <= std::stod(right)) ? "true" : "false";
        }
    } else if (op == ">=") {
        if (isNumeric(left) && isNumeric(right)) {
            return (std::stod(left) >= std::stod(right)) ? "true" : "false";
        }
    }
    
    return "";
}

std::string CHTLDynamicExpressionEvaluator::evaluateUnaryOperation(const std::string& op, const std::string& operand) {
    if (op == "+") {
        return operand;
    } else if (op == "-") {
        if (isNumeric(operand)) {
            return std::to_string(-std::stod(operand));
        }
    } else if (op == "!") {
        return (operand == "true" || operand == "1") ? "false" : "true";
    }
    
    return "";
}

std::string CHTLDynamicExpressionEvaluator::evaluateFunctionCall(const std::string& func, const std::vector<std::string>& args) {
    // Simple function evaluation
    if (func == "length" && !args.empty()) {
        return std::to_string(args[0].length());
    } else if (func == "upper" && !args.empty()) {
        std::string result = args[0];
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    } else if (func == "lower" && !args.empty()) {
        std::string result = args[0];
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    return "";
}

std::string CHTLDynamicExpressionEvaluator::evaluatePropertyAccess(const std::string& /* object */, const std::string& /* property */) {
    // Simple property access
    return "";
}

std::string CHTLDynamicExpressionEvaluator::evaluateArrayAccess(const std::string& /* array */, const std::string& /* index */) {
    // Simple array access
    return "";
}

std::string CHTLDynamicExpressionEvaluator::evaluateConditional(const std::string& condition, const std::string& true_value, const std::string& false_value) {
    return (condition == "true" || condition == "1") ? true_value : false_value;
}

bool CHTLDynamicExpressionEvaluator::isOperator(const std::string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/" ||
           token == "==" || token == "!=" || token == "<=" || token == ">=" ||
           token == "<" || token == ">" || token == "&&" || token == "||";
}

bool CHTLDynamicExpressionEvaluator::isFunction(const std::string& token) const {
    return token == "length" || token == "upper" || token == "lower" ||
           token == "abs" || token == "round" || token == "floor" || token == "ceil";
}

bool CHTLDynamicExpressionEvaluator::isVariable(const std::string& token) const {
    return !token.empty() && (std::isalpha(token[0]) || token[0] == '_');
}

int CHTLDynamicExpressionEvaluator::getOperatorPrecedence(const std::string& op) const {
    if (op == "*" || op == "/") return 3;
    if (op == "+" || op == "-") return 2;
    if (op == "==" || op == "!=" || op == "<=" || op == ">=" || op == "<" || op == ">") return 1;
    return 0;
}

bool CHTLDynamicExpressionEvaluator::isLeftAssociative(const std::string& op) const {
    return op != "**"; // Most operators are left-associative
}

std::string CHTLDynamicExpressionEvaluator::trimWhitespace(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> CHTLDynamicExpressionEvaluator::tokenizeExpression(const std::string& expression) const {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : expression) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (isOperator(std::string(1, c)) || c == '(' || c == ')' || c == '[' || c == ']' || c == '.' || c == ',') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool CHTLDynamicExpressionEvaluator::isNumeric(const std::string& str) const {
    if (str.empty()) return false;
    size_t pos = 0;
    if (str[0] == '-' || str[0] == '+') pos++;
    if (pos >= str.length()) return false;
    
    bool has_digit = false;
    bool has_dot = false;
    
    for (size_t i = pos; i < str.length(); ++i) {
        if (std::isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else {
            return false;
        }
    }
    
    return has_digit;
}

bool CHTLDynamicExpressionEvaluator::isString(const std::string& str) const {
    return str.length() >= 2 && ((str[0] == '"' && str[str.length()-1] == '"') ||
                                 (str[0] == '\'' && str[str.length()-1] == '\''));
}

std::string CHTLDynamicExpressionEvaluator::unquoteString(const std::string& str) const {
    if (isString(str)) {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

// CHTLDynamicAttributeProcessor implementation
CHTLDynamicAttributeProcessor::CHTLDynamicAttributeProcessor() {
}

CHTLDynamicAttributeProcessor::~CHTLDynamicAttributeProcessor() {
}

std::string CHTLDynamicAttributeProcessor::processAttributeCondition(const std::string& attribute, const std::string& condition, const DynamicExpressionContext& context) {
    bool result = evaluator.evaluateExpression(condition, context) == "true";
    return result ? attribute : "";
}

std::string CHTLDynamicAttributeProcessor::processAttributeExpression(const std::string& attribute, const std::string& expression, const DynamicExpressionContext& context) {
    std::string value = evaluator.evaluateExpression(expression, context);
    return attribute + "=\"" + value + "\"";
}

bool CHTLDynamicAttributeProcessor::evaluateCondition(const std::string& condition, const DynamicExpressionContext& context) {
    return evaluator.evaluateExpression(condition, context) == "true";
}

std::string CHTLDynamicAttributeProcessor::evaluateExpression(const std::string& expression, const DynamicExpressionContext& context) {
    return evaluator.evaluateExpression(expression, context);
}

std::string CHTLDynamicAttributeProcessor::setAttributeValue(const std::string& attribute, const std::string& value) {
    return attribute + "=\"" + value + "\"";
}

std::string CHTLDynamicAttributeProcessor::removeAttribute(const std::string& /* attribute */) {
    return "";
}

std::string CHTLDynamicAttributeProcessor::toggleAttribute(const std::string& attribute, bool condition) {
    return condition ? attribute : "";
}

void CHTLDynamicAttributeProcessor::setContext(const DynamicExpressionContext& ctx) {
    context = ctx;
}

DynamicExpressionContext CHTLDynamicAttributeProcessor::getContext() const {
    return context;
}

std::vector<std::string> CHTLDynamicAttributeProcessor::extractAttributeConditions(const std::string& /* attribute */) const {
    std::vector<std::string> conditions;
    // Simple extraction logic
    return conditions;
}

std::vector<std::string> CHTLDynamicAttributeProcessor::extractAttributeExpressions(const std::string& /* attribute */) const {
    std::vector<std::string> expressions;
    // Simple extraction logic
    return expressions;
}

bool CHTLDynamicAttributeProcessor::hasAttributeCondition(const std::string& attribute) const {
    return attribute.find("?") != std::string::npos;
}

bool CHTLDynamicAttributeProcessor::hasAttributeExpression(const std::string& attribute) const {
    return attribute.find("{{") != std::string::npos;
}

std::string CHTLDynamicAttributeProcessor::parseAttributeCondition(const std::string& condition) const {
    return condition;
}

std::string CHTLDynamicAttributeProcessor::parseAttributeExpression(const std::string& expression) const {
    return expression;
}

bool CHTLDynamicAttributeProcessor::isConditionalAttribute(const std::string& attribute) const {
    return hasAttributeCondition(attribute);
}

bool CHTLDynamicAttributeProcessor::isExpressionAttribute(const std::string& attribute) const {
    return hasAttributeExpression(attribute);
}

std::string CHTLDynamicAttributeProcessor::extractConditionFromAttribute(const std::string& attribute) const {
    size_t pos = attribute.find("?");
    if (pos != std::string::npos) {
        return attribute.substr(0, pos);
    }
    return "";
}

std::string CHTLDynamicAttributeProcessor::extractExpressionFromAttribute(const std::string& attribute) const {
    size_t start = attribute.find("{{");
    if (start != std::string::npos) {
        size_t end = attribute.find("}}", start);
        if (end != std::string::npos) {
            return attribute.substr(start + 2, end - start - 2);
        }
    }
    return "";
}

// CHTLDynamicExpressionManager implementation
CHTLDynamicExpressionManager::CHTLDynamicExpressionManager() {
}

CHTLDynamicExpressionManager::~CHTLDynamicExpressionManager() {
}

bool CHTLDynamicExpressionManager::registerExpression(const std::string& name, const std::string& expression) {
    if (name.empty() || expression.empty()) {
        return false;
    }
    
    registered_expressions[name] = expression;
    updateExpressionNode(name, expression);
    return true;
}

bool CHTLDynamicExpressionManager::unregisterExpression(const std::string& name) {
    auto it = registered_expressions.find(name);
    if (it != registered_expressions.end()) {
        registered_expressions.erase(it);
        expression_nodes.erase(name);
        return true;
    }
    return false;
}

std::string CHTLDynamicExpressionManager::getExpression(const std::string& name) const {
    auto it = registered_expressions.find(name);
    return (it != registered_expressions.end()) ? it->second : "";
}

bool CHTLDynamicExpressionManager::hasExpression(const std::string& name) const {
    return registered_expressions.find(name) != registered_expressions.end();
}

std::string CHTLDynamicExpressionManager::evaluateExpression(const std::string& name, const DynamicExpressionContext& context) {
    auto it = expression_nodes.find(name);
    if (it != expression_nodes.end()) {
        return evaluator.evaluateExpression(it->second, context);
    }
    return "";
}

std::string CHTLDynamicExpressionManager::evaluateExpressionString(const std::string& expression, const DynamicExpressionContext& context) {
    return evaluator.evaluateExpression(expression, context);
}

void CHTLDynamicExpressionManager::setGlobalContext(const DynamicExpressionContext& context) {
    global_context = context;
}

DynamicExpressionContext CHTLDynamicExpressionManager::getGlobalContext() const {
    return global_context;
}

void CHTLDynamicExpressionManager::updateGlobalContext(const DynamicExpressionContext& context) {
    global_context = context;
}

std::vector<std::string> CHTLDynamicExpressionManager::getRegisteredExpressions() const {
    std::vector<std::string> expressions;
    for (const auto& pair : registered_expressions) {
        expressions.push_back(pair.first);
    }
    return expressions;
}

std::vector<std::string> CHTLDynamicExpressionManager::getExpressionDependencies(const std::string& name) const {
    auto it = expression_nodes.find(name);
    if (it != expression_nodes.end()) {
        return evaluator.extractVariables(it->second);
    }
    return {};
}

bool CHTLDynamicExpressionManager::validateAllExpressions() const {
    for (const auto& pair : expression_nodes) {
        if (!evaluator.validateExpression(pair.second)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CHTLDynamicExpressionManager::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& pair : expression_nodes) {
        auto node_errors = evaluator.getExpressionErrors(pair.second);
        errors.insert(errors.end(), node_errors.begin(), node_errors.end());
    }
    return errors;
}

void CHTLDynamicExpressionManager::optimizeAllExpressions() {
    for (auto& pair : expression_nodes) {
        pair.second = evaluator.optimizeExpression(pair.second);
    }
}

void CHTLDynamicExpressionManager::optimizeExpression(const std::string& name) {
    auto it = expression_nodes.find(name);
    if (it != expression_nodes.end()) {
        it->second = evaluator.optimizeExpression(it->second);
    }
}

void CHTLDynamicExpressionManager::updateExpressionNode(const std::string& name, const std::string& expression) {
    auto node = evaluator.parseExpression(expression);
    if (node) {
        expression_nodes[name] = node;
    }
}

std::vector<std::string> CHTLDynamicExpressionManager::findExpressionDependencies(const std::string& expression) const {
    auto node = evaluator.parseExpression(expression);
    return evaluator.extractVariables(node);
}

bool CHTLDynamicExpressionManager::hasCircularDependency(const std::string& name, const std::vector<std::string>& visited) const {
    if (std::find(visited.begin(), visited.end(), name) != visited.end()) {
        return true;
    }
    
    auto dependencies = getExpressionDependencies(name);
    for (const auto& dep : dependencies) {
        if (hasExpression(dep)) {
            std::vector<std::string> new_visited = visited;
            new_visited.push_back(name);
            if (hasCircularDependency(dep, new_visited)) {
                return true;
            }
        }
    }
    
    return false;
}

void CHTLDynamicExpressionManager::clearExpressionCache() {
    expression_nodes.clear();
}

} // namespace CHTL