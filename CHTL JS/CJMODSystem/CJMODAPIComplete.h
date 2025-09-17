#ifndef CJMOD_API_COMPLETE_H
#define CJMOD_API_COMPLETE_H

#include "CJMODAPI.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <regex>

namespace CHTLJS {

/**
 * 完整的CJMOD API
 * 强大的C++ API，用于创建CHTL JS语法扩展
 * 严格遵循CHTL.md中定义的CJMOD规范
 */

/**
 * 增强的原子参数类
 * 支持更复杂的占位符和数据绑定
 */
class AtomArgComplete : public AtomArg {
public:
    enum class ExtendedArgType {
        // 基础类型（继承自AtomArg）
        PLACEHOLDER,            // $
        OPTIONAL_PLACEHOLDER,   // $?
        REQUIRED_PLACEHOLDER,   // $!
        UNORDERED_PLACEHOLDER,  // $_
        VARIADIC_PLACEHOLDER,   // ...
        
        // 扩展类型
        TYPED_PLACEHOLDER,      // $:type
        CONDITIONAL_PLACEHOLDER, // $?condition
        REGEX_PLACEHOLDER,      // $~/pattern/
        LITERAL_PLACEHOLDER,    // $"literal"
        EXPRESSION_PLACEHOLDER, // $expr
        FUNCTION_PLACEHOLDER,   // $func()
        OBJECT_PLACEHOLDER,     // ${}
        ARRAY_PLACEHOLDER,      // $[]
        CHTLJS_PLACEHOLDER,     // $CHTLJS
        UNKNOWN_EXT
    };
    
    // 占位符约束
    struct PlaceholderConstraint {
        std::string type_constraint;
        std::string value_constraint;
        std::function<bool(const std::string&)> validator;
        std::function<std::string(const std::string&)> transformer;
        bool is_required;
        std::string default_value;
        
        PlaceholderConstraint() : is_required(false) {}
    };
    
    AtomArgComplete(ExtendedArgType type, const std::string& value = "");
    AtomArgComplete(const AtomArg& base);
    ~AtomArgComplete();
    
    // 扩展功能
    ExtendedArgType getExtendedType() const;
    void setExtendedType(ExtendedArgType type);
    
    // 约束管理
    void setConstraint(const PlaceholderConstraint& constraint);
    PlaceholderConstraint getConstraint() const;
    bool hasConstraint() const;
    
    // 类型检查和验证
    bool validateType(const std::string& value) const;
    bool validateValue(const std::string& value) const;
    std::string transformValue(const std::string& value) const;
    
    // 特殊占位符处理
    std::string processTypedPlaceholder(const std::string& type, const std::string& value) const;
    std::string processConditionalPlaceholder(const std::string& condition, const std::string& value) const;
    std::string processRegexPlaceholder(const std::string& pattern, const std::string& value) const;
    std::string processExpressionPlaceholder(const std::string& expression) const;
    
    // 代码生成
    std::string generatePlaceholderCode() const;
    std::string generateValidationCode() const;
    std::string generateTransformationCode() const;
    
    // 调试
    std::string toDetailedString() const;

private:
    ExtendedArgType extended_type_;
    PlaceholderConstraint constraint_;
    std::map<std::string, std::string> metadata_;
};

/**
 * 增强的参数列表类
 * 支持模式匹配、模板处理和高级绑定
 */
class ArgComplete : public Arg {
public:
    // 参数模式
    struct ArgumentPattern {
        std::string name;
        std::vector<AtomArgComplete> pattern;
        std::function<bool(const ArgComplete&)> matcher;
        std::function<ArgComplete(const ArgComplete&)> transformer;
        int priority;
        
        ArgumentPattern() : priority(0) {}
    };
    
    // 模板上下文
    struct TemplateContext {
        std::map<std::string, std::string> variables;
        std::map<std::string, std::function<std::string(const std::string&)>> functions;
        std::map<std::string, ArgComplete> nested_args;
        std::string current_scope;
    };
    
    ArgComplete();
    ArgComplete(const Arg& base);
    ArgComplete(const std::vector<AtomArgComplete>& atoms);
    ~ArgComplete();
    
    // 模式匹配
    void addPattern(const ArgumentPattern& pattern);
    ArgumentPattern findMatchingPattern(const ArgComplete& input) const;
    bool matchesPattern(const ArgumentPattern& pattern) const;
    ArgComplete applyPattern(const ArgumentPattern& pattern) const;
    
    // 模板处理
    void setTemplateContext(const TemplateContext& context);
    TemplateContext getTemplateContext() const;
    std::string processTemplate(const std::string& template_str) const;
    std::string expandTemplate(const std::string& template_str, const TemplateContext& context) const;
    
    // 高级绑定
    void bindFunction(const std::string& name, std::function<std::string(const ArgComplete&)> func);
    void bindVariable(const std::string& name, const std::string& value);
    void bindNested(const std::string& name, const ArgComplete& args);
    
    // 类型转换和验证
    bool validateAllArguments() const;
    ArgComplete transformAllArguments() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 代码生成
    std::string generateArgumentCode() const;
    std::string generateValidationCode() const;
    std::string generateBindingCode() const;
    
    // 序列化和反序列化
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // 调试和分析
    void dumpStructure() const;
    std::string getStructureAnalysis() const;

private:
    std::vector<ArgumentPattern> patterns_;
    TemplateContext template_context_;
    std::map<std::string, std::function<std::string(const ArgComplete&)>> bound_functions_;
    std::vector<std::string> validation_errors_;
};

/**
 * 增强的语法分析类
 * 支持复杂的CHTL JS语法模式识别和AST构建
 */
class SyntaxComplete : public Syntax {
public:
    // 语法规则
    struct SyntaxRule {
        std::string name;
        std::string pattern;
        std::function<ArgComplete(const std::string&)> parser;
        std::function<bool(const std::string&)> validator;
        int precedence;
        
        SyntaxRule() : precedence(0) {}
    };
    
    // 语法上下文
    struct SyntaxContext {
        std::string source_language;    // "chtl", "chtljs", "css", "js"
        std::string target_language;    // "javascript", "css", "html"
        std::map<std::string, std::string> context_variables;
        std::vector<std::string> imported_modules;
        std::string current_scope;
    };
    
    // 重写基类方法
    static ArgComplete analyzeComplete(const std::string& syntax);
    static ArgComplete analyzeWithContext(const std::string& syntax, const SyntaxContext& context);
    
    // 高级语法分析
    static std::vector<SyntaxRule> extractSyntaxRules(const std::string& syntax);
    static bool validateSyntaxStructure(const std::string& syntax);
    static std::string normalizeSyntax(const std::string& syntax);
    
    // CHTL JS特有语法分析
    static bool isCHTLJSScriptLoader(const std::string& str);
    static bool isCHTLJSListen(const std::string& str);
    static bool isCHTLJSAnimate(const std::string& str);
    static bool isCHTLJSRouter(const std::string& str);
    static bool isCHTLJSVir(const std::string& str);
    static bool isCHTLJSUtilThen(const std::string& str);
    static bool isCHTLJSPrintMylove(const std::string& str);
    static bool isCHTLJSINeverAway(const std::string& str);
    static bool isCHTLJSResponsiveValue(const std::string& str);
    static bool isCHTLJSSelector(const std::string& str);
    
    // 模式匹配
    static std::vector<std::string> findPatternMatches(const std::string& source, const std::string& pattern);
    static std::string extractPatternContent(const std::string& source, const std::string& pattern);
    static bool matchesComplexPattern(const std::string& str, const std::string& pattern);
    
    // AST构建
    static std::shared_ptr<CHTLJSBaseNode> buildAST(const ArgComplete& args);
    static std::shared_ptr<CHTLJSBaseNode> buildCHTLJSNode(const std::string& syntax, const ArgComplete& args);
    
    // 语法变换
    static std::string transformSyntax(const std::string& source, const std::vector<SyntaxRule>& rules);
    static std::string applySyntaxRule(const std::string& source, const SyntaxRule& rule);

private:
    static std::vector<SyntaxRule> syntax_rules_;
    static std::map<std::string, std::regex> compiled_patterns_;
    
    // 内部辅助方法
    static std::regex compilePattern(const std::string& pattern);
    static bool isValidCHTLJSSyntax(const std::string& syntax);
    static std::vector<std::string> tokenizeAdvanced(const std::string& syntax);
    static std::string extractFunctionName(const std::string& syntax);
    static std::map<std::string, std::string> extractParameters(const std::string& syntax);
};

/**
 * 增强的CJMOD扫描器
 * 支持智能源码分析、上下文感知扫描和增量更新
 */
class CJMODScannerComplete : public CJMODScanner {
public:
    // 扫描配置
    struct ScanConfiguration {
        bool enable_context_analysis;
        bool enable_incremental_scan;
        bool preserve_whitespace;
        bool extract_comments;
        int max_scan_depth;
        std::vector<std::string> ignore_patterns;
        
        ScanConfiguration() 
            : enable_context_analysis(true), enable_incremental_scan(false),
              preserve_whitespace(true), extract_comments(false), max_scan_depth(10) {}
    };
    
    // 扫描结果
    struct ScanResult {
        ArgComplete primary_args;
        std::map<std::string, ArgComplete> context_args;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::map<std::string, std::string> metadata;
        double scan_time_ms;
        
        ScanResult() : scan_time_ms(0.0) {}
    };
    
    CJMODScannerComplete();
    ~CJMODScannerComplete();
    
    // 配置管理
    void setConfiguration(const ScanConfiguration& config);
    ScanConfiguration getConfiguration() const;
    
    // 增强的扫描方法
    static ScanResult scanAdvanced(const std::string& source, const std::string& keyword);
    static ScanResult scanWithContext(const std::string& source, const std::string& keyword, 
                                     const SyntaxComplete::SyntaxContext& context);
    static ScanResult scanIncremental(const std::string& source, const std::string& keyword, 
                                     const ScanResult& previous_result);
    
    // 多关键字扫描
    static std::map<std::string, ScanResult> scanMultipleKeywords(const std::string& source, 
                                                                 const std::vector<std::string>& keywords);
    
    // 智能分析
    static std::vector<std::string> suggestKeywords(const std::string& source);
    static std::map<std::string, int> analyzeKeywordFrequency(const std::string& source);
    static std::vector<std::string> extractCHTLJSPatterns(const std::string& source);
    
    // 上下文分析
    static std::string analyzeSourceContext(const std::string& source, size_t position);
    static std::vector<std::string> findRelatedKeywords(const std::string& source, const std::string& keyword);
    static std::map<std::string, std::string> extractContextVariables(const std::string& source);
    
    // 性能优化
    static void enableCaching(bool enable);
    static void clearCache();
    static std::string getCacheStatistics();

private:
    static ScanConfiguration default_config_;
    static std::map<std::string, ScanResult> scan_cache_;
    static bool caching_enabled_;
    
    // 内部扫描方法
    static std::vector<size_t> findAllOccurrences(const std::string& source, const std::string& keyword);
    static std::string extractSurroundingContext(const std::string& source, size_t position, int radius);
    static bool isValidKeywordPosition(const std::string& source, size_t position, const std::string& keyword);
    static std::string normalizeKeyword(const std::string& keyword);
    static double measureScanTime(std::function<void()> scan_function);
};

/**
 * 增强的CJMOD生成器
 * 支持多种输出格式、代码优化和自定义模板
 */
class CJMODGeneratorComplete : public CJMODGenerator {
public:
    // 输出格式
    enum class OutputFormat {
        JAVASCRIPT,         // 标准JavaScript
        CHTLJS,            // CHTL JS语法
        TYPESCRIPT,        // TypeScript
        MINIFIED_JS,       // 压缩JavaScript
        DOCUMENTED_JS,     // 带文档的JavaScript
        MODULE_JS,         // ES6模块
        COMMONJS,          // CommonJS模块
        JSON,              // JSON格式
        YAML,              // YAML格式
        XML                // XML格式
    };
    
    // 生成配置
    struct GenerationConfig {
        OutputFormat output_format;
        bool include_comments;
        bool include_source_map;
        bool optimize_code;
        bool validate_output;
        std::string indentation;
        std::string line_ending;
        std::map<std::string, std::string> custom_templates;
        
        GenerationConfig() 
            : output_format(OutputFormat::JAVASCRIPT), include_comments(true),
              include_source_map(false), optimize_code(false), validate_output(true),
              indentation("    "), line_ending("\n") {}
    };
    
    // 代码模板
    struct CodeTemplate {
        std::string name;
        std::string template_content;
        std::map<std::string, std::string> placeholders;
        std::function<std::string(const ArgComplete&)> generator;
        OutputFormat target_format;
        
        CodeTemplate() : target_format(OutputFormat::JAVASCRIPT) {}
    };
    
    CJMODGeneratorComplete();
    ~CJMODGeneratorComplete();
    
    // 配置管理
    void setGenerationConfig(const GenerationConfig& config);
    GenerationConfig getGenerationConfig() const;
    
    // 模板管理
    void addTemplate(const CodeTemplate& template_def);
    void removeTemplate(const std::string& name);
    CodeTemplate getTemplate(const std::string& name) const;
    std::vector<std::string> getAvailableTemplates() const;
    
    // 增强的生成方法
    static std::string generateAdvanced(const ArgComplete& args, const GenerationConfig& config = GenerationConfig());
    static std::string generateWithTemplate(const ArgComplete& args, const std::string& template_name);
    static std::string generateForFormat(const ArgComplete& args, OutputFormat format);
    
    // 多目标生成
    static std::map<OutputFormat, std::string> generateMultiFormat(const ArgComplete& args, 
                                                                   const std::vector<OutputFormat>& formats);
    
    // 代码优化
    static std::string optimizeJavaScript(const std::string& code);
    static std::string minifyCode(const std::string& code);
    static std::string beautifyCode(const std::string& code);
    static std::string addDocumentation(const std::string& code, const ArgComplete& args);
    
    // 验证和分析
    static bool validateGeneratedCode(const std::string& code, OutputFormat format);
    static std::vector<std::string> analyzeCodeQuality(const std::string& code);
    static std::string generateSourceMap(const std::string& original_source, const std::string& generated_code);
    
    // 模块生成
    static std::string generateES6Module(const ArgComplete& args);
    static std::string generateCommonJSModule(const ArgComplete& args);
    static std::string generateUMDModule(const ArgComplete& args);
    
    // 特殊格式生成
    static std::string generateJSON(const ArgComplete& args);
    static std::string generateYAML(const ArgComplete& args);
    static std::string generateXML(const ArgComplete& args);

private:
    GenerationConfig config_;
    std::map<std::string, CodeTemplate> templates_;
    
    // 内部生成方法
    std::string processCodeTemplate(const CodeTemplate& template_def, const ArgComplete& args) const;
    std::string replacePlaceholders(const std::string& template_content, const ArgComplete& args) const;
    std::string formatCode(const std::string& code, OutputFormat format) const;
    std::string addIndentation(const std::string& code, const std::string& indentation) const;
    std::string normalizeLineEndings(const std::string& code, const std::string& line_ending) const;
    
    // 初始化
    void initializeDefaultTemplates();
    void loadCustomTemplates();
};

/**
 * 增强的CHTL JS函数类
 * 支持复杂函数定义、运行时绑定和虚拟对象集成
 */
class CHTLJSFunctionComplete : public CHTLJSFunction {
public:
    // 函数类型
    enum class FunctionType {
        SCRIPT_LOADER,      // ScriptLoader
        LISTEN,             // Listen
        ANIMATE,            // Animate
        ROUTER,             // Router
        VIR,                // Vir
        UTIL_THEN,          // util.then
        PRINTMYLOVE,        // printMylove
        INEVERAWAY,         // iNeverAway
        CUSTOM,             // 自定义函数
        BUILTIN             // 内置函数
    };
    
    // 函数签名
    struct FunctionSignature {
        std::string name;
        std::vector<std::string> parameter_names;
        std::map<std::string, std::string> parameter_types;
        std::map<std::string, std::string> parameter_defaults;
        std::string return_type;
        bool is_variadic;
        
        FunctionSignature() : is_variadic(false) {}
    };
    
    // 运行时绑定
    struct RuntimeBinding {
        std::string target_object;
        std::string binding_method;
        std::map<std::string, std::string> binding_parameters;
        std::function<void()> initialization_code;
        std::function<void()> cleanup_code;
    };
    
    CHTLJSFunctionComplete(const std::string& functionName, FunctionType type = FunctionType::CUSTOM);
    CHTLJSFunctionComplete(const CHTLJSFunction& base);
    ~CHTLJSFunctionComplete();
    
    // 类型管理
    FunctionType getFunctionType() const;
    void setFunctionType(FunctionType type);
    
    // 签名管理
    void setSignature(const FunctionSignature& signature);
    FunctionSignature getSignature() const;
    bool validateSignature() const;
    
    // 运行时绑定
    void addRuntimeBinding(const RuntimeBinding& binding);
    void removeRuntimeBinding(const std::string& target_object);
    std::vector<RuntimeBinding> getRuntimeBindings() const;
    void initializeRuntimeBindings();
    void cleanupRuntimeBindings();
    
    // 虚拟对象集成
    void bindToVirtualObject(const std::string& object_name);
    void unbindFromVirtualObject(const std::string& object_name);
    std::vector<std::string> getBoundVirtualObjects() const;
    
    // 增强的代码生成
    std::string generateCompleteCode() const;
    std::string generateRuntimeCode() const;
    std::string generateBindingCode() const;
    std::string generateValidationCode() const;
    
    // 参数处理
    void addTypedParameter(const std::string& key, const std::string& value, const std::string& type);
    void addOptionalParameter(const std::string& key, const std::string& default_value);
    void addVariadicParameter(const std::string& key, const std::vector<std::string>& values);
    
    // 函数调用
    std::string generateFunctionCall(const std::map<std::string, std::string>& call_params) const;
    std::string generateAsyncFunctionCall(const std::map<std::string, std::string>& call_params) const;
    
    // 调试和分析
    std::string getFunctionAnalysis() const;
    std::vector<std::string> validateParameters() const;
    void dumpFunctionStructure() const;

private:
    FunctionType function_type_;
    FunctionSignature signature_;
    std::vector<RuntimeBinding> runtime_bindings_;
    std::set<std::string> bound_virtual_objects_;
    
    // 内部方法
    std::string generateTypeValidation(const std::string& param_name, const std::string& param_type) const;
    std::string generateDefaultValueAssignment(const std::string& param_name, const std::string& default_value) const;
    std::string generateVariadicHandling(const std::string& param_name) const;
    bool isValidParameterType(const std::string& type) const;
    std::string convertTypeToJavaScript(const std::string& type) const;
};

/**
 * CJMOD模块管理器
 * 负责CJMOD模块的加载、注册和生命周期管理
 */
class CJMODModuleManager {
public:
    // 模块信息
    struct ModuleInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::vector<std::string> dependencies;
        std::map<std::string, std::string> exports;
        std::string module_path;
        bool is_loaded;
        
        ModuleInfo() : is_loaded(false) {}
    };
    
    // 模块注册表
    struct ModuleRegistry {
        std::map<std::string, ModuleInfo> modules;
        std::map<std::string, std::vector<std::string>> dependency_graph;
        std::vector<std::string> load_order;
    };
    
    static CJMODModuleManager& getInstance();
    
    // 模块管理
    bool registerModule(const ModuleInfo& module_info);
    bool unregisterModule(const std::string& module_name);
    bool loadModule(const std::string& module_name);
    bool unloadModule(const std::string& module_name);
    
    // 依赖管理
    std::vector<std::string> resolveDependencies(const std::string& module_name);
    bool checkCircularDependencies(const std::string& module_name);
    std::vector<std::string> getLoadOrder();
    
    // 查询
    ModuleInfo getModuleInfo(const std::string& module_name) const;
    std::vector<std::string> getLoadedModules() const;
    std::vector<std::string> getAvailableModules() const;
    bool isModuleLoaded(const std::string& module_name) const;
    
    // 导出功能
    void exportFunction(const std::string& module_name, const std::string& function_name, 
                       std::shared_ptr<CHTLJSFunctionComplete> function);
    std::shared_ptr<CHTLJSFunctionComplete> importFunction(const std::string& module_name, 
                                                           const std::string& function_name);
    
    // 清理
    void clearAll();

private:
    CJMODModuleManager() = default;
    ~CJMODModuleManager() = default;
    CJMODModuleManager(const CJMODModuleManager&) = delete;
    CJMODModuleManager& operator=(const CJMODModuleManager&) = delete;
    
    ModuleRegistry registry_;
    std::map<std::string, std::map<std::string, std::shared_ptr<CHTLJSFunctionComplete>>> exported_functions_;
    
    // 内部方法
    bool validateModuleInfo(const ModuleInfo& module_info) const;
    void buildDependencyGraph();
    std::vector<std::string> topologicalSort();
};

} // namespace CHTLJS

#endif // CJMOD_API_COMPLETE_H