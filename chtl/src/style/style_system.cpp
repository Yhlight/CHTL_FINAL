#include "chtl/style_system.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace chtl {
namespace style {

// StyleExpressionParser 实现
StyleExpressionParser::StyleExpressionParser(const std::string& expression, StyleContext& context)
    : expression_(expression), position_(0), context_(context) {}

std::shared_ptr<StyleValue> StyleExpressionParser::parse() {
    return parseExpression();
}

void StyleExpressionParser::skipWhitespace() {
    while (position_ < expression_.length() && std::isspace(expression_[position_])) {
        position_++;
    }
}

char StyleExpressionParser::currentChar() const {
    if (position_ >= expression_.length()) return '\0';
    return expression_[position_];
}

char StyleExpressionParser::peekChar() const {
    if (position_ + 1 >= expression_.length()) return '\0';
    return expression_[position_ + 1];
}

void StyleExpressionParser::advance() {
    if (position_ < expression_.length()) {
        position_++;
    }
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseExpression() {
    return parseConditional();
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseConditional() {
    auto left = parseLogicalOr();
    
    skipWhitespace();
    if (currentChar() == '?') {
        advance(); // 跳过 ?
        auto trueValue = parseExpression();
        skipWhitespace();
        if (currentChar() == ':') {
            advance(); // 跳过 :
            auto falseValue = parseExpression();
            
            auto result = std::make_shared<StyleValue>(StyleValueType::CONDITIONAL);
            result->condition = left;
            result->trueValue = trueValue;
            result->falseValue = falseValue;
            return result;
        }
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    skipWhitespace();
    if (currentChar() == '|' && peekChar() == '|') {
        advance(); // 跳过 |
        advance(); // 跳过 |
        auto right = parseLogicalOr();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "||";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseLogicalAnd() {
    auto left = parseEquality();
    
    skipWhitespace();
    if (currentChar() == '&' && peekChar() == '&') {
        advance(); // 跳过 &
        advance(); // 跳过 &
        auto right = parseLogicalAnd();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "&&";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseEquality() {
    auto left = parseComparison();
    
    skipWhitespace();
    if (currentChar() == '=' && peekChar() == '=') {
        advance(); // 跳过 =
        advance(); // 跳过 =
        auto right = parseEquality();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "==";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '!' && peekChar() == '=') {
        advance(); // 跳过 !
        advance(); // 跳过 =
        auto right = parseEquality();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "!=";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseComparison() {
    auto left = parseAddition();
    
    skipWhitespace();
    if (currentChar() == '<' && peekChar() == '=') {
        advance(); // 跳过 <
        advance(); // 跳过 =
        auto right = parseComparison();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "<=";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '>' && peekChar() == '=') {
        advance(); // 跳过 >
        advance(); // 跳过 =
        auto right = parseComparison();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = ">=";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '<') {
        advance(); // 跳过 <
        auto right = parseComparison();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "<";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '>') {
        advance(); // 跳过 >
        auto right = parseComparison();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = ">";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseAddition() {
    auto left = parseMultiplication();
    
    skipWhitespace();
    if (currentChar() == '+') {
        advance(); // 跳过 +
        auto right = parseAddition();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "+";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '-') {
        advance(); // 跳过 -
        auto right = parseAddition();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "-";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseMultiplication() {
    auto left = parseUnary();
    
    skipWhitespace();
    if (currentChar() == '*') {
        advance(); // 跳过 *
        auto right = parseMultiplication();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "*";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '/') {
        advance(); // 跳过 /
        auto right = parseMultiplication();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "/";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    } else if (currentChar() == '%') {
        advance(); // 跳过 %
        auto right = parseMultiplication();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "%";
        result->arguments.push_back(left);
        result->arguments.push_back(right);
        return result;
    }
    
    return left;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseUnary() {
    skipWhitespace();
    
    if (currentChar() == '-') {
        advance(); // 跳过 -
        auto operand = parseUnary();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "-";
        result->arguments.push_back(operand);
        return result;
    } else if (currentChar() == '!') {
        advance(); // 跳过 !
        auto operand = parseUnary();
        
        auto result = std::make_shared<StyleValue>(StyleValueType::EXPRESSION);
        result->value = "!";
        result->arguments.push_back(operand);
        return result;
    }
    
    return parsePrimary();
}

std::shared_ptr<StyleValue> StyleExpressionParser::parsePrimary() {
    skipWhitespace();
    
    if (currentChar() == '(') {
        advance(); // 跳过 (
        auto result = parseExpression();
        skipWhitespace();
        if (currentChar() == ')') {
            advance(); // 跳过 )
        }
        return result;
    } else if (currentChar() == '"' || currentChar() == '\'') {
        return parseString();
    } else if (std::isdigit(currentChar())) {
        return parseNumber();
    } else if (std::isalpha(currentChar()) || currentChar() == '_') {
        return parseIdentifier();
    }
    
    // 默认返回空值
    return std::make_shared<StyleValue>(StyleValueType::LITERAL, "");
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseFunction() {
    std::string name = parseIdentifierLiteral();
    skipWhitespace();
    
    if (currentChar() == '(') {
        advance(); // 跳过 (
        std::vector<std::shared_ptr<StyleValue>> args;
        
        skipWhitespace();
        if (currentChar() != ')') {
            do {
                args.push_back(parseExpression());
                skipWhitespace();
                if (currentChar() == ',') {
                    advance(); // 跳过 ,
                    skipWhitespace();
                }
            } while (currentChar() != ')');
        }
        
        if (currentChar() == ')') {
            advance(); // 跳过 )
        }
        
        auto result = std::make_shared<StyleValue>(StyleValueType::FUNCTION);
        result->value = name;
        result->arguments = args;
        return result;
    }
    
    return std::make_shared<StyleValue>(StyleValueType::LITERAL, name);
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseReference() {
    std::string reference = parseIdentifierLiteral();
    
    auto result = std::make_shared<StyleValue>(StyleValueType::REFERENCE);
    result->value = reference;
    return result;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseLiteral() {
    if (currentChar() == '"' || currentChar() == '\'') {
        return parseString();
    } else if (std::isdigit(currentChar())) {
        return parseNumber();
    } else {
        return parseIdentifier();
    }
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseString() {
    std::string value = parseStringLiteral();
    
    auto result = std::make_shared<StyleValue>(StyleValueType::LITERAL);
    result->value = value;
    return result;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseNumber() {
    std::string value = parseNumberLiteral();
    
    auto result = std::make_shared<StyleValue>(StyleValueType::LITERAL);
    result->value = value;
    return result;
}

std::shared_ptr<StyleValue> StyleExpressionParser::parseIdentifier() {
    std::string value = parseIdentifierLiteral();
    
    // 检查是否是函数调用
    skipWhitespace();
    if (currentChar() == '(') {
        return parseFunction();
    }
    
    // 检查是否是引用
    if (context_.variables.find(value) != context_.variables.end()) {
        return parseReference();
    }
    
    // 默认作为字面量
    auto result = std::make_shared<StyleValue>(StyleValueType::LITERAL);
    result->value = value;
    return result;
}

std::string StyleExpressionParser::parseStringLiteral() {
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string result;
    while (currentChar() != quote && currentChar() != '\0') {
        if (currentChar() == '\\') {
            advance(); // 跳过 \
            if (currentChar() == 'n') {
                result += '\n';
            } else if (currentChar() == 't') {
                result += '\t';
            } else if (currentChar() == 'r') {
                result += '\r';
            } else if (currentChar() == '\\') {
                result += '\\';
            } else if (currentChar() == quote) {
                result += quote;
            } else {
                result += currentChar();
            }
        } else {
            result += currentChar();
        }
        advance();
    }
    
    if (currentChar() == quote) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string StyleExpressionParser::parseNumberLiteral() {
    std::string result;
    
    if (currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    while (std::isdigit(currentChar())) {
        result += currentChar();
        advance();
    }
    
    if (currentChar() == '.') {
        result += currentChar();
        advance();
        while (std::isdigit(currentChar())) {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

std::string StyleExpressionParser::parseIdentifierLiteral() {
    std::string result;
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    return result;
}

bool StyleExpressionParser::isOperator(const std::string& op) const {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
           op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=" ||
           op == "&&" || op == "||" || op == "!";
}

bool StyleExpressionParser::isKeyword(const std::string& word) const {
    return word == "true" || word == "false" || word == "null" || word == "undefined";
}

int StyleExpressionParser::getPrecedence(const std::string& op) const {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == "<" || op == ">" || op == "<=" || op == ">=") return 4;
    if (op == "+" || op == "-") return 5;
    if (op == "*" || op == "/" || op == "%") return 6;
    if (op == "!") return 7;
    return 0;
}

bool StyleExpressionParser::isRightAssociative(const std::string& op) const {
    return op == "!" || op == "-";
}

// StyleSelectorParser 实现
StyleSelectorParser::StyleSelectorParser(const std::string& selector, StyleContext& context)
    : selector_(selector), position_(0), context_(context) {}

std::string StyleSelectorParser::parse() {
    return parseSelector();
}

void StyleSelectorParser::skipWhitespace() {
    while (position_ < selector_.length() && std::isspace(selector_[position_])) {
        position_++;
    }
}

char StyleSelectorParser::currentChar() const {
    if (position_ >= selector_.length()) return '\0';
    return selector_[position_];
}

char StyleSelectorParser::peekChar() const {
    if (position_ + 1 >= selector_.length()) return '\0';
    return selector_[position_ + 1];
}

void StyleSelectorParser::advance() {
    if (position_ < selector_.length()) {
        position_++;
    }
}

std::string StyleSelectorParser::parseSelector() {
    std::string result;
    
    while (currentChar() != '\0') {
        skipWhitespace();
        
        if (currentChar() == '.') {
            result += parseClass();
        } else if (currentChar() == '#') {
            result += parseId();
        } else if (currentChar() == '[') {
            result += parseAttribute();
        } else if (currentChar() == ':') {
            result += parsePseudo();
        } else if (currentChar() == ' ') {
            result += " ";
            advance();
        } else if (currentChar() == '>' || currentChar() == '+' || currentChar() == '~') {
            result += parseCombinator();
        } else {
            result += parseElement();
        }
    }
    
    return result;
}

std::string StyleSelectorParser::parseElement() {
    std::string result;
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseClass() {
    std::string result = ".";
    advance(); // 跳过 .
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseId() {
    std::string result = "#";
    advance(); // 跳过 #
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseAttribute() {
    std::string result = "[";
    advance(); // 跳过 [
    
    while (currentChar() != ']' && currentChar() != '\0') {
        result += currentChar();
        advance();
    }
    
    if (currentChar() == ']') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parsePseudo() {
    std::string result = ":";
    advance(); // 跳过 :
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-' || currentChar() == '(') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseCombinator() {
    std::string result;
    
    if (currentChar() == '>') {
        result = " > ";
        advance();
    } else if (currentChar() == '+') {
        result = " + ";
        advance();
    } else if (currentChar() == '~') {
        result = " ~ ";
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseString() {
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string result;
    while (currentChar() != quote && currentChar() != '\0') {
        if (currentChar() == '\\') {
            advance(); // 跳过 \
            if (currentChar() == 'n') {
                result += '\n';
            } else if (currentChar() == 't') {
                result += '\t';
            } else if (currentChar() == 'r') {
                result += '\r';
            } else if (currentChar() == '\\') {
                result += '\\';
            } else if (currentChar() == quote) {
                result += quote;
            } else {
                result += currentChar();
            }
        } else {
            result += currentChar();
        }
        advance();
    }
    
    if (currentChar() == quote) {
        advance(); // 跳过结束引号
    }
    
    return result;
}

std::string StyleSelectorParser::parseIdentifier() {
    std::string result;
    
    while (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string StyleSelectorParser::parseNumber() {
    std::string result;
    
    if (currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    while (std::isdigit(currentChar())) {
        result += currentChar();
        advance();
    }
    
    if (currentChar() == '.') {
        result += currentChar();
        advance();
        while (std::isdigit(currentChar())) {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

// StyleCompiler 实现
StyleCompiler::StyleCompiler() {
    initializeFunctions();
}

std::string StyleCompiler::compile(const std::string& input) {
    return input;
}

std::string StyleCompiler::compileStyleGroup(const StyleGroup& group) {
    std::stringstream result;
    
    result << "." << group.name << " {\n";
    
    for (const auto& property : group.properties) {
        result << "  " << property.name << ": " << compileStyleValue(*property.value) << ";\n";
    }
    
    for (const auto& rule : group.rules) {
        result << "  " << compileStyleRule(*rule) << "\n";
    }
    
    result << "}\n";
    
    return result.str();
}

std::string StyleCompiler::compileStyleRule(const StyleRule& rule) {
    std::stringstream result;
    
    result << rule.selector << " {\n";
    
    for (const auto& property : rule.properties) {
        result << "  " << property.name << ": " << compileStyleValue(*property.value) << ";\n";
    }
    
    for (const auto& nestedRule : rule.nestedRules) {
        result << "  " << compileStyleRule(*nestedRule) << "\n";
    }
    
    result << "}";
    
    return result.str();
}

std::string StyleCompiler::compileStyleProperty(const StyleProperty& property) {
    std::stringstream result;
    
    result << property.name << ": " << compileStyleValue(*property.value);
    
    if (property.important) {
        result << " !important";
    }
    
    result << ";";
    
    return result.str();
}

std::string StyleCompiler::compileStyleValue(const StyleValue& value) {
    switch (value.type) {
        case StyleValueType::LITERAL:
            return value.value;
            
        case StyleValueType::REFERENCE:
            return resolveReference(value.value);
            
        case StyleValueType::EXPRESSION:
            return evaluateExpression(value);
            
        case StyleValueType::FUNCTION:
            return evaluateExpression(value);
            
        case StyleValueType::CONDITIONAL:
            return evaluateExpression(value);
            
        default:
            return value.value;
    }
}

void StyleCompiler::setContext(const StyleContext& context) {
    context_ = context;
}

StyleContext& StyleCompiler::getContext() {
    return context_;
}

void StyleCompiler::initializeFunctions() {
    functions_["calc"] = [](const std::vector<std::string>& args) -> std::string {
        std::string result = "calc(";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) result += " ";
            result += args[i];
        }
        result += ")";
        return result;
    };
    
    functions_["var"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.empty()) return "";
        return "var(--" + args[0] + ")";
    };
    
    functions_["rgb"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 3) return "rgb(0, 0, 0)";
        return "rgb(" + args[0] + ", " + args[1] + ", " + args[2] + ")";
    };
    
    functions_["rgba"] = [](const std::vector<std::string>& args) -> std::string {
        if (args.size() < 4) return "rgba(0, 0, 0, 1)";
        return "rgba(" + args[0] + ", " + args[1] + ", " + args[2] + ", " + args[3] + ")";
    };
}

std::string StyleCompiler::resolveReference(const std::string& reference) {
    auto it = context_.variables.find(reference);
    if (it != context_.variables.end()) {
        return compileStyleValue(*it->second);
    }
    return reference;
}

std::string StyleCompiler::evaluateExpression(const StyleValue& value) {
    if (value.type == StyleValueType::EXPRESSION) {
        if (value.value == "+") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " + " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "-") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " - " + evaluateExpression(*value.arguments[1]);
            } else if (value.arguments.size() == 1) {
                return "-" + evaluateExpression(*value.arguments[0]);
            }
        } else if (value.value == "*") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " * " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "/") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " / " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "==") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " == " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "!=") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " != " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "<") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " < " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == ">") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " > " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "<=") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " <= " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == ">=") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " >= " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "&&") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " && " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "||") {
            if (value.arguments.size() >= 2) {
                return evaluateExpression(*value.arguments[0]) + " || " + evaluateExpression(*value.arguments[1]);
            }
        } else if (value.value == "!") {
            if (value.arguments.size() >= 1) {
                return "!" + evaluateExpression(*value.arguments[0]);
            }
        }
    } else if (value.type == StyleValueType::FUNCTION) {
        auto it = functions_.find(value.value);
        if (it != functions_.end()) {
            std::vector<std::string> args;
            for (const auto& arg : value.arguments) {
                args.push_back(evaluateExpression(*arg));
            }
            return it->second(args);
        }
    } else if (value.type == StyleValueType::CONDITIONAL) {
        if (value.condition && value.trueValue && value.falseValue) {
            return evaluateExpression(*value.condition) + " ? " + 
                   evaluateExpression(*value.trueValue) + " : " + 
                   evaluateExpression(*value.falseValue);
        }
    }
    
    return value.value;
}

std::string StyleCompiler::generateCSS(const std::string& selector, const std::vector<StyleProperty>& properties) {
    std::stringstream result;
    
    result << selector << " {\n";
    
    for (const auto& property : properties) {
        result << "  " << compileStyleProperty(property) << "\n";
    }
    
    result << "}";
    
    return result.str();
}

std::string StyleCompiler::generateClass(const std::string& name) {
    return "." + name;
}

std::string StyleCompiler::generateId(const std::string& name) {
    return "#" + name;
}

// StyleManager 实现
StyleManager::StyleManager() {}

void StyleManager::addStyleGroup(const std::string& name, const StyleGroup& group) {
    groups_[name] = std::make_shared<StyleGroup>(group);
}

void StyleManager::addStyleRule(const std::string& selector, const StyleRule& rule) {
    rules_[selector].push_back(std::make_shared<StyleRule>(rule));
}

void StyleManager::addStyleProperty(const std::string& selector, const StyleProperty& property) {
    if (rules_.find(selector) == rules_.end()) {
        rules_[selector] = std::vector<std::shared_ptr<StyleRule>>();
    }
    
    if (rules_[selector].empty()) {
        rules_[selector].push_back(std::make_shared<StyleRule>(selector));
    }
    
    rules_[selector][0]->properties.push_back(property);
}

std::shared_ptr<StyleGroup> StyleManager::getStyleGroup(const std::string& name) {
    auto it = groups_.find(name);
    if (it != groups_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<StyleRule>> StyleManager::getStyleRules(const std::string& selector) {
    auto it = rules_.find(selector);
    if (it != rules_.end()) {
        return it->second;
    }
    return std::vector<std::shared_ptr<StyleRule>>();
}

std::string StyleManager::generateCSS() {
    std::stringstream result;
    
    for (const auto& group : groups_) {
        result << compiler_.compileStyleGroup(*group.second) << "\n";
    }
    
    for (const auto& rule : rules_) {
        for (const auto& r : rule.second) {
            result << compiler_.compileStyleRule(*r) << "\n";
        }
    }
    
    return result.str();
}

std::string StyleManager::generateCSSForSelector(const std::string& selector) {
    std::stringstream result;
    
    auto rules = getStyleRules(selector);
    for (const auto& rule : rules) {
        result << compiler_.compileStyleRule(*rule) << "\n";
    }
    
    return result.str();
}

void StyleManager::setContext(const StyleContext& context) {
    context_ = context;
    compiler_.setContext(context);
}

StyleContext& StyleManager::getContext() {
    return context_;
}

void StyleManager::mergeContext(const StyleContext& other) {
    for (const auto& var : other.variables) {
        context_.variables[var.first] = var.second;
    }
    
    for (const auto& group : other.groups) {
        context_.groups[group.first] = group.second;
    }
    
    for (const auto& cls : other.classStack) {
        context_.classStack.push_back(cls);
    }
    
    for (const auto& id : other.idStack) {
        context_.idStack.push_back(id);
    }
}

std::string StyleManager::resolveSelector(const std::string& selector) {
    return selector;
}

std::string StyleManager::generateAutoClass(const std::string& name) {
    return "." + name;
}

std::string StyleManager::generateAutoId(const std::string& name) {
    return "#" + name;
}

} // namespace style
} // namespace chtl