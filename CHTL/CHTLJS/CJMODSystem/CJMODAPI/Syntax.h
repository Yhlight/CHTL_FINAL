#ifndef CJMOD_SYNTAX_H
#define CJMOD_SYNTAX_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {
namespace CJMOD {

class Arg;

class Syntax {
public:
    // 语法分析
    static std::shared_ptr<Arg> analyze(const std::string& syntax);
    
    // 对象判断
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
    
    // 语法验证
    static bool isValidSyntax(const std::string& syntax);
    static bool hasPlaceholder(const std::string& syntax);
    static std::vector<std::string> extractPlaceholders(const std::string& syntax);
    
    // 语法转换
    static std::string normalizeSyntax(const std::string& syntax);
    static std::string escapeSpecialChars(const std::string& syntax);
    static std::string unescapeSpecialChars(const std::string& syntax);
    
    // 语法匹配
    static bool matchPattern(const std::string& code, const std::string& pattern);
    static std::vector<std::string> extractMatches(const std::string& code, const std::string& pattern);
    
    // 语法分割
    static std::vector<std::string> splitSyntax(const std::string& syntax);
    static std::vector<std::string> splitByPlaceholders(const std::string& syntax);
    
    // 语法合并
    static std::string mergeSyntax(const std::vector<std::string>& parts);
    static std::string mergeWithPlaceholders(const std::vector<std::string>& parts, 
                                           const std::vector<std::string>& placeholders);
    
private:
    // 内部辅助方法
    static bool isBalanced(const std::string& code);
    static bool isQuoted(const std::string& text);
    static bool isNumeric(const std::string& text);
    static bool isIdentifier(const std::string& text);
    
    static std::string trimWhitespace(const std::string& text);
    static std::vector<std::string> tokenize(const std::string& syntax);
    static std::string parseToken(const std::string& token);
    
    static bool isPlaceholderToken(const std::string& token);
    static bool isOperatorToken(const std::string& token);
    static bool isDelimiterToken(const std::string& token);
    
    // 语法规则
    static const std::vector<std::string> PLACEHOLDER_PREFIXES;
    static const std::vector<std::string> OPERATOR_TOKENS;
    static const std::vector<std::string> DELIMITER_TOKENS;
    static const std::vector<std::string> KEYWORD_TOKENS;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_SYNTAX_H