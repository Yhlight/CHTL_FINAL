#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <variant>

namespace chtl {
namespace constraint_system {

// 约束类型
enum class ConstraintType {
    PRECISE,        // 精确约束
    TYPE,           // 类型约束
    GLOBAL          // 全局约束
};

// 约束目标类型
enum class ConstraintTarget {
    HTML_ELEMENT,   // HTML元素
    CUSTOM_OBJECT,  // 自定义对象
    TEMPLATE_OBJECT,// 模板对象
    TYPE            // 类型
};

// 约束条件
struct ConstraintCondition {
    std::string name;
    std::string value;
    std::string operator_;  // 操作符：==, !=, <, >, <=, >=, in, not_in
    
    ConstraintCondition(const std::string& n = "", const std::string& v = "", const std::string& op = "==")
        : name(n), value(v), operator_(op) {}
};

// 约束规则
struct ConstraintRule {
    std::string name;
    ConstraintType type;
    ConstraintTarget target;
    std::vector<ConstraintCondition> conditions;
    std::string scope;
    bool isActive;
    
    ConstraintRule(const std::string& n = "", ConstraintType t = ConstraintType::PRECISE, ConstraintTarget tg = ConstraintTarget::HTML_ELEMENT)
        : name(n), type(t), target(tg), isActive(true) {}
};

// 约束解析器
class ConstraintParser {
public:
    ConstraintParser(const std::string& input);
    
    std::vector<std::shared_ptr<ConstraintRule>> parse();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<ConstraintRule> parseConstraintRule();
    std::vector<ConstraintCondition> parseConditions();
    ConstraintCondition parseCondition();
    std::string parseString();
    std::string parseIdentifier();
    std::string parseOperator();
    
    bool isKeyword(const std::string& word) const;
    bool isConstraintKeyword(const std::string& word) const;
};

// 约束编译器
class ConstraintCompiler {
public:
    ConstraintCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    std::string compileConstraintRule(const ConstraintRule& rule);
    std::string compileCondition(const ConstraintCondition& condition);
    
    void addRule(std::shared_ptr<ConstraintRule> rule);
    std::shared_ptr<ConstraintRule> getRule(const std::string& name);
    
    bool validate(const std::string& target, const std::map<std::string, std::string>& attributes);
    bool validatePreciseConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes);
    bool validateTypeConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes);
    bool validateGlobalConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes);
    
public:
    std::map<std::string, std::shared_ptr<ConstraintRule>> rules_;
    
    std::string generateValidationCode(const ConstraintRule& rule);
    std::string generateConditionCode(const ConstraintCondition& condition);
    bool evaluateCondition(const ConstraintCondition& condition, const std::map<std::string, std::string>& attributes);
};

// 约束管理器
class ConstraintManager {
public:
    ConstraintManager();
    
    void addRule(std::shared_ptr<ConstraintRule> rule);
    std::shared_ptr<ConstraintRule> getRule(const std::string& name);
    
    std::string generateCode(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    std::string generateValidationCode();
    
    bool validateTarget(const std::string& target, const std::map<std::string, std::string>& attributes);
    bool validateHTMLElement(const std::string& element, const std::map<std::string, std::string>& attributes);
    bool validateCustomObject(const std::string& object, const std::map<std::string, std::string>& attributes);
    bool validateTemplateObject(const std::string& object, const std::map<std::string, std::string>& attributes);
    bool validateType(const std::string& type, const std::map<std::string, std::string>& attributes);
    
    void clear();
    size_t getRuleCount() const;
    
private:
    ConstraintCompiler compiler_;
    
    std::string processConstraintScope(const std::string& scope);
    std::string resolveConstraintDependencies(const std::string& content);
    std::string validateConstraintParameters(const std::string& ruleName, const std::map<std::string, std::string>& parameters);
};

// 约束验证器
class ConstraintValidator {
public:
    ConstraintValidator();
    
    void setRules(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    
    bool validate(const std::string& target, const std::map<std::string, std::string>& attributes);
    std::vector<std::string> getValidationErrors();
    std::vector<std::string> getValidationWarnings();
    
    void clearErrors();
    void clearWarnings();
    
private:
    std::vector<std::shared_ptr<ConstraintRule>> rules_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    bool checkConstraint(const ConstraintRule& rule, const std::string& target, const std::map<std::string, std::string>& attributes);
};

} // namespace constraint_system
} // namespace chtl