#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTLNode/BaseNode.h"
#include "TemplateSystem.h"
#include "ModuleSystem.h"
#include "CHTL_JS/CHTLJSCompiler.h"
#include "Scanner/UnifiedScanner.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {

// 生成选项
struct GenerationOptions {
    bool enableMinification = false;
    bool enableSourceMap = false;
    bool enableComments = true;
    bool enablePrettyPrint = true;
    bool enableTemplateProcessing = true;
    bool enableModuleProcessing = true;
    bool enableCHTLJSProcessing = true;
    bool enableCSSProcessing = true;
    bool enableJSProcessing = true;
    
    // CSS选项
    bool enableAutoClassId = true;
    bool enableContextInference = true;
    bool enableAttributeArithmetic = true;
    bool enableReferenceAttributes = true;
    bool enableConditionalExpressions = true;
    
    // JS选项
    bool enableCHTLJSExtensions = true;
    bool enableEventDelegation = true;
    bool enableAnimationSupport = true;
    bool enableRouterSupport = true;
    bool enableVirtualObjects = true;
    
    // 输出选项
    std::string outputFormat = "html"; // html, xhtml, xml
    std::string indentStyle = "  ";    // 缩进样式
    std::string lineEnding = "\n";     // 行结束符
};

// 生成结果
struct GenerationResult {
    std::string html;
    std::string css;
    std::string js;
    std::string sourceMap;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clear() {
        html.clear();
        css.clear();
        js.clear();
        sourceMap.clear();
        errors.clear();
        warnings.clear();
    }
};

// 代码生成器
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;
    
    // 主要接口
    GenerationResult generate(NodePtr root, const GenerationOptions& options = GenerationOptions{});
    GenerationResult generateFromSource(const std::string& source, const GenerationOptions& options = GenerationOptions{});
    
    // 组件生成
    std::string generateHTML(NodePtr root, const GenerationOptions& options = GenerationOptions{});
    std::string generateCSS(NodePtr root, const GenerationOptions& options = GenerationOptions{});
    std::string generateJS(NodePtr root, const GenerationOptions& options = GenerationOptions{});
    
    // 系统集成
    void setTemplateSystem(std::shared_ptr<TemplateSystem> templateSystem);
    void setModuleSystem(std::shared_ptr<ModuleSystem> moduleSystem);
    void setCHTLJSCompiler(std::shared_ptr<CHTLJSCompiler> chtljsCompiler);
    void setUnifiedScanner(std::shared_ptr<UnifiedScanner> scanner);
    
    // 配置
    void setOptions(const GenerationOptions& options) { options_ = options; }
    const GenerationOptions& getOptions() const { return options_; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    void setWarningHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 清理
    void clear();
    
private:
    GenerationOptions options_;
    std::shared_ptr<TemplateSystem> templateSystem_;
    std::shared_ptr<ModuleSystem> moduleSystem_;
    std::shared_ptr<CHTLJSCompiler> chtljsCompiler_;
    std::shared_ptr<UnifiedScanner> scanner_;
    
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    std::function<void(const std::string&, size_t, size_t)> warningHandler_;
    
    // 生成状态
    std::string currentIndent_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // HTML生成
    std::string generateElementHTML(ElementNode* element, const GenerationOptions& options);
    std::string generateTextHTML(TextNode* text, const GenerationOptions& options);
    std::string generateAttributesHTML(ElementNode* element, const GenerationOptions& options);
    std::string generateInlineStylesHTML(ElementNode* element, const GenerationOptions& options);
    std::string generateClassesHTML(ElementNode* element, const GenerationOptions& options);
    
    // CSS生成
    std::string generateElementCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateStyleBlockCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateInlineStylesCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateContextInferenceCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateAttributeArithmeticCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateReferenceAttributesCSS(ElementNode* element, const GenerationOptions& options);
    std::string generateConditionalExpressionsCSS(ElementNode* element, const GenerationOptions& options);
    
    // JS生成
    std::string generateElementJS(ElementNode* element, const GenerationOptions& options);
    std::string generateScriptBlockJS(ElementNode* element, const GenerationOptions& options);
    std::string generateCHTLJSJS(ElementNode* element, const GenerationOptions& options);
    std::string generateEventHandlersJS(ElementNode* element, const GenerationOptions& options);
    std::string generateAnimationsJS(ElementNode* element, const GenerationOptions& options);
    std::string generateRouterJS(ElementNode* element, const GenerationOptions& options);
    std::string generateVirtualObjectsJS(ElementNode* element, const GenerationOptions& options);
    
    // 模板处理
    std::string processTemplates(const std::string& content, const GenerationOptions& options);
    std::string processTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& variables);
    std::string processCustom(const std::string& name, const std::string& content);
    
    // 模块处理
    std::string processModules(const std::string& content, const GenerationOptions& options);
    std::string processImport(const std::string& importPath, const std::string& importType);
    std::string processModule(const std::string& moduleName);
    
    // 扫描处理
    ScanResult processScanning(const std::string& content, const GenerationOptions& options);
    std::string processCHTLFragments(const std::vector<CodeFragment>& fragments, const GenerationOptions& options);
    std::string processCHTLJSFragments(const std::vector<CodeFragment>& fragments, const GenerationOptions& options);
    std::string processCSSFragments(const std::vector<CodeFragment>& fragments, const GenerationOptions& options);
    std::string processJSFragments(const std::vector<CodeFragment>& fragments, const GenerationOptions& options);
    
    // 代码优化
    std::string optimizeHTML(const std::string& html, const GenerationOptions& options);
    std::string optimizeCSS(const std::string& css, const GenerationOptions& options);
    std::string optimizeJS(const std::string& js, const GenerationOptions& options);
    
    // 代码压缩
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    // 源映射生成
    std::string generateSourceMap(const std::string& generated, const std::string& original);
    
    // 辅助方法
    std::string indent(const std::string& content, const std::string& indent = "");
    std::string escapeHTML(const std::string& content);
    std::string escapeCSS(const std::string& content);
    std::string escapeJS(const std::string& content);
    
    // 错误报告
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    void reportWarning(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 节点遍历
    void traverseNode(NodePtr node, std::function<void(NodePtr)> callback);
    void traverseElement(ElementNode* element, std::function<void(ElementNode*)> callback);
    
    // 上下文管理
    void pushContext(const std::string& context);
    void popContext();
    std::string getCurrentContext() const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // 类名和ID管理
    std::string generateClassName(ElementNode* element);
    std::string generateId(ElementNode* element);
    std::string generateUniqueClassName(const std::string& base);
    std::string generateUniqueId(const std::string& base);
    
private:
    std::vector<std::string> contextStack_;
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_set<std::string> usedClassNames_;
    std::unordered_set<std::string> usedIds_;
    size_t classNameCounter_;
    size_t idCounter_;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H