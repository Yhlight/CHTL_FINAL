#include "Syntax.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

Syntax::Syntax() {
    initializeDefaultRules();
}

void Syntax::addRule(const SyntaxRule& rule) {
    rules[rule.pattern] = rule;
    ruleList.push_back(rule);
    std::cout << "Added syntax rule: " << rule.pattern << " (type: " << static_cast<int>(rule.type) << ")" << std::endl;
}

void Syntax::removeRule(const std::string& pattern) {
    auto it = rules.find(pattern);
    if (it != rules.end()) {
        rules.erase(it);
        ruleList.erase(std::remove_if(ruleList.begin(), ruleList.end(),
            [&pattern](const SyntaxRule& rule) {
                return rule.pattern == pattern;
            }), ruleList.end());
        std::cout << "Removed syntax rule: " << pattern << std::endl;
    }
}

bool Syntax::hasRule(const std::string& pattern) const {
    return rules.find(pattern) != rules.end();
}

SyntaxRule Syntax::getRule(const std::string& pattern) const {
    auto it = rules.find(pattern);
    if (it != rules.end()) {
        return it->second;
    }
    
    // 返回空规则
    SyntaxRule emptyRule;
    emptyRule.pattern = "";
    emptyRule.type = SyntaxType::Identifier;
    emptyRule.priority = 0;
    emptyRule.isRequired = false;
    emptyRule.description = "";
    return emptyRule;
}

std::vector<SyntaxRule> Syntax::getAllRules() const {
    return ruleList;
}

bool Syntax::validateSyntax(const std::string& input) const {
    return getSyntaxErrors(input).empty();
}

std::vector<std::string> Syntax::getSyntaxErrors(const std::string& input) const {
    std::vector<std::string> errors;
    
    // 简化的语法验证
    if (input.empty()) {
        errors.push_back("Input is empty");
        return errors;
    }
    
    // 检查基本语法规则
    for (const auto& rule : ruleList) {
        if (rule.isRequired && !matchesPattern(input, rule.pattern)) {
            errors.push_back("Required pattern not found: " + rule.pattern);
        }
    }
    
    return errors;
}

std::vector<SyntaxRule> Syntax::analyze(const std::string& input) const {
    std::vector<SyntaxRule> matchedRules;
    
    for (const auto& rule : ruleList) {
        if (matchesPattern(input, rule.pattern)) {
            matchedRules.push_back(rule);
        }
    }
    
    // 按优先级排序
    std::sort(matchedRules.begin(), matchedRules.end(),
        [](const SyntaxRule& a, const SyntaxRule& b) {
            return a.priority > b.priority;
        });
    
    return matchedRules;
}

std::string Syntax::getSyntaxType(const std::string& token) const {
    auto rules = analyze(token);
    if (!rules.empty()) {
        switch (rules[0].type) {
            case SyntaxType::Keyword: return "keyword";
            case SyntaxType::Operator: return "operator";
            case SyntaxType::Delimiter: return "delimiter";
            case SyntaxType::Identifier: return "identifier";
            case SyntaxType::Literal: return "literal";
            case SyntaxType::Comment: return "comment";
            case SyntaxType::Whitespace: return "whitespace";
            case SyntaxType::Newline: return "newline";
            case SyntaxType::EOF_TOKEN: return "eof";
            default: return "unknown";
        }
    }
    return "unknown";
}

std::vector<SyntaxRule> Syntax::findRulesByType(SyntaxType type) const {
    std::vector<SyntaxRule> result;
    for (const auto& rule : ruleList) {
        if (rule.type == type) {
            result.push_back(rule);
        }
    }
    return result;
}

std::vector<SyntaxRule> Syntax::findRulesByPattern(const std::string& pattern) const {
    std::vector<SyntaxRule> result;
    for (const auto& rule : ruleList) {
        if (rule.pattern.find(pattern) != std::string::npos) {
            result.push_back(rule);
        }
    }
    return result;
}

std::string Syntax::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"rules\": [\n";
    for (size_t i = 0; i < ruleList.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "    {\n";
        ss << "      \"pattern\": \"" << ruleList[i].pattern << "\",\n";
        ss << "      \"type\": " << static_cast<int>(ruleList[i].type) << ",\n";
        ss << "      \"priority\": " << ruleList[i].priority << ",\n";
        ss << "      \"isRequired\": " << (ruleList[i].isRequired ? "true" : "false") << ",\n";
        ss << "      \"description\": \"" << ruleList[i].description << "\"\n";
        ss << "    }";
    }
    ss << "\n  ]\n";
    ss << "}";
    return ss.str();
}

std::string Syntax::toString() const {
    std::stringstream ss;
    ss << "Syntax Rules (" << ruleList.size() << "):\n";
    for (const auto& rule : ruleList) {
        ss << "  - " << rule.pattern << " (type: " << static_cast<int>(rule.type) 
           << ", priority: " << rule.priority << ")\n";
    }
    return ss.str();
}

void Syntax::initializeDefaultRules() {
    // 关键字规则
    addRule({"\\b(fileloader|listen|delegate|animate|vir|router|util|iNeverAway)\\b", 
             SyntaxType::Keyword, 100, true, "CHTL JS keywords"});
    
    // 运算符规则
    addRule({"->", SyntaxType::Operator, 90, false, "Arrow operator"});
    addRule({"\\+\\+|--", SyntaxType::Operator, 80, false, "Increment/decrement operators"});
    addRule({"\\+|-|\\*|/|%", SyntaxType::Operator, 70, false, "Arithmetic operators"});
    addRule({"==|!=|<=|>=|<|>", SyntaxType::Operator, 60, false, "Comparison operators"});
    addRule({"&&|\\|\\|", SyntaxType::Operator, 50, false, "Logical operators"});
    
    // 分隔符规则
    addRule({"\\{|\\}", SyntaxType::Delimiter, 95, false, "Braces"});
    addRule({"\\[|\\]", SyntaxType::Delimiter, 90, false, "Brackets"});
    addRule{"\\(|\\)", SyntaxType::Delimiter, 85, false, "Parentheses"};
    addRule{";", SyntaxType::Delimiter, 80, false, "Semicolon"};
    addRule{",", SyntaxType::Delimiter, 75, false, "Comma"};
    addRule{":", SyntaxType::Delimiter, 70, false, "Colon"};
    
    // 标识符规则
    addRule{"\\b[a-zA-Z_][a-zA-Z0-9_]*\\b", SyntaxType::Identifier, 40, false, "Identifiers"};
    
    // 字面量规则
    addRule{"\\b\\d+\\.\\d+\\b", SyntaxType::Literal, 30, false, "Floating point numbers"};
    addRule{"\\b\\d+\\b", SyntaxType::Literal, 25, false, "Integer numbers"};
    addRule{"\"[^\"]*\"", SyntaxType::Literal, 20, false, "Double quoted strings"};
    addRule{"'[^']*'", SyntaxType::Literal, 15, false, "Single quoted strings"};
    addRule{"\\b(true|false)\\b", SyntaxType::Literal, 10, false, "Boolean literals"};
    
    // 注释规则
    addRule{"//.*", SyntaxType::Comment, 5, false, "Single line comments"};
    addRule{"/\\*[\\s\\S]*?\\*/", SyntaxType::Comment, 5, false, "Multi-line comments"};
    
    // 空白规则
    addRule{"\\s+", SyntaxType::Whitespace, 1, false, "Whitespace"};
    addRule{"\\n", SyntaxType::Newline, 2, false, "Newline"};
}

bool Syntax::matchesPattern(const std::string& input, const std::string& pattern) const {
    try {
        std::regex regexPattern(pattern);
        return std::regex_search(input, regexPattern);
    } catch (const std::regex_error& e) {
        std::cerr << "Regex error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace CHTL