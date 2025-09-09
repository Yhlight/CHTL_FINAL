#ifndef CHTL_PROPERTY_REFERENCE_H
#define CHTL_PROPERTY_REFERENCE_H

#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CHTLPropertyReference {
private:
    // 配置
    bool debugMode;
    bool strictMode;
    bool enableChaining;
    bool enableOptionalChaining;
    
    // 元素映射
    std::map<std::string, std::shared_ptr<ElementNode>> elementMap;
    std::map<std::string, std::map<std::string, std::string>> elementProperties;
    
    // 样式映射
    std::map<std::string, std::shared_ptr<StyleNode>> styleMap;
    std::map<std::string, std::map<std::string, std::string>> styleProperties;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 选择器解析
    std::string parseSelector(const std::string& selector) const;
    std::vector<std::string> parseComplexSelector(const std::string& selector) const;
    std::string parseSimpleSelector(const std::string& selector) const;
    
    // 选择器类型判断
    bool isIdSelector(const std::string& selector) const;
    bool isClassSelector(const std::string& selector) const;
    bool isTagSelector(const std::string& selector) const;
    bool isAttributeSelector(const std::string& selector) const;
    bool isPseudoSelector(const std::string& selector) const;
    bool isDescendantSelector(const std::string& selector) const;
    bool isChildSelector(const std::string& selector) const;
    bool isSiblingSelector(const std::string& selector) const;
    
    // 选择器匹配
    bool matchesElement(const std::string& selector, std::shared_ptr<ElementNode> element) const;
    bool matchesIdSelector(const std::string& id, std::shared_ptr<ElementNode> element) const;
    bool matchesClassSelector(const std::string& className, std::shared_ptr<ElementNode> element) const;
    bool matchesTagSelector(const std::string& tagName, std::shared_ptr<ElementNode> element) const;
    bool matchesAttributeSelector(const std::string& attribute, std::shared_ptr<ElementNode> element) const;
    bool matchesPseudoSelector(const std::string& pseudo, std::shared_ptr<ElementNode> element) const;
    
    // 属性解析
    std::string parseProperty(const std::string& property) const;
    std::string parseAttributeProperty(const std::string& property) const;
    std::string parseStyleProperty(const std::string& property) const;
    std::string parseComputedProperty(const std::string& property) const;
    
    // 属性值处理
    std::string processPropertyValue(const std::string& value) const;
    std::string processExpression(const std::string& expression) const;
    std::string processArithmeticExpression(const std::string& expression) const;
    std::string processConditionalExpression(const std::string& expression) const;
    std::string processLogicalExpression(const std::string& expression) const;
    
    // 链式调用
    std::string processChainedCall(const std::string& chain) const;
    std::string processOptionalChaining(const std::string& chain) const;
    std::string processMethodCall(const std::string& method, const std::string& args) const;
    
    // 属性查找
    std::string findElementProperty(const std::string& selector, const std::string& property) const;
    std::string findStyleProperty(const std::string& selector, const std::string& property) const;
    std::string findComputedProperty(const std::string& selector, const std::string& property) const;
    
    // 元素查找
    std::shared_ptr<ElementNode> findElement(const std::string& selector) const;
    std::vector<std::shared_ptr<ElementNode>> findElements(const std::string& selector) const;
    
    // 样式查找
    std::shared_ptr<StyleNode> findStyle(const std::string& selector) const;
    std::vector<std::shared_ptr<StyleNode>> findStyles(const std::string& selector) const;
    
    // 属性验证
    bool isValidProperty(const std::string& property) const;
    bool isValidSelector(const std::string& selector) const;
    bool isValidValue(const std::string& value) const;
    
    // 类型转换
    std::string convertToString(const std::string& value) const;
    double convertToNumber(const std::string& value) const;
    bool convertToBoolean(const std::string& value) const;
    
    // 单位处理
    std::string parseValueWithUnit(const std::string& value) const;
    std::string convertUnit(const std::string& value, const std::string& fromUnit, const std::string& toUnit) const;
    bool isValidUnit(const std::string& unit) const;
    
    // 颜色处理
    std::string parseColor(const std::string& color) const;
    std::string convertColorFormat(const std::string& color) const;
    bool isValidColor(const std::string& color) const;
    
    // 表达式解析
    std::vector<std::string> tokenizeExpression(const std::string& expression) const;
    bool isOperator(const std::string& token) const;
    bool isNumber(const std::string& token) const;
    bool isString(const std::string& token) const;
    bool isIdentifier(const std::string& token) const;
    bool isFunction(const std::string& token) const;
    
    // 函数处理
    std::string processFunction(const std::string& function, const std::vector<std::string>& args) const;
    std::string processMathFunction(const std::string& function, const std::vector<std::string>& args) const;
    std::string processStringFunction(const std::string& function, const std::vector<std::string>& args) const;
    std::string processColorFunction(const std::string& function, const std::vector<std::string>& args) const;
    std::string processUnitFunction(const std::string& function, const std::vector<std::string>& args) const;

public:
    CHTLPropertyReference();
    ~CHTLPropertyReference() = default;
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setEnableChaining(bool enable) { enableChaining = enable; }
    void setEnableOptionalChaining(bool enable) { enableOptionalChaining = enable; }
    
    // 元素和样式注册
    void registerElement(const std::string& id, std::shared_ptr<ElementNode> element);
    void registerStyle(const std::string& selector, std::shared_ptr<StyleNode> style);
    void unregisterElement(const std::string& id);
    void unregisterStyle(const std::string& selector);
    
    // 属性引用解析
    std::string resolvePropertyReference(const std::string& reference) const;
    std::string resolveElementProperty(const std::string& selector, const std::string& property) const;
    std::string resolveStyleProperty(const std::string& selector, const std::string& property) const;
    std::string resolveComputedProperty(const std::string& selector, const std::string& property) const;
    
    // 链式调用解析
    std::string resolveChainedCall(const std::string& chain) const;
    std::string resolveOptionalChaining(const std::string& chain) const;
    
    // 表达式求值
    std::string evaluateExpression(const std::string& expression) const;
    std::string evaluateArithmeticExpression(const std::string& expression) const;
    std::string evaluateConditionalExpression(const std::string& expression) const;
    std::string evaluateLogicalExpression(const std::string& expression) const;
    
    // 属性设置
    void setElementProperty(const std::string& selector, const std::string& property, const std::string& value);
    void setStyleProperty(const std::string& selector, const std::string& property, const std::string& value);
    
    // 属性获取
    std::string getElementProperty(const std::string& selector, const std::string& property) const;
    std::string getStyleProperty(const std::string& selector, const std::string& property) const;
    
    // 批量操作
    void setElementProperties(const std::string& selector, const std::map<std::string, std::string>& properties);
    void setStyleProperties(const std::string& selector, const std::map<std::string, std::string>& properties);
    
    // 获取所有属性
    std::map<std::string, std::string> getAllElementProperties(const std::string& selector) const;
    std::map<std::string, std::string> getAllStyleProperties(const std::string& selector) const;
    
    // 属性存在性检查
    bool hasElementProperty(const std::string& selector, const std::string& property) const;
    bool hasStyleProperty(const std::string& selector, const std::string& property) const;
    
    // 属性删除
    void removeElementProperty(const std::string& selector, const std::string& property);
    void removeStyleProperty(const std::string& selector, const std::string& property);
    
    // 清除所有属性
    void clearElementProperties(const std::string& selector);
    void clearStyleProperties(const std::string& selector);
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 重置处理器
    void reset();
    
    // 验证
    bool validate() const;
    
    // 调试信息
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_PROPERTY_REFERENCE_H