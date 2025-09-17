#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 生成选项
struct GenerationOptions {
    bool includeDefaultStructure = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool minify = false;
    bool debugMode = false;
    std::string outputDirectory = ".";
    std::string cssFileName = "style.css";
    std::string jsFileName = "script.js";
    std::string htmlFileName = "index.html";
};

// 生成结果
struct GenerationResult {
    std::string html;
    std::string css;
    std::string js;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

class CHTLGenerator {
public:
    CHTLGenerator();
    
    // 主要接口
    GenerationResult generate(NodePtr root, const GenerationOptions& options = GenerationOptions());
    GenerationResult generateHTML(NodePtr root, const GenerationOptions& options = GenerationOptions());
    GenerationResult generateCSS(NodePtr root, const GenerationOptions& options = GenerationOptions());
    GenerationResult generateJS(NodePtr root, const GenerationOptions& options = GenerationOptions());
    
    // 配置
    void setOptions(const GenerationOptions& options) { options_ = options; }
    const GenerationOptions& getOptions() const { return options_; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    void setWarningHandler(std::function<void(const std::string&)> handler);
    
private:
    GenerationOptions options_;
    std::function<void(const std::string&)> errorHandler_;
    std::function<void(const std::string&)> warningHandler_;
    
    // 生成状态
    std::string currentCSS_;
    std::string currentJS_;
    std::string currentHTML_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // HTML生成
    std::string generateHTMLFromNode(NodePtr node, int indentLevel = 0);
    std::string generateElementHTML(ElementNode* element, int indentLevel = 0);
    std::string generateTextHTML(TextNode* textNode, int indentLevel = 0);
    std::string generateTemplateHTML(NodePtr node, int indentLevel = 0);
    std::string generateCustomHTML(NodePtr node, int indentLevel = 0);
    std::string generateStyleHTML(NodePtr node, int indentLevel = 0);
    std::string generateScriptHTML(NodePtr node, int indentLevel = 0);
    std::string generateOriginHTML(NodePtr node, int indentLevel = 0);
    std::string generateImportHTML(NodePtr node, int indentLevel = 0);
    std::string generateConfigurationHTML(NodePtr node, int indentLevel = 0);
    std::string generateNamespaceHTML(NodePtr node, int indentLevel = 0);
    std::string generateOperatorHTML(NodePtr node, int indentLevel = 0);
    
    // CSS生成
    std::string generateCSSFromNode(NodePtr node);
    std::string generateElementCSS(ElementNode* element);
    std::string generateStyleCSS(NodePtr node);
    std::string generateTemplateCSS(NodePtr node);
    std::string generateCustomCSS(NodePtr node);
    
    // JS生成
    std::string generateJSFromNode(NodePtr node);
    std::string generateElementJS(ElementNode* element);
    std::string generateScriptJS(NodePtr node);
    std::string generateTemplateJS(NodePtr node);
    std::string generateCustomJS(NodePtr node);
    
    // 辅助方法
    std::string getIndent(int level) const;
    std::string formatHTML(const std::string& html) const;
    std::string formatCSS(const std::string& css) const;
    std::string formatJS(const std::string& js) const;
    std::string minifyHTML(const std::string& html) const;
    std::string minifyCSS(const std::string& css) const;
    std::string minifyJS(const std::string& js) const;
    
    // 默认结构生成
    std::string generateDefaultHTMLStructure() const;
    std::string generateDefaultCSSStructure() const;
    std::string generateDefaultJSStructure() const;
    
    // 模板处理
    void processTemplates(NodePtr root);
    void processTemplate(NodePtr templateNode);
    void processCustom(NodePtr customNode);
    
    // 导入处理
    void processImports(NodePtr root);
    void processImport(NodePtr importNode);
    
    // 配置处理
    void processConfiguration(NodePtr root);
    void processConfigurationNode(NodePtr configNode);
    
    // 命名空间处理
    void processNamespaces(NodePtr root);
    void processNamespace(NodePtr namespaceNode);
    
    // 错误和警告
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void reportError(const std::string& message);
    void reportWarning(const std::string& message);
    
    // 验证
    bool validateNode(NodePtr node);
    bool validateElement(ElementNode* element);
    bool validateTemplate(NodePtr templateNode);
    bool validateCustom(NodePtr customNode);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H