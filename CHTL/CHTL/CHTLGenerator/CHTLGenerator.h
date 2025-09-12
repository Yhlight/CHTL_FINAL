#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTL/CHTLParser/CHTLParser.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// CHTL生成器
class CHTLGenerator {
private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 样式和脚本收集
    std::string globalStyles;
    std::string globalScripts;
    std::map<std::string, std::string> localStyles;
    std::map<std::string, std::string> localScripts;
    
    // 模板和自定义存储
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
    std::map<std::string, std::shared_ptr<CustomNode>> customs;
    std::map<std::string, std::shared_ptr<ConfigurationNode>> configurations;
    
    // 错误处理
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 生成方法
    std::string generateHTML(std::shared_ptr<CHTLNode> node);
    std::string generateElement(std::shared_ptr<ElementNode> element);
    std::string generateText(std::shared_ptr<TextNode> text);
    std::string generateComment(std::shared_ptr<CommentNode> comment);
    std::string generateStyle(std::shared_ptr<StyleNode> style);
    std::string generateScript(std::shared_ptr<ScriptNode> script);
    std::string generateTemplate(std::shared_ptr<TemplateNode> templateNode);
    std::string generateCustom(std::shared_ptr<CustomNode> customNode);
    std::string generateOrigin(std::shared_ptr<OriginNode> originNode);
    std::string generateImport(std::shared_ptr<ImportNode> importNode);
    std::string generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    std::string generateConstraint(std::shared_ptr<ConstraintNode> constraintNode);
    std::string generateConfiguration(std::shared_ptr<ConfigurationNode> configNode);
    std::string generateUse(std::shared_ptr<UseNode> useNode);
    
    // 样式处理
    std::string processInlineStyles(const std::map<std::string, std::string>& styles);
    std::string processClassSelectors(const std::vector<std::string>& classes);
    std::string processIdSelectors(const std::vector<std::string>& ids);
    std::string processStyleProperties(const std::map<std::string, std::string>& properties);
    
    // 属性处理
    std::string processAttributes(const std::map<std::string, std::string>& attributes);
    std::string processAttributeValue(const std::string& value);
    
    // 表达式处理
    std::string processExpression(const std::string& expression);
    std::string processConditionalExpression(const std::string& expression);
    std::string processArithmeticExpression(const std::string& expression);
    std::string processLogicalExpression(const std::string& expression);
    
    // 模板处理
    std::string processTemplateReference(const std::string& templateName, 
                                       const std::map<std::string, std::string>& parameters);
    std::string processCustomReference(const std::string& customName, 
                                     const std::map<std::string, std::string>& parameters);
    
    // 原始嵌入处理
    std::string processOriginContent(const std::string& content, OriginNode::OriginType type);
    
    // 导入处理
    std::string processImportContent(const std::string& path, ImportNode::ImportType type);
    
    // 约束处理
    bool processConstraint(const std::string& target, const std::string& condition);
    
    // 配置处理
    std::string processConfiguration(const std::string& configName);
    
    // Use处理
    std::string processUse(const std::string& target, const std::vector<std::string>& parameters);
    
    // 工具方法
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJavaScript(const std::string& text);
    std::string generateClassName(const std::string& base);
    std::string generateId(const std::string& base);
    
public:
    CHTLGenerator();
    ~CHTLGenerator();
    
    // 主要方法
    std::string generate(std::shared_ptr<CHTLNode> ast);
    std::string generateCSS();
    std::string generateJavaScript();
    std::string generateComplete();
    
    // 模板和自定义管理
    void addTemplate(std::shared_ptr<TemplateNode> templateNode);
    void addCustom(std::shared_ptr<CustomNode> customNode);
    void addConfiguration(std::shared_ptr<ConfigurationNode> configNode);
    
    std::shared_ptr<TemplateNode> getTemplate(const std::string& name) const;
    std::shared_ptr<CustomNode> getCustom(const std::string& name) const;
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name) const;
    
    // 状态查询
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // 调试
    void printErrors() const;
    void printWarnings() const;
    
    // 重置
    void reset();
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H