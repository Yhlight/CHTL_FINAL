#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLParser/CHTLASTNode.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

enum class OutputFormat {
    HTML,           // 标准HTML输出
    XHTML,          // XHTML输出
    MINIFIED,       // 压缩输出
    PRETTY,         // 美化输出
    DEBUG           // 调试输出
};

struct GeneratorOptions {
    OutputFormat format;
    bool includeComments;
    bool minifyOutput;
    bool preserveWhitespace;
    int indentSize;
    std::string indentChar;
    bool useTabs;
    bool addDoctype;
    std::string charset;
    std::string lang;
    
    GeneratorOptions() : format(OutputFormat::HTML), includeComments(true), 
                        minifyOutput(false), preserveWhitespace(true),
                        indentSize(2), indentChar(" "), useTabs(false),
                        addDoctype(true), charset("UTF-8"), lang("en") {}
};

class CHTLGenerator {
private:
    GeneratorOptions options;
    std::unordered_map<std::string, std::shared_ptr<CHTLASTNode>> templates;
    std::unordered_map<std::string, std::shared_ptr<CHTLASTNode>> customElements;
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> namespaces;
    
    int currentIndent;
    std::string currentIndentStr;
    std::vector<std::string> outputLines;
    
    // 内部方法
    void updateIndent();
    void addIndent();
    void removeIndent();
    void addLine(const std::string& line);
    void addContent(const std::string& content);
    
    std::string generateHTML(std::shared_ptr<CHTLASTNode> node);
    std::string generateHTMLElement(std::shared_ptr<HTMLElementNode> element);
    std::string generateTextNode(std::shared_ptr<TextNode> textNode);
    std::string generateAttribute(std::shared_ptr<AttributeNode> attribute);
    std::string generateStyleBlock(std::shared_ptr<StyleBlockNode> styleBlock);
    std::string generateStyleRule(std::shared_ptr<StyleRuleNode> styleRule);
    std::string generateStyleProperty(std::shared_ptr<StylePropertyNode> property);
    std::string generateScriptBlock(std::shared_ptr<ScriptBlockNode> scriptBlock);
    
    // 模板生成
    std::string generateTemplateDeclaration(std::shared_ptr<TemplateDeclarationNode> templateNode);
    std::string generateCustomDeclaration(std::shared_ptr<CustomDeclarationNode> customNode);
    std::string generateOriginNode(std::shared_ptr<OriginNode> originNode);
    std::string generateImportNode(std::shared_ptr<ImportNode> importNode);
    std::string generateNamespaceNode(std::shared_ptr<NamespaceNode> namespaceNode);
    std::string generateConfigurationNode(std::shared_ptr<ConfigurationNode> configNode);
    std::string generateInfoNode(std::shared_ptr<InfoNode> infoNode);
    std::string generateExportNode(std::shared_ptr<ExportNode> exportNode);
    
    // 操作生成
    std::string generateDeleteNode(std::shared_ptr<CHTLASTNode> deleteNode);
    std::string generateInsertNode(std::shared_ptr<CHTLASTNode> insertNode);
    std::string generateInheritNode(std::shared_ptr<CHTLASTNode> inheritNode);
    std::string generateExceptNode(std::shared_ptr<CHTLASTNode> exceptNode);
    
    // 辅助方法
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSS(const std::string& text) const;
    std::string escapeJS(const std::string& text) const;
    std::string formatOutput(const std::string& content) const;
    std::string minifyOutput(const std::string& content) const;
    std::string addDoctype() const;
    std::string addMetaTags() const;
    
    // 模板处理
    std::string processTemplate(const std::string& templateName, 
                               const std::unordered_map<std::string, std::string>& params) const;
    std::string processCustomElement(const std::string& elementName,
                                   const std::unordered_map<std::string, std::string>& params) const;
    std::string processVariable(const std::string& varName) const;
    
    // 工具方法
    bool isSelfClosingElement(const std::string& tagName) const;
    bool isInlineElement(const std::string& tagName) const;
    std::string getIndentString() const;
    void resetOutput();
    
public:
    CHTLGenerator();
    CHTLGenerator(const GeneratorOptions& opts);
    ~CHTLGenerator() = default;
    
    // 主要生成方法
    std::string generate(std::shared_ptr<CHTLASTNode> ast);
    std::string generate(std::shared_ptr<CHTLASTNode> ast, const GeneratorOptions& opts);
    
    // 配置方法
    void setOptions(const GeneratorOptions& opts);
    GeneratorOptions getOptions() const;
    void setOutputFormat(OutputFormat format);
    void setMinifyOutput(bool minify);
    void setIncludeComments(bool include);
    void setPreserveWhitespace(bool preserve);
    void setIndentSize(int size);
    void setUseTabs(bool useTabs);
    void setAddDoctype(bool add);
    void setCharset(const std::string& charset);
    void setLang(const std::string& lang);
    
    // 模板管理
    void addTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> templateNode);
    void removeTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;
    std::shared_ptr<CHTLASTNode> getTemplate(const std::string& name) const;
    
    // 自定义元素管理
    void addCustomElement(const std::string& name, std::shared_ptr<CHTLASTNode> element);
    void removeCustomElement(const std::string& name);
    bool hasCustomElement(const std::string& name) const;
    std::shared_ptr<CHTLASTNode> getCustomElement(const std::string& name) const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    void removeVariable(const std::string& name);
    bool hasVariable(const std::string& name) const;
    std::string getVariable(const std::string& name) const;
    
    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& prefix);
    void removeNamespace(const std::string& name);
    bool hasNamespace(const std::string& name) const;
    std::string getNamespacePrefix(const std::string& name) const;
    
    // 输出管理
    std::vector<std::string> getOutputLines() const;
    std::string getOutput() const;
    void clearOutput();
    
    // 调试方法
    void printTemplates() const;
    void printCustomElements() const;
    void printVariables() const;
    void printNamespaces() const;
    void printOptions() const;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H