#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>

namespace CHTL {

/**
 * @brief 语法分析结果
 */
class SyntaxAnalysisResult {
public:
    SyntaxAnalysisResult(bool isValid, const std::string& errorMessage = "");
    ~SyntaxAnalysisResult() = default;

    bool isValid() const { return m_isValid; }
    std::string getErrorMessage() const { return m_errorMessage; }
    std::vector<std::string> getTokens() const { return m_tokens; }
    std::unordered_map<std::string, std::any> getMetadata() const { return m_metadata; }
    
    void setTokens(const std::vector<std::string>& tokens) { m_tokens = tokens; }
    void setMetadata(const std::string& key, const std::any& value) { m_metadata[key] = value; }
    void addToken(const std::string& token) { m_tokens.push_back(token); }

private:
    bool m_isValid;
    std::string m_errorMessage;
    std::vector<std::string> m_tokens;
    std::unordered_map<std::string, std::any> m_metadata;
};

/**
 * @brief 参数绑定结果
 */
class ParameterBindingResult {
public:
    ParameterBindingResult(bool isSuccess, const std::string& errorMessage = "");
    ~ParameterBindingResult() = default;

    bool isSuccess() const { return m_isSuccess; }
    std::string getErrorMessage() const { return m_errorMessage; }
    std::unordered_map<std::string, std::any> getBoundParameters() const { return m_boundParameters; }
    std::vector<std::string> getUnboundParameters() const { return m_unboundParameters; }
    
    void setBoundParameter(const std::string& name, const std::any& value) { m_boundParameters[name] = value; }
    void addUnboundParameter(const std::string& name) { m_unboundParameters.push_back(name); }
    bool hasParameter(const std::string& name) const { return m_boundParameters.find(name) != m_boundParameters.end(); }
    std::any getParameter(const std::string& name) const;

private:
    bool m_isSuccess;
    std::string m_errorMessage;
    std::unordered_map<std::string, std::any> m_boundParameters;
    std::vector<std::string> m_unboundParameters;
};

/**
 * @brief 代码生成结果
 */
class CodeGenerationResult {
public:
    CodeGenerationResult(bool isSuccess, const std::string& errorMessage = "");
    ~CodeGenerationResult() = default;

    bool isSuccess() const { return m_isSuccess; }
    std::string getErrorMessage() const { return m_errorMessage; }
    std::string getGeneratedCode() const { return m_generatedCode; }
    std::unordered_map<std::string, std::string> getGeneratedFiles() const { return m_generatedFiles; }
    
    void setGeneratedCode(const std::string& code) { m_generatedCode = code; }
    void addGeneratedFile(const std::string& filename, const std::string& content) { m_generatedFiles[filename] = content; }

private:
    bool m_isSuccess;
    std::string m_errorMessage;
    std::string m_generatedCode;
    std::unordered_map<std::string, std::string> m_generatedFiles;
};

/**
 * @brief 语法分析器
 */
class SyntaxAnalyzer {
public:
    explicit SyntaxAnalyzer(std::shared_ptr<CHTLContext> context);
    ~SyntaxAnalyzer() = default;

    // 语法分析
    std::shared_ptr<SyntaxAnalysisResult> analyze(const std::string& code);
    std::shared_ptr<SyntaxAnalysisResult> analyzeFile(const std::string& filePath);
    
    // 对象类型检测
    bool isObject(const std::string& code) const;
    bool isFunction(const std::string& code) const;
    bool isArray(const std::string& code) const;
    bool isCHTLJSFunction(const std::string& code) const;
    
    // 语法验证
    bool validateSyntax(const std::string& code) const;
    bool validateObjectSyntax(const std::string& code) const;
    bool validateFunctionSyntax(const std::string& code) const;
    bool validateArraySyntax(const std::string& code) const;
    
    // 语法解析
    std::vector<std::string> parseTokens(const std::string& code) const;
    std::unordered_map<std::string, std::string> parseObjectProperties(const std::string& code) const;
    std::vector<std::string> parseFunctionParameters(const std::string& code) const;
    std::vector<std::string> parseArrayElements(const std::string& code) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::vector<std::string> tokenize(const std::string& code) const;
    bool isBalanced(const std::string& code, char open, char close) const;
    std::string extractContent(const std::string& code, char open, char close) const;
    std::vector<std::string> splitByDelimiter(const std::string& code, char delimiter) const;
    std::string trim(const std::string& str) const;
};

/**
 * @brief 参数绑定器
 */
class ParameterBinder {
public:
    explicit ParameterBinder(std::shared_ptr<CHTLContext> context);
    ~ParameterBinder() = default;

    // 参数绑定
    std::shared_ptr<ParameterBindingResult> bind(const std::string& functionSignature, 
                                                 const std::vector<std::string>& arguments);
    std::shared_ptr<ParameterBindingResult> bindWithPlaceholders(const std::string& functionSignature, 
                                                                const std::unordered_map<std::string, std::string>& placeholders);
    
    // 参数填充
    std::string fillValue(const std::string& template_, const std::unordered_map<std::string, std::string>& values) const;
    std::string transform(const std::string& template_, const std::unordered_map<std::string, std::string>& transformations) const;
    
    // 占位符处理
    std::vector<std::string> extractPlaceholders(const std::string& template_) const;
    std::string replacePlaceholder(const std::string& template_, const std::string& placeholder, const std::string& value) const;
    std::string replaceAllPlaceholders(const std::string& template_, const std::unordered_map<std::string, std::string>& replacements) const;
    
    // 参数验证
    bool validateParameters(const std::string& functionSignature, const std::vector<std::string>& arguments) const;
    bool validatePlaceholders(const std::string& template_, const std::vector<std::string>& placeholders) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::vector<std::string> parseFunctionSignature(const std::string& signature) const;
    std::string extractParameterName(const std::string& parameter) const;
    std::string extractParameterType(const std::string& parameter) const;
    bool isOptionalParameter(const std::string& parameter) const;
    std::string generatePlaceholder(const std::string& name) const;
};

/**
 * @brief CJMOD 扫描器
 */
class CJMODScanner {
public:
    explicit CJMODScanner(std::shared_ptr<CHTLContext> context);
    ~CJMODScanner() = default;

    // 扫描功能
    std::vector<std::string> scan(const std::string& code);
    std::vector<std::string> scanFile(const std::string& filePath);
    std::vector<std::string> scanDirectory(const std::string& directoryPath);
    
    // 扫描模式
    void setScanMode(const std::string& mode) { m_scanMode = mode; }
    void setScanPattern(const std::string& pattern) { m_scanPattern = pattern; }
    void setScanOptions(const std::unordered_map<std::string, std::string>& options) { m_scanOptions = options; }
    
    // 扫描结果处理
    std::vector<std::string> filterResults(const std::vector<std::string>& results, const std::string& filter) const;
    std::vector<std::string> sortResults(const std::vector<std::string>& results, const std::string& sortBy) const;
    std::unordered_map<std::string, std::vector<std::string>> groupResults(const std::vector<std::string>& results) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_scanMode;
    std::string m_scanPattern;
    std::unordered_map<std::string, std::string> m_scanOptions;
    
    // 辅助方法
    std::vector<std::string> scanByPattern(const std::string& code, const std::string& pattern) const;
    std::vector<std::string> scanByMode(const std::string& code, const std::string& mode) const;
    bool matchesPattern(const std::string& text, const std::string& pattern) const;
    std::string applyOptions(const std::string& text, const std::unordered_map<std::string, std::string>& options) const;
};

/**
 * @brief CJMOD 生成器
 */
class CJMODGenerator {
public:
    explicit CJMODGenerator(std::shared_ptr<CHTLContext> context);
    ~CJMODGenerator() = default;

    // 代码生成
    std::shared_ptr<CodeGenerationResult> generate(const std::string& template_, 
                                                   const std::unordered_map<std::string, std::any>& parameters);
    std::shared_ptr<CodeGenerationResult> generateFromFile(const std::string& templateFile, 
                                                          const std::unordered_map<std::string, std::any>& parameters);
    
    // 结果导出
    std::string exportResult(const std::shared_ptr<CodeGenerationResult>& result) const;
    bool exportResultToFile(const std::shared_ptr<CodeGenerationResult>& result, const std::string& filePath) const;
    bool exportResultToFiles(const std::shared_ptr<CodeGenerationResult>& result, const std::string& directoryPath) const;
    
    // 模板处理
    std::string processTemplate(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const;
    std::string processTemplateFile(const std::string& templateFile, const std::unordered_map<std::string, std::any>& parameters) const;
    
    // 代码优化
    std::string optimizeCode(const std::string& code) const;
    std::string minifyCode(const std::string& code) const;
    std::string beautifyCode(const std::string& code) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string replacePlaceholders(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const;
    std::string processConditionals(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const;
    std::string processLoops(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const;
    std::string processIncludes(const std::string& template_, const std::unordered_map<std::string, std::any>& parameters) const;
    std::string formatValue(const std::any& value) const;
};

/**
 * @brief 原子参数占位符
 */
class AtomArg {
public:
    AtomArg(const std::string& placeholder, const std::string& type = "");
    ~AtomArg() = default;

    std::string getPlaceholder() const { return m_placeholder; }
    std::string getType() const { return m_type; }
    bool isRequired() const { return m_isRequired; }
    std::string getDefaultValue() const { return m_defaultValue; }
    
    void setType(const std::string& type) { m_type = type; }
    void setRequired(bool required) { m_isRequired = required; }
    void setDefaultValue(const std::string& value) { m_defaultValue = value; }
    
    // 占位符类型检测
    bool isVariable() const;
    bool isOptional() const;
    bool isRequired() const;
    bool isWildcard() const;
    bool isSpecial() const;

private:
    std::string m_placeholder;
    std::string m_type;
    bool m_isRequired;
    std::string m_defaultValue;
};

/**
 * @brief CHTL JS 函数
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& name, const std::string& signature, const std::string& body);
    ~CHTLJSFunction() = default;

    std::string getName() const { return m_name; }
    std::string getSignature() const { return m_signature; }
    std::string getBody() const { return m_body; }
    std::vector<std::string> getParameters() const { return m_parameters; }
    
    void setBody(const std::string& body) { m_body = body; }
    void addParameter(const std::string& parameter) { m_parameters.push_back(parameter); }
    
    // 函数操作
    std::string call(const std::vector<std::string>& arguments) const;
    std::string bind(const std::unordered_map<std::string, std::string>& bindings) const;
    std::string createVirtualObject() const;
    
    // 函数验证
    bool validate() const;
    bool validateParameters(const std::vector<std::string>& arguments) const;

private:
    std::string m_name;
    std::string m_signature;
    std::string m_body;
    std::vector<std::string> m_parameters;
    
    // 辅助方法
    std::vector<std::string> parseParameters(const std::string& signature) const;
    std::string generateFunctionCall(const std::vector<std::string>& arguments) const;
    std::string generateVirtualObjectCode() const;
};

/**
 * @brief CJMOD API 管理器
 */
class CJMODAPIManager {
public:
    explicit CJMODAPIManager(std::shared_ptr<CHTLContext> context);
    ~CJMODAPIManager() = default;

    // API 注册
    void registerSyntaxAnalyzer(std::shared_ptr<SyntaxAnalyzer> analyzer);
    void registerParameterBinder(std::shared_ptr<ParameterBinder> binder);
    void registerCJMODScanner(std::shared_ptr<CJMODScanner> scanner);
    void registerCJMODGenerator(std::shared_ptr<CJMODGenerator> generator);
    
    // API 获取
    std::shared_ptr<SyntaxAnalyzer> getSyntaxAnalyzer() const;
    std::shared_ptr<ParameterBinder> getParameterBinder() const;
    std::shared_ptr<CJMODScanner> getCJMODScanner() const;
    std::shared_ptr<CJMODGenerator> getCJMODGenerator() const;
    
    // 函数管理
    void registerCHTLJSFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> function);
    std::shared_ptr<CHTLJSFunction> getCHTLJSFunction(const std::string& name) const;
    bool hasCHTLJSFunction(const std::string& name) const;
    
    // 占位符管理
    void registerAtomArg(const std::string& placeholder, std::shared_ptr<AtomArg> atomArg);
    std::shared_ptr<AtomArg> getAtomArg(const std::string& placeholder) const;
    bool hasAtomArg(const std::string& placeholder) const;
    
    // 扫描算法
    std::vector<std::string> performDoublePointerScan(const std::string& code) const;
    std::vector<std::string> performPreTruncationScan(const std::string& code) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::shared_ptr<SyntaxAnalyzer> m_syntaxAnalyzer;
    std::shared_ptr<ParameterBinder> m_parameterBinder;
    std::shared_ptr<CJMODScanner> m_cjmodScanner;
    std::shared_ptr<CJMODGenerator> m_cjmodGenerator;
    std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> m_functions;
    std::unordered_map<std::string, std::shared_ptr<AtomArg>> m_atomArgs;
    
    // 辅助方法
    std::vector<std::string> doublePointerScan(const std::string& code) const;
    std::vector<std::string> preTruncationScan(const std::string& code) const;
};

} // namespace CHTL