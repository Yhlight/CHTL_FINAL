#ifndef CHTL_STYLE_PROCESSOR_H
#define CHTL_STYLE_PROCESSOR_H

#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLProperty/CHTLPropertyReference.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLStyleProcessor {
private:
    // 配置
    bool debugMode;
    bool strictMode;
    bool autoAddClass;
    bool autoAddId;
    
    // 样式上下文
    std::map<std::string, std::string> elementProperties;
    std::map<std::string, std::shared_ptr<ElementNode>> elementMap;
    
    // 属性引用处理器
    std::unique_ptr<class CHTLPropertyReference> propertyReference;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 属性运算
    std::string evaluateArithmeticExpression(const std::string& expression) const;
    std::string evaluateConditionalExpression(const std::string& expression) const;
    std::string evaluateLogicalExpression(const std::string& expression) const;
    std::string evaluateEqualityExpression(const std::string& expression) const;
    std::string evaluateRelationalExpression(const std::string& expression) const;
    std::string evaluateAdditiveExpression(const std::string& expression) const;
    std::string evaluateMultiplicativeExpression(const std::string& expression) const;
    std::string evaluateUnaryExpression(const std::string& expression) const;
    std::string evaluatePrimaryExpression(const std::string& expression) const;
    
    // 属性引用
    std::string resolvePropertyReference(const std::string& reference) const;
    std::string resolveElementProperty(const std::string& selector, const std::string& property) const;
    std::string resolveSelector(const std::string& selector) const;
    
    // 选择器处理
    std::string processClassSelector(const std::string& className) const;
    std::string processIdSelector(const std::string& id) const;
    std::string processTagSelector(const std::string& tagName) const;
    std::string processDescendantSelector(const std::string& selector) const;
    std::string processPseudoClassSelector(const std::string& pseudoClass) const;
    std::string processPseudoElementSelector(const std::string& pseudoElement) const;
    std::string processContextReference(const std::string& context) const;
    
    // 上下文推导
    std::string processContextDerivation(const std::string& context, std::shared_ptr<ElementNode> element) const;
    std::string processAmpersandReference(const std::string& reference, std::shared_ptr<ElementNode> element) const;
    
    // 自动化处理
    void autoAddClassToElement(std::shared_ptr<ElementNode> element, const std::string& className) const;
    void autoAddIdToElement(std::shared_ptr<ElementNode> element, const std::string& id) const;
    void processAutoClassAndId(std::shared_ptr<StyleNode> styleNode, std::shared_ptr<ElementNode> element) const;
    
    // 样式规则处理
    std::string processStyleRule(const std::string& selector, const std::map<std::string, std::string>& properties) const;
    std::string processInlineStyle(const std::map<std::string, std::string>& properties) const;
    
    // 表达式解析
    std::vector<std::string> tokenizeExpression(const std::string& expression) const;
    bool isOperator(const std::string& token) const;
    bool isNumber(const std::string& token) const;
    bool isString(const std::string& token) const;
    bool isIdentifier(const std::string& token) const;
    
    // 单位处理
    std::string parseValueWithUnit(const std::string& value) const;
    std::string convertUnit(const std::string& value, const std::string& fromUnit, const std::string& toUnit) const;
    bool isValidUnit(const std::string& unit) const;
    
    // 颜色处理
    std::string parseColor(const std::string& color) const;
    std::string convertColorFormat(const std::string& color) const;
    bool isValidColor(const std::string& color) const;

public:
    CHTLStyleProcessor();
    ~CHTLStyleProcessor() = default;
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setAutoAddClass(bool autoAdd) { autoAddClass = autoAdd; }
    void setAutoAddId(bool autoAdd) { autoAddId = autoAdd; }
    
    // 主要处理方法
    std::string processStyleBlock(std::shared_ptr<StyleNode> styleNode, std::shared_ptr<ElementNode> element);
    std::string processGlobalStyleBlock(std::shared_ptr<StyleNode> styleNode);
    
    // 样式收集
    void collectElementProperties(std::shared_ptr<ElementNode> element);
    void clearElementProperties();
    
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
    
    // 局部样式块处理方法
    std::string processPropertyValue(const std::string& value) const;
    std::string processPropertyReferences(const std::string& value) const;
    std::string processArithmeticExpressions(const std::string& value) const;
    std::string processConditionalExpressions(const std::string& value) const;
    std::string processLogicalExpressions(const std::string& value) const;
    bool evaluateCondition(const std::string& condition) const;
};

} // namespace CHTL

#endif // CHTL_STYLE_PROCESSOR_H