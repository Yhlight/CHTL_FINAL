#ifndef CHTL_GENERATOR_COMPLETE_H
#define CHTL_GENERATOR_COMPLETE_H

#include "CHTLGenerator.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 完整的CHTL代码生成器
 * 实现所有CHTL特性，包括局部样式块、属性运算、引用属性等
 */
class CHTLGeneratorComplete : public CHTLGenerator {
public:
    CHTLGeneratorComplete(std::shared_ptr<CHTLContext> context = nullptr);
    ~CHTLGeneratorComplete();
    
    // 重写基类方法
    std::string generate(std::shared_ptr<CHTLBaseNode> ast) override;
    std::string generateDocument(const std::vector<std::shared_ptr<CHTLBaseNode>>& statements) override;
    
    // 局部样式块生成
    std::string generateLocalStyleBlock(std::shared_ptr<CHTLBaseNode> node);
    std::string generateInlineStyle(const std::map<std::string, std::string>& properties) const;
    std::string generateAutoClassStyle(const std::string& className, const std::map<std::string, std::string>& properties) const;
    std::string generateAutoIdStyle(const std::string& idName, const std::map<std::string, std::string>& properties) const;
    std::string generateContextualStyle(const std::string& context, const std::map<std::string, std::string>& properties) const;
    
    // 属性运算生成
    std::string generatePropertyCalculation(const std::string& expression) const;
    std::string generateArithmeticExpression(const std::string& expression) const;
    std::string generateUnitConversion(const std::string& value, const std::string& fromUnit, const std::string& toUnit) const;
    
    // 引用属性生成
    std::string generatePropertyReference(const std::string& selector, const std::string& property) const;
    std::string generateDynamicPropertyReference(const std::string& expression) const;
    std::string generateConditionalProperty(const std::string& condition, const std::string& trueValue, const std::string& falseValue) const;
    
    // 模板系统生成
    std::string generateTemplateVariable(const std::string& varName, const std::string& value) const;
    std::string generateCustomVariable(const std::string& varName, const std::string& value) const;
    std::string generateTemplateStyleGroup(const std::string& groupName, const std::map<std::string, std::string>& properties) const;
    std::string generateCustomStyleGroup(const std::string& groupName, const std::map<std::string, std::string>& properties) const;
    std::string generateStyleGroupSpecialization(const std::string& parentGroup, const std::string& childGroup, 
                                               const std::map<std::string, std::string>& properties) const;
    
    // 响应式值生成
    std::string generateResponsiveValue(const std::string& jsVarName) const;
    std::string generateResponsiveCalculation(const std::string& expression) const;
    
    // 命名空间生成
    std::string generateNamespaceImport(const std::string& namespaceName, const std::string& itemName) const;
    std::string generateFullQualifiedName(const std::string& namespaceName, const std::string& itemName) const;
    
    // 配置生成
    std::string generateConfigurationValue(const std::string& configName, const std::string& key) const;
    
    // 原始嵌入生成
    std::string generateRawHTML(const std::string& content) const;
    std::string generateRawCSS(const std::string& content) const;
    std::string generateRawJavaScript(const std::string& content) const;
    
    // 导入生成
    std::string generateImportHTML(const std::string& path, const std::string& alias = "") const;
    std::string generateImportCSS(const std::string& path, const std::string& alias = "") const;
    std::string generateImportJavaScript(const std::string& path, const std::string& alias = "") const;
    std::string generateImportCHTL(const std::string& path, const std::string& alias = "") const;
    std::string generateImportCJMOD(const std::string& path, const std::string& alias = "") const;
    
    // 操作符生成
    std::string generateDeleteAttribute(const std::string& attributeName) const;
    std::string generateDeleteElement(const std::string& elementSelector) const;
    std::string generateDeleteStyleProperty(const std::string& propertyName) const;
    std::string generateDeleteStyleGroup(const std::string& groupName) const;
    std::string generateDeleteInheritance(const std::string& inheritedGroup) const;
    
    std::string generateInsertElement(const std::string& position, const std::string& target, 
                                    const std::string& content) const;
    std::string generateInsertAttribute(const std::string& attributeName, const std::string& value) const;
    std::string generateInsertStyleProperty(const std::string& propertyName, const std::string& value) const;
    
    // 特例化生成
    std::string generateVariableSpecialization(const std::string& parentVar, const std::string& childVar, 
                                             const std::string& value) const;
    std::string generateTemplateSpecialization(const std::string& parentTemplate, const std::string& childTemplate,
                                             const std::map<std::string, std::string>& properties) const;
    
    // 样式组继承生成
    std::string generateStyleGroupInheritance(const std::string& parentGroup, const std::string& childGroup) const;
    std::string generateStyleGroupInheritanceDeletion(const std::string& parentGroup, const std::string& childGroup) const;
    
    // 上下文管理
    void setContext(std::shared_ptr<CHTLContext> context);
    std::shared_ptr<CHTLContext> getContext() const;
    
    // 代码优化
    std::string optimizeCSS(const std::string& css) const;
    std::string optimizeHTML(const std::string& html) const;
    std::string optimizeJavaScript(const std::string& js) const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    
private:
    std::shared_ptr<CHTLContext> context_;
    std::function<void(const std::string&, int, int)> error_handler_;
    
    // 辅助方法
    std::string processTemplateVariables(const std::string& content) const;
    std::string processCustomVariables(const std::string& content) const;
    std::string processResponsiveValues(const std::string& content) const;
    std::string processPropertyReferences(const std::string& content) const;
    
    // 样式处理
    std::string processStyleProperties(const std::map<std::string, std::string>& properties) const;
    std::string processStyleInheritance(const std::vector<std::string>& inheritedGroups) const;
    std::string processStyleDeletion(const std::vector<std::string>& deletedProperties) const;
    
    // 表达式处理
    std::string processArithmeticExpression(const std::string& expression) const;
    std::string processConditionalExpression(const std::string& condition, const std::string& trueValue, 
                                           const std::string& falseValue) const;
    
    // 选择器处理
    std::string processSelector(const std::string& selector) const;
    std::string processContextualSelector(const std::string& context, const std::string& selector) const;
    
    // 单位处理
    std::string processUnits(const std::string& value) const;
    std::string convertUnits(const std::string& value, const std::string& fromUnit, const std::string& toUnit) const;
    
    // 验证方法
    bool validatePropertyValue(const std::string& property, const std::string& value) const;
    bool validateSelector(const std::string& selector) const;
    bool validateExpression(const std::string& expression) const;
    
    // 错误处理
    void logError(const std::string& message, int line = 0, int column = 0);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_COMPLETE_H