#include "Scanner/CompilerDispatcher.h"

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : m_prettyPrint(true), m_minify(false), m_debugMode(false), m_strictMode(false),
      m_compiledFragments(0), m_compiledLines(0), m_compiledSize(0), m_compileTime(0.0) {
    initializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

CompileResult CompilerDispatcher::compile(const std::string& source) {
    // 使用统一扫描器分离代码片段
    auto fragments = m_scanner->scan(source);
    
    // 编译代码片段
    return compileFragments(fragments);
}

CompileResult CompilerDispatcher::compileFragments(const std::vector<CodeFragment>& fragments) {
    CompileResult result;
    
    std::string html;
    std::string css;
    std::string javascript;
    
    for (const auto& fragment : fragments) {
        CompileResult fragmentResult;
        
        switch (fragment.type) {
            case CodeFragmentType::CHTL:
                fragmentResult = compileCHTL(fragment);
                break;
            case CodeFragmentType::CHTL_JS:
                fragmentResult = compileCHTLJS(fragment);
                break;
            case CodeFragmentType::CSS:
                fragmentResult = compileCSS(fragment);
                break;
            case CodeFragmentType::JAVASCRIPT:
                fragmentResult = compileJavaScript(fragment);
                break;
        }
        
        if (fragmentResult.success) {
            html += fragmentResult.html;
            css += fragmentResult.css;
            javascript += fragmentResult.javascript;
        } else {
            result.errors.insert(result.errors.end(), fragmentResult.errors.begin(), fragmentResult.errors.end());
        }
    }
    
    // 合并结果
    result.html = m_merger->merge(html, css, javascript);
    result.css = css;
    result.javascript = javascript;
    result.success = result.errors.empty();
    
    m_compiledFragments = fragments.size();
    m_compiledSize = result.html.length();
    
    return result;
}

void CompilerDispatcher::setPrettyPrint(bool enabled) {
    m_prettyPrint = enabled;
}

void CompilerDispatcher::setMinify(bool enabled) {
    m_minify = enabled;
}

void CompilerDispatcher::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void CompilerDispatcher::setStrictMode(bool enabled) {
    m_strictMode = enabled;
}

size_t CompilerDispatcher::getCompiledFragments() const {
    return m_compiledFragments;
}

size_t CompilerDispatcher::getCompiledLines() const {
    return m_compiledLines;
}

size_t CompilerDispatcher::getCompiledSize() const {
    return m_compiledSize;
}

double CompilerDispatcher::getCompileTime() const {
    return m_compileTime;
}

CompileResult CompilerDispatcher::compileCHTL(const CodeFragment& fragment) {
    CompileResult result;
    
    // TODO: 实现CHTL编译
    result.html = fragment.content;
    result.success = true;
    
    return result;
}

CompileResult CompilerDispatcher::compileCHTLJS(const CodeFragment& fragment) {
    CompileResult result;
    
    // TODO: 实现CHTL JS编译
    result.javascript = fragment.content;
    result.success = true;
    
    return result;
}

CompileResult CompilerDispatcher::compileCSS(const CodeFragment& fragment) {
    CompileResult result;
    
    // TODO: 实现CSS编译
    result.css = fragment.content;
    result.success = true;
    
    return result;
}

CompileResult CompilerDispatcher::compileJavaScript(const CodeFragment& fragment) {
    CompileResult result;
    
    // TODO: 实现JavaScript编译
    result.javascript = fragment.content;
    result.success = true;
    
    return result;
}

void CompilerDispatcher::initializeCompilers() {
    m_context = std::make_shared<CHTLContext>();
    m_scanner = std::make_shared<CHTLUnifiedScanner>();
    m_merger = std::make_shared<CodeMerger>();
    
    initializeCHTLCompiler();
    initializeCHTLJSCompiler();
    initializeCSSCompiler();
    initializeJavaScriptCompiler();
}

void CompilerDispatcher::initializeCHTLCompiler() {
    // TODO: 初始化CHTL编译器
}

void CompilerDispatcher::initializeCHTLJSCompiler() {
    // TODO: 初始化CHTL JS编译器
}

void CompilerDispatcher::initializeCSSCompiler() {
    // TODO: 初始化CSS编译器
}

void CompilerDispatcher::initializeJavaScriptCompiler() {
    // TODO: 初始化JavaScript编译器
}

void CompilerDispatcher::addError(const std::string& error) {
    m_errors.push_back(error);
}

void CompilerDispatcher::addWarning(const std::string& warning) {
    m_warnings.push_back(warning);
}

const std::vector<std::string>& CompilerDispatcher::getErrors() const {
    return m_errors;
}

const std::vector<std::string>& CompilerDispatcher::getWarnings() const {
    return m_warnings;
}

bool CompilerDispatcher::hasErrors() const {
    return !m_errors.empty();
}

bool CompilerDispatcher::hasWarnings() const {
    return !m_warnings.empty();
}

} // namespace CHTL