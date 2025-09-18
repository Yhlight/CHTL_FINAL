#pragma once

#include "CHTL/Common.h"
#include "CHTL/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * CHTL生成器 - 将AST转换为HTML、CSS、JS代码
 */
class CHTLGenerator {
public:
    // 生成选项
    struct GenerateOptions {
        bool enableInlineCSS = false;
        bool enableInlineJS = false;
        bool enableDefaultStruct = false;
        bool enableMinification = false;
        bool enableSourceMaps = false;
        std::string outputFormat = "html5"; // html5, xhtml, html4
        std::string cssOutputPath = "styles.css";
        std::string jsOutputPath = "scripts.js";
        std::string sourceMapPath = "source.map";
        bool debugMode = false;
    };

    // 生成结果
    struct GenerateResult {
        bool success;
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        std::string sourceMapOutput;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        GenerateResult() : success(false) {}
    };

    // 样式信息
    struct StyleInfo {
        std::map<std::string, std::string> inlineStyles;
        std::map<std::string, std::string> classStyles;
        std::map<std::string, std::string> idStyles;
        std::vector<std::string> globalStyles;
        std::map<std::string, std::string> pseudoStyles;
        std::map<std::string, std::string> mediaQueries;
    };

    // 脚本信息
    struct ScriptInfo {
        std::vector<std::string> inlineScripts;
        std::vector<std::string> externalScripts;
        std::vector<std::string> moduleScripts;
        std::map<std::string, std::string> eventHandlers;
        std::map<std::string, std::string> virtualObjects;
    };

public:
    CHTLGenerator();
    ~CHTLGenerator();

    /**
     * 从AST生成代码
     * @param ast AST根节点
     * @param options 生成选项
     * @return 生成结果
     */
    GenerateResult generate(std::shared_ptr<BaseNode> ast, const GenerateOptions& options = {});

    /**
     * 设置上下文
     * @param context 上下文
     */
    void setContext(std::shared_ptr<void> context);

    /**
     * 获取上下文
     * @return 上下文
     */
    std::shared_ptr<void> getContext() const;

    /**
     * 重置生成器状态
     */
    void reset();

private:
    // 内部生成方法
    std::string generateHTML(std::shared_ptr<BaseNode> ast, const GenerateOptions& options);
    std::string generateCSS(std::shared_ptr<BaseNode> ast, const GenerateOptions& options);
    std::string generateJS(std::shared_ptr<BaseNode> ast, const GenerateOptions& options);
    std::string generateSourceMap(const std::string& originalCode, const std::string& generatedCode);
    
    // HTML生成
    std::string generateElement(std::shared_ptr<ElementNode> element, int indent = 0);
    std::string generateText(std::shared_ptr<TextNode> text, int indent = 0);
    std::string generateAttributes(const std::map<std::string, std::string>& attributes);
    std::string generateAttribute(const std::string& name, const std::string& value);
    
    // CSS生成
    std::string generateStyleBlock(std::shared_ptr<StyleNode> style, int indent = 0);
    std::string generateStyleProperties(const std::map<std::string, std::string>& properties, int indent = 0);
    std::string generateStyleProperty(const std::string& name, const std::string& value);
    std::string generateClassSelector(const std::string& className, const std::map<std::string, std::string>& properties, int indent = 0);
    std::string generateIdSelector(const std::string& id, const std::map<std::string, std::string>& properties, int indent = 0);
    std::string generatePseudoSelector(const std::string& pseudo, const std::map<std::string, std::string>& properties, int indent = 0);
    std::string generateMediaQuery(const std::string& media, const std::map<std::string, std::string>& properties, int indent = 0);
    
    // JS生成
    std::string generateScriptBlock(std::shared_ptr<ScriptNode> script, int indent = 0);
    std::string generateScriptContent(const std::string& content, int indent = 0);
    std::string generateEventHandlers(const std::map<std::string, std::string>& handlers, int indent = 0);
    std::string generateVirtualObjects(const std::map<std::string, std::string>& objects, int indent = 0);
    
    // 模板生成
    std::string generateTemplate(std::shared_ptr<TemplateNode> template, int indent = 0);
    std::string generateCustom(std::shared_ptr<CustomNode> custom, int indent = 0);
    std::string generateImport(std::shared_ptr<ImportNode> import, int indent = 0);
    std::string generateNamespace(std::shared_ptr<NamespaceNode> namespace, int indent = 0);
    std::string generateConfiguration(std::shared_ptr<ConfigurationNode> config, int indent = 0);
    std::string generateOrigin(std::shared_ptr<OriginNode> origin, int indent = 0);
    std::string generateOperator(std::shared_ptr<OperatorNode> op, int indent = 0);
    
    // 特殊语法处理
    std::string processAttributeOperations(const std::string& expression);
    std::string processPropertyReferences(const std::string& expression);
    std::string processConditionalExpressions(const std::string& expression);
    std::string processResponsiveValues(const std::string& expression);
    std::string processTemplateVariables(const std::string& expression);
    std::string processCustomVariables(const std::string& expression);
    
    // 样式处理
    std::string processLocalStyleBlock(std::shared_ptr<StyleNode> style, std::shared_ptr<ElementNode> parent);
    std::string processGlobalStyleBlock(std::shared_ptr<StyleNode> style);
    std::string processStyleInheritance(const std::string& style, const std::string& parentStyle);
    std::string processStyleSpecialization(const std::string& style, const std::string& specialization);
    
    // 脚本处理
    std::string processLocalScriptBlock(std::shared_ptr<ScriptNode> script, std::shared_ptr<ElementNode> parent);
    std::string processGlobalScriptBlock(std::shared_ptr<ScriptNode> script);
    std::string processScriptLoader(const std::string& content);
    std::string processEventDelegation(const std::string& content);
    
    // 模板处理
    std::string processTemplateInheritance(const std::string& template, const std::string& parent);
    std::string processTemplateSpecialization(const std::string& template, const std::string& specialization);
    std::string processTemplateParameters(const std::string& template, const std::map<std::string, std::string>& parameters);
    
    // 自定义处理
    std::string processCustomSpecialization(const std::string& custom, const std::string& specialization);
    std::string processCustomInheritance(const std::string& custom, const std::string& parent);
    std::string processCustomParameters(const std::string& custom, const std::map<std::string, std::string>& parameters);
    
    // 导入处理
    std::string processImportResolution(const std::string& import, const std::string& path);
    std::string processImportAlias(const std::string& import, const std::string& alias);
    std::string processImportNamespace(const std::string& import, const std::string& namespace);
    
    // 命名空间处理
    std::string processNamespaceResolution(const std::string& namespace, const std::string& name);
    std::string processNamespaceNesting(const std::string& namespace, const std::string& parent);
    std::string processNamespaceConflict(const std::string& namespace, const std::string& conflicting);
    
    // 配置处理
    std::string processConfigurationProperties(const std::map<std::string, std::string>& properties);
    std::string processConfigurationInheritance(const std::string& config, const std::string& parent);
    std::string processConfigurationSpecialization(const std::string& config, const std::string& specialization);
    
    // 原始嵌入处理
    std::string processOriginEmbedding(const std::string& origin, const std::string& type);
    std::string processOriginType(const std::string& origin, const std::string& type);
    std::string processOriginContent(const std::string& origin, const std::string& content);
    
    // 操作符处理
    std::string processDeleteOperator(const std::string& target, const std::string& element);
    std::string processInsertOperator(const std::string& target, const std::string& element, const std::string& position);
    std::string processReplaceOperator(const std::string& target, const std::string& element);
    std::string processInheritOperator(const std::string& target, const std::string& parent);
    
    // 工具方法
    std::string indentCode(const std::string& code, int indent);
    std::string escapeHTML(const std::string& html);
    std::string escapeCSS(const std::string& css);
    std::string escapeJS(const std::string& js);
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    std::string generateDefaultHTMLStructure(const std::string& content, const GenerateOptions& options);
    std::string generateDefaultCSSStructure(const std::string& content, const GenerateOptions& options);
    std::string generateDefaultJSStructure(const std::string& content, const GenerateOptions& options);
    
    // 错误处理
    void handleError(const std::string& error, GenerateResult& result);
    void handleWarning(const std::string& warning, GenerateResult& result);
    
    // 验证
    bool validateHTML(const std::string& html) const;
    bool validateCSS(const std::string& css) const;
    bool validateJS(const std::string& js) const;

private:
    std::shared_ptr<void> context_;
    GenerateOptions currentOptions_;
    
    // 生成状态
    StyleInfo currentStyleInfo_;
    ScriptInfo currentScriptInfo_;
    std::map<std::string, std::string> templateRegistry_;
    std::map<std::string, std::string> customRegistry_;
    std::map<std::string, std::string> variableRegistry_;
    std::map<std::string, std::string> namespaceRegistry_;
    std::map<std::string, std::string> configurationRegistry_;
    
    // 计数器
    size_t elementCounter_;
    size_t classCounter_;
    size_t idCounter_;
    size_t scriptCounter_;
    size_t styleCounter_;
    
    // 初始化
    void initializeCounters();
    void resetCounters();
};

} // namespace CHTL