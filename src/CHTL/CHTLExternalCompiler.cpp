#include "CHTL/CHTLExternalCompiler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

// 检查编译时定义
#ifdef ANTLR4_AVAILABLE
#include <antlr4-runtime.h>
#endif

#ifdef LIBSASS_AVAILABLE
#include <sass.h>
#endif

#ifdef V8_AVAILABLE
#include <v8.h>
#endif

#ifdef JSON_AVAILABLE
#include <nlohmann/json.hpp>
#endif

namespace CHTL {

// ANTLR4CSSCompiler 实现
ANTLR4CSSCompiler::ANTLR4CSSCompiler()
    : m_outputStyle("expanded"), m_sourceMap(false), m_precision(5) {
}

std::string ANTLR4CSSCompiler::compile(const std::string& css) {
    clearErrors();
    
    try {
        // 这里应该使用 ANTLR4 进行 CSS 解析和编译
        // 简化实现，实际应该使用 ANTLR4 语法树
        return processCSS(css);
    } catch (const std::exception& e) {
        addError("CSS compilation error: " + std::string(e.what()));
        return css; // 返回原始 CSS
    }
}

std::string ANTLR4CSSCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open CSS file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string css = buffer.str();
    
    return compile(css);
}

void ANTLR4CSSCompiler::setOutputStyle(const std::string& style) {
    m_outputStyle = style;
}

void ANTLR4CSSCompiler::setSourceMap(bool enable) {
    m_sourceMap = enable;
}

void ANTLR4CSSCompiler::setPrecision(int precision) {
    m_precision = precision;
}

std::vector<std::string> ANTLR4CSSCompiler::getErrors() const {
    return m_errors;
}

bool ANTLR4CSSCompiler::hasErrors() const {
    return !m_errors.empty();
}

void ANTLR4CSSCompiler::clearErrors() {
    m_errors.clear();
}

std::string ANTLR4CSSCompiler::processCSS(const std::string& css) const {
    // 简化的 CSS 处理
    std::string result = css;
    
    // 移除注释
    std::regex commentRegex(R"(/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 压缩空白
    if (m_outputStyle == "compressed") {
        std::regex whitespaceRegex(R"(\s+)");
        result = std::regex_replace(result, whitespaceRegex, " ");
        
        // 移除不必要的空格
        std::regex unnecessarySpace(R"(\s*([{}:;,])\s*)");
        result = std::regex_replace(result, unnecessarySpace, "$1");
    }
    
    return result;
}

void ANTLR4CSSCompiler::addError(const std::string& error) {
    m_errors.push_back(error);
}

// ANTLR4JSCompiler 实现
ANTLR4JSCompiler::ANTLR4JSCompiler()
    : m_target("es5"), m_optimizationLevel(1), m_sourceMap(false) {
}

std::string ANTLR4JSCompiler::compile(const std::string& js) {
    clearErrors();
    
    try {
        // 这里应该使用 ANTLR4 进行 JavaScript 解析和编译
        // 简化实现，实际应该使用 ANTLR4 语法树
        return processJS(js);
    } catch (const std::exception& e) {
        addError("JavaScript compilation error: " + std::string(e.what()));
        return js; // 返回原始 JS
    }
}

std::string ANTLR4JSCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open JavaScript file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string js = buffer.str();
    
    return compile(js);
}

std::string ANTLR4JSCompiler::execute(const std::string& js) {
    clearErrors();
    
    try {
        // 这里应该使用 JavaScript 引擎执行代码
        // 简化实现，返回执行结果
        return "// Executed: " + js;
    } catch (const std::exception& e) {
        addError("JavaScript execution error: " + std::string(e.what()));
        return "";
    }
}

std::string ANTLR4JSCompiler::executeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open JavaScript file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string js = buffer.str();
    
    return execute(js);
}

void ANTLR4JSCompiler::setTarget(const std::string& target) {
    m_target = target;
}

void ANTLR4JSCompiler::setOptimizationLevel(int level) {
    m_optimizationLevel = level;
}

void ANTLR4JSCompiler::setSourceMap(bool enable) {
    m_sourceMap = enable;
}

std::vector<std::string> ANTLR4JSCompiler::getErrors() const {
    return m_errors;
}

bool ANTLR4JSCompiler::hasErrors() const {
    return !m_errors.empty();
}

void ANTLR4JSCompiler::clearErrors() {
    m_errors.clear();
}

std::string ANTLR4JSCompiler::processJS(const std::string& js) const {
    // 简化的 JavaScript 处理
    std::string result = js;
    
    // 移除注释
    std::regex commentRegex(R"(//.*$|/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 压缩空白
    if (m_optimizationLevel > 0) {
        std::regex whitespaceRegex(R"(\s+)");
        result = std::regex_replace(result, whitespaceRegex, " ");
        
        // 移除不必要的空格
        std::regex unnecessarySpace(R"(\s*([{}();,=])\s*)");
        result = std::regex_replace(result, unnecessarySpace, "$1");
    }
    
    return result;
}

void ANTLR4JSCompiler::addError(const std::string& error) {
    m_errors.push_back(error);
}

// LibSassCSSCompiler 实现
LibSassCSSCompiler::LibSassCSSCompiler()
    : m_outputStyle("expanded"), m_sourceMap(false), m_precision(5) {
}

std::string LibSassCSSCompiler::compile(const std::string& css) {
    clearErrors();
    
    try {
        // 这里应该使用 libsass 进行 SASS/SCSS 编译
        // 简化实现，实际应该使用 libsass API
        return processSass(css);
    } catch (const std::exception& e) {
        addError("SASS compilation error: " + std::string(e.what()));
        return css; // 返回原始 CSS
    }
}

std::string LibSassCSSCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open SASS file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sass = buffer.str();
    
    return compile(sass);
}

void LibSassCSSCompiler::setOutputStyle(const std::string& style) {
    m_outputStyle = style;
}

void LibSassCSSCompiler::setSourceMap(bool enable) {
    m_sourceMap = enable;
}

void LibSassCSSCompiler::setPrecision(int precision) {
    m_precision = precision;
}

std::vector<std::string> LibSassCSSCompiler::getErrors() const {
    return m_errors;
}

bool LibSassCSSCompiler::hasErrors() const {
    return !m_errors.empty();
}

void LibSassCSSCompiler::clearErrors() {
    m_errors.clear();
}

std::string LibSassCSSCompiler::processSass(const std::string& sass) const {
    // 简化的 SASS 处理
    std::string result = sass;
    
    // 处理变量
    std::regex variableRegex(R"(\$(\w+):\s*([^;]+);)");
    std::smatch match;
    std::unordered_map<std::string, std::string> variables;
    
    while (std::regex_search(result, match, variableRegex)) {
        variables[match[1].str()] = match[2].str();
        result = std::regex_replace(result, variableRegex, "");
    }
    
    // 替换变量使用
    for (const auto& [var, value] : variables) {
        std::regex varUseRegex("\\$" + var + "\\b");
        result = std::regex_replace(result, varUseRegex, value);
    }
    
    // 处理嵌套
    // 这里应该实现 SASS 嵌套规则的处理
    // 简化实现，直接返回结果
    
    return result;
}

void LibSassCSSCompiler::addError(const std::string& error) {
    m_errors.push_back(error);
}

// V8JSCompiler 实现
V8JSCompiler::V8JSCompiler()
    : m_target("es5"), m_optimizationLevel(1), m_sourceMap(false) {
}

std::string V8JSCompiler::compile(const std::string& js) {
    clearErrors();
    
    try {
        // 这里应该使用 V8 进行 JavaScript 编译
        // 简化实现，实际应该使用 V8 API
        return processJS(js);
    } catch (const std::exception& e) {
        addError("V8 JavaScript compilation error: " + std::string(e.what()));
        return js; // 返回原始 JS
    }
}

std::string V8JSCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open JavaScript file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string js = buffer.str();
    
    return compile(js);
}

std::string V8JSCompiler::execute(const std::string& js) {
    clearErrors();
    
    try {
        // 这里应该使用 V8 执行 JavaScript 代码
        // 简化实现，返回执行结果
        return "// V8 Executed: " + js;
    } catch (const std::exception& e) {
        addError("V8 JavaScript execution error: " + std::string(e.what()));
        return "";
    }
}

std::string V8JSCompiler::executeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open JavaScript file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string js = buffer.str();
    
    return execute(js);
}

void V8JSCompiler::setTarget(const std::string& target) {
    m_target = target;
}

void V8JSCompiler::setOptimizationLevel(int level) {
    m_optimizationLevel = level;
}

void V8JSCompiler::setSourceMap(bool enable) {
    m_sourceMap = enable;
}

std::vector<std::string> V8JSCompiler::getErrors() const {
    return m_errors;
}

bool V8JSCompiler::hasErrors() const {
    return !m_errors.empty();
}

void V8JSCompiler::clearErrors() {
    m_errors.clear();
}

std::string V8JSCompiler::processJS(const std::string& js) const {
    // 简化的 V8 JavaScript 处理
    std::string result = js;
    
    // 移除注释
    std::regex commentRegex(R"(//.*$|/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 压缩空白
    if (m_optimizationLevel > 0) {
        std::regex whitespaceRegex(R"(\s+)");
        result = std::regex_replace(result, whitespaceRegex, " ");
        
        // 移除不必要的空格
        std::regex unnecessarySpace(R"(\s*([{}();,=])\s*)");
        result = std::regex_replace(result, unnecessarySpace, "$1");
    }
    
    return result;
}

void V8JSCompiler::addError(const std::string& error) {
    m_errors.push_back(error);
}

// ExternalCompilerManager 实现
ExternalCompilerManager::ExternalCompilerManager() {
    initializeDefaultCompilers();
}

void ExternalCompilerManager::setCSSCompiler(std::shared_ptr<CSSCompiler> compiler) {
    m_cssCompiler = compiler;
}

void ExternalCompilerManager::setJSCompiler(std::shared_ptr<JSCompiler> compiler) {
    m_jsCompiler = compiler;
}

std::shared_ptr<CSSCompiler> ExternalCompilerManager::getCSSCompiler() const {
    return m_cssCompiler;
}

std::shared_ptr<JSCompiler> ExternalCompilerManager::getJSCompiler() const {
    return m_jsCompiler;
}

std::string ExternalCompilerManager::compileCSS(const std::string& css) {
    if (!m_cssCompiler) {
        return css; // 返回原始 CSS
    }
    
    return m_cssCompiler->compile(css);
}

std::string ExternalCompilerManager::compileJS(const std::string& js) {
    if (!m_jsCompiler) {
        return js; // 返回原始 JS
    }
    
    return m_jsCompiler->compile(js);
}

std::string ExternalCompilerManager::executeJS(const std::string& js) {
    if (!m_jsCompiler) {
        return ""; // 无法执行
    }
    
    return m_jsCompiler->execute(js);
}

void ExternalCompilerManager::useANTLR4Compilers() {
    m_cssCompiler = std::make_shared<ANTLR4CSSCompiler>();
    m_jsCompiler = std::make_shared<ANTLR4JSCompiler>();
}

void ExternalCompilerManager::useLibSassV8Compilers() {
    m_cssCompiler = std::make_shared<LibSassCSSCompiler>();
    m_jsCompiler = std::make_shared<V8JSCompiler>();
}

void ExternalCompilerManager::useDefaultCompilers() {
    initializeDefaultCompilers();
}

std::vector<std::string> ExternalCompilerManager::getCSSErrors() const {
    if (m_cssCompiler) {
        return m_cssCompiler->getErrors();
    }
    return {};
}

std::vector<std::string> ExternalCompilerManager::getJSErrors() const {
    if (m_jsCompiler) {
        return m_jsCompiler->getErrors();
    }
    return {};
}

bool ExternalCompilerManager::hasErrors() const {
    return (!m_cssCompiler || m_cssCompiler->hasErrors()) ||
           (!m_jsCompiler || m_jsCompiler->hasErrors());
}

void ExternalCompilerManager::clearErrors() {
    if (m_cssCompiler) {
        m_cssCompiler->clearErrors();
    }
    if (m_jsCompiler) {
        m_jsCompiler->clearErrors();
    }
}

void ExternalCompilerManager::initializeDefaultCompilers() {
    // 默认使用 ANTLR4 编译器
    useANTLR4Compilers();
}

} // namespace CHTL