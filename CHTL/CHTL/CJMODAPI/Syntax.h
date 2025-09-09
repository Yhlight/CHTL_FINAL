#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// 语法类型枚举
enum class SyntaxType {
    Keyword,        // 关键字
    Operator,       // 运算符
    Delimiter,      // 分隔符
    Identifier,     // 标识符
    Literal,        // 字面量
    Comment,        // 注释
    Whitespace,     // 空白
    Newline,        // 换行
    EOF_TOKEN       // 文件结束
};

// 语法规则结构
struct SyntaxRule {
    std::string pattern;         // 匹配模式
    SyntaxType type;            // 语法类型
    int priority;               // 优先级
    bool isRequired;            // 是否必需
    std::string description;    // 描述
};

// 语法类
class Syntax {
public:
    Syntax();
    ~Syntax() = default;
    
    // 规则管理
    void addRule(const SyntaxRule& rule);
    void removeRule(const std::string& pattern);
    bool hasRule(const std::string& pattern) const;
    SyntaxRule getRule(const std::string& pattern) const;
    std::vector<SyntaxRule> getAllRules() const;
    
    // 语法验证
    bool validateSyntax(const std::string& input) const;
    std::vector<std::string> getSyntaxErrors(const std::string& input) const;
    
    // 语法分析
    std::vector<SyntaxRule> analyze(const std::string& input) const;
    std::string getSyntaxType(const std::string& token) const;
    
    // 规则搜索
    std::vector<SyntaxRule> findRulesByType(SyntaxType type) const;
    std::vector<SyntaxRule> findRulesByPattern(const std::string& pattern) const;
    
    // 序列化
    std::string toJSON() const;
    std::string toString() const;

private:
    std::map<std::string, SyntaxRule> rules;
    std::vector<SyntaxRule> ruleList;
    
    void initializeDefaultRules();
    bool matchesPattern(const std::string& input, const std::string& pattern) const;
};

} // namespace CHTL