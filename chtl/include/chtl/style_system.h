#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <functional>

namespace chtl {
namespace style {

// 样式值类型
enum class StyleValueType {
    LITERAL,        // 字面量值
    REFERENCE,      // 引用属性
    EXPRESSION,     // 表达式
    FUNCTION,       // 函数调用
    CONDITIONAL     // 条件表达式
};

// 样式值
struct StyleValue {
    StyleValueType type;
    std::string value;
    std::vector<std::shared_ptr<StyleValue>> arguments;
    std::shared_ptr<StyleValue> condition;
    std::shared_ptr<StyleValue> trueValue;
    std::shared_ptr<StyleValue> falseValue;
    
    StyleValue(StyleValueType t, const std::string& v = "")
        : type(t), value(v) {}
};

// 样式属性
struct StyleProperty {
    std::string name;
    std::shared_ptr<StyleValue> value;
    bool important;
    bool autoClass;
    bool autoId;
    
    StyleProperty(const std::string& n, std::shared_ptr<StyleValue> v, bool imp = false)
        : name(n), value(v), important(imp), autoClass(false), autoId(false) {}
};

// 样式规则
struct StyleRule {
    std::string selector;
    std::vector<StyleProperty> properties;
    std::vector<std::shared_ptr<StyleRule>> nestedRules;
    std::string parentSelector;
    
    StyleRule(const std::string& sel = "") : selector(sel) {}
};

// 样式组
struct StyleGroup {
    std::string name;
    std::vector<StyleProperty> properties;
    std::vector<std::shared_ptr<StyleRule>> rules;
    std::map<std::string, std::shared_ptr<StyleGroup>> nestedGroups;
    std::string parentGroup;
    
    StyleGroup(const std::string& n = "") : name(n) {}
};

// 样式上下文
struct StyleContext {
    std::map<std::string, std::shared_ptr<StyleValue>> variables;
    std::map<std::string, std::shared_ptr<StyleGroup>> groups;
    std::vector<std::string> classStack;
    std::vector<std::string> idStack;
    std::string currentElement;
    std::string currentAttribute;
    
    StyleContext() = default;
};

// 样式表达式解析器
class StyleExpressionParser {
public:
    StyleExpressionParser(const std::string& expression, StyleContext& context);
    
    std::shared_ptr<StyleValue> parse();
    
private:
    std::string expression_;
    size_t position_;
    StyleContext& context_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<StyleValue> parseExpression();
    std::shared_ptr<StyleValue> parseConditional();
    std::shared_ptr<StyleValue> parseLogicalOr();
    std::shared_ptr<StyleValue> parseLogicalAnd();
    std::shared_ptr<StyleValue> parseEquality();
    std::shared_ptr<StyleValue> parseComparison();
    std::shared_ptr<StyleValue> parseAddition();
    std::shared_ptr<StyleValue> parseMultiplication();
    std::shared_ptr<StyleValue> parseUnary();
    std::shared_ptr<StyleValue> parsePrimary();
    
    std::shared_ptr<StyleValue> parseFunction();
    std::shared_ptr<StyleValue> parseReference();
    std::shared_ptr<StyleValue> parseLiteral();
    std::shared_ptr<StyleValue> parseString();
    std::shared_ptr<StyleValue> parseNumber();
    std::shared_ptr<StyleValue> parseIdentifier();
    
    std::string parseStringLiteral();
    std::string parseNumberLiteral();
    std::string parseIdentifierLiteral();
    
    bool isOperator(const std::string& op) const;
    bool isKeyword(const std::string& word) const;
    int getPrecedence(const std::string& op) const;
    bool isRightAssociative(const std::string& op) const;
};

// 样式选择器解析器
class StyleSelectorParser {
public:
    StyleSelectorParser(const std::string& selector, StyleContext& context);
    
    std::string parse();
    
private:
    std::string selector_;
    size_t position_;
    StyleContext& context_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::string parseSelector();
    std::string parseElement();
    std::string parseClass();
    std::string parseId();
    std::string parseAttribute();
    std::string parsePseudo();
    std::string parseCombinator();
    
    std::string parseString();
    std::string parseIdentifier();
    std::string parseNumber();
};

// 样式编译器
class StyleCompiler {
public:
    StyleCompiler();
    
    std::string compile(const std::string& input);
    std::string compileStyleGroup(const StyleGroup& group);
    std::string compileStyleRule(const StyleRule& rule);
    std::string compileStyleProperty(const StyleProperty& property);
    std::string compileStyleValue(const StyleValue& value);
    
    void setContext(const StyleContext& context);
    StyleContext& getContext();
    
private:
    StyleContext context_;
    std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> functions_;
    
    void initializeFunctions();
    std::string resolveReference(const std::string& reference);
    std::string evaluateExpression(const StyleValue& value);
    std::string generateCSS(const std::string& selector, const std::vector<StyleProperty>& properties);
    std::string generateClass(const std::string& name);
    std::string generateId(const std::string& name);
};

// 样式管理器
class StyleManager {
public:
    StyleManager();
    
    void addStyleGroup(const std::string& name, const StyleGroup& group);
    void addStyleRule(const std::string& selector, const StyleRule& rule);
    void addStyleProperty(const std::string& selector, const StyleProperty& property);
    
    std::shared_ptr<StyleGroup> getStyleGroup(const std::string& name);
    std::vector<std::shared_ptr<StyleRule>> getStyleRules(const std::string& selector);
    
    std::string generateCSS();
    std::string generateCSSForSelector(const std::string& selector);
    
    void setContext(const StyleContext& context);
    StyleContext& getContext();
    
private:
    StyleContext context_;
    std::map<std::string, std::shared_ptr<StyleGroup>> groups_;
    std::map<std::string, std::vector<std::shared_ptr<StyleRule>>> rules_;
    StyleCompiler compiler_;
    
    void mergeContext(const StyleContext& other);
    std::string resolveSelector(const std::string& selector);
    std::string generateAutoClass(const std::string& name);
    std::string generateAutoId(const std::string& name);
};

} // namespace style
} // namespace chtl