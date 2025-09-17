#include "CompilerDispatcher.h"
#include <fstream>
#include <chrono>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher()
    : minify_output_(false), include_source_maps_(false), optimize_output_(false),
      compilation_time_(0), input_size_(0), output_size_(0) {
    initializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() {
}

void CompilerDispatcher::initializeCompilers() {
    scanner_ = std::make_unique<UnifiedScanner>();
    chtl_lexer_ = nullptr; // 将在需要时创建
    chtl_parser_ = nullptr; // 将在需要时创建
    chtl_generator_ = std::make_unique<CHTLGenerator>();
    chtljs_lexer_ = nullptr; // 将在需要时创建
    chtljs_parser_ = nullptr; // 将在需要时创建
    chtljs_generator_ = nullptr; // 将在需要时创建
}

std::string CompilerDispatcher::compile(const std::string& sourceCode) {
    startTimer();
    clearErrors();
    
    try {
        // 1. 扫描代码，分离不同语言片段
        std::vector<CodeFragment> fragments = scanCode(sourceCode);
        
        // 2. 分别编译每个片段
        std::map<CodeFragmentType, std::string> compilationResults;
        
        for (const auto& fragment : fragments) {
            std::string result = processCodeFragment(fragment);
            compilationResults[fragment.type] = result;
        }
        
        // 3. 验证编译结果
        validateCompilationResults(compilationResults);
        
        // 4. 合并编译结果
        std::string finalResult = mergeCompilationResults(compilationResults);
        
        endTimer();
        updateStatistics(sourceCode.size(), finalResult.size());
        
        return finalResult;
        
    } catch (const std::exception& e) {
        reportError("Compilation failed: " + std::string(e.what()));
        endTimer();
        return "";
    }
}

std::string CompilerDispatcher::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        reportError("Failed to open file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    
    return compile(sourceCode);
}

std::vector<CodeFragment> CompilerDispatcher::scanCode(const std::string& sourceCode) {
    if (!scanner_) {
        reportError("Scanner not initialized");
        return {};
    }
    
    try {
        return scanner_->scan(sourceCode);
    } catch (const std::exception& e) {
        reportError("Code scanning failed: " + std::string(e.what()));
        return {};
    }
}

std::string CompilerDispatcher::compileCHTL(const std::string& chtlCode) {
    try {
        // 词法分析
        if (!chtl_lexer_) {
            chtl_lexer_ = std::make_unique<CHTLLexer>(chtlCode);
        } else {
            // 重新创建lexer以处理新代码
            chtl_lexer_ = std::make_unique<CHTLLexer>(chtlCode);
        }
        
        std::vector<Token> tokens = chtl_lexer_->tokenize();
        
        // 语法分析
        if (!chtl_parser_) {
            chtl_parser_ = std::make_unique<CHTLParser>(tokens);
        } else {
            chtl_parser_ = std::make_unique<CHTLParser>(tokens);
        }
        
        std::shared_ptr<CHTLBaseNode> ast = chtl_parser_->parse();
        
        // 代码生成
        if (!chtl_generator_) {
            chtl_generator_ = std::make_unique<CHTLGenerator>();
        }
        
        return chtl_generator_->generate(ast);
        
    } catch (const std::exception& e) {
        reportError("CHTL compilation failed: " + std::string(e.what()));
        return "";
    }
}

std::string CompilerDispatcher::compileCHTLJS(const std::string& chtljsCode) {
    try {
        // 词法分析
        if (!chtljs_lexer_) {
            chtljs_lexer_ = std::make_unique<CHTLJSLexer>(chtljsCode);
        } else {
            chtljs_lexer_ = std::make_unique<CHTLJSLexer>(chtljsCode);
        }
        
        std::vector<CHTLJS_Token> tokens = chtljs_lexer_->tokenize();
        
        // 语法分析
        if (!chtljs_parser_) {
            chtljs_parser_ = std::make_unique<CHTLJSParser>(tokens);
        } else {
            chtljs_parser_ = std::make_unique<CHTLJSParser>(tokens);
        }
        
        std::shared_ptr<CHTLJSBaseNode> ast = chtljs_parser_->parse();
        
        // 代码生成
        if (!chtljs_generator_) {
            chtljs_generator_ = std::make_unique<CHTLJSGenerator>(nullptr);
        }
        
        return chtljs_generator_->generate(ast);
        
    } catch (const std::exception& e) {
        reportError("CHTL JS compilation failed: " + std::string(e.what()));
        return "";
    }
}

std::string CompilerDispatcher::compileCSS(const std::string& cssCode) {
    // 简单的CSS处理，实际项目中应该使用ANTLR4
    if (minify_output_) {
        // 简单的CSS压缩
        std::string result = cssCode;
        
        // 移除注释
        result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
        
        // 移除多余空白
        result = std::regex_replace(result, std::regex("\\s+"), " ");
        result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
        result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
        result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
        
        return result;
    }
    
    return cssCode;
}

std::string CompilerDispatcher::compileJS(const std::string& jsCode) {
    // 简单的JS处理，实际项目中应该使用ANTLR4
    if (minify_output_) {
        // 简单的JS压缩
        std::string result = jsCode;
        
        // 移除注释
        result = std::regex_replace(result, std::regex("//.*"), "");
        result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
        
        // 移除多余空白
        result = std::regex_replace(result, std::regex("\\s+"), " ");
        result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
        result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
        result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
        
        return result;
    }
    
    return jsCode;
}

void CompilerDispatcher::setOutputFormat(const std::string& format) {
    output_format_ = format;
}

void CompilerDispatcher::setMinifyOutput(bool minify) {
    minify_output_ = minify;
}

void CompilerDispatcher::setIncludeSourceMaps(bool include) {
    include_source_maps_ = include;
}

void CompilerDispatcher::setOptimizeOutput(bool optimize) {
    optimize_output_ = optimize;
}

void CompilerDispatcher::setErrorHandler(std::function<void(const std::string&, int, int)> handler) {
    error_handler_ = handler;
}

void CompilerDispatcher::reportError(const std::string& message, int line, int column) {
    logError(message, line, column);
    
    if (error_handler_) {
        error_handler_(message, line, column);
    }
}

bool CompilerDispatcher::hasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CompilerDispatcher::getErrors() const {
    return errors_;
}

void CompilerDispatcher::clearErrors() {
    errors_.clear();
}

int CompilerDispatcher::getCompilationTime() const {
    return compilation_time_;
}

size_t CompilerDispatcher::getInputSize() const {
    return input_size_;
}

size_t CompilerDispatcher::getOutputSize() const {
    return output_size_;
}

std::string CompilerDispatcher::processCodeFragment(const CodeFragment& fragment) {
    switch (fragment.type) {
        case CodeFragmentType::CHTL:
            return compileCHTL(fragment.content);
        case CodeFragmentType::CHTL_JS:
            return compileCHTLJS(fragment.content);
        case CodeFragmentType::CSS:
            return compileCSS(fragment.content);
        case CodeFragmentType::JS:
            return compileJS(fragment.content);
        default:
            reportError("Unknown code fragment type");
            return "";
    }
}

std::string CompilerDispatcher::mergeCompilationResults(const std::map<CodeFragmentType, std::string>& results) {
    std::stringstream ss;
    
    // HTML结构
    if (results.find(CodeFragmentType::CHTL) != results.end()) {
        ss << results.at(CodeFragmentType::CHTL) << "\n";
    }
    
    // CSS样式
    if (results.find(CodeFragmentType::CSS) != results.end() && !results.at(CodeFragmentType::CSS).empty()) {
        ss << "<style>\n" << results.at(CodeFragmentType::CSS) << "\n</style>\n";
    }
    
    // JavaScript代码
    if (results.find(CodeFragmentType::JS) != results.end() && !results.at(CodeFragmentType::JS).empty()) {
        ss << "<script>\n" << results.at(CodeFragmentType::JS) << "\n</script>\n";
    }
    
    // CHTL JS代码
    if (results.find(CodeFragmentType::CHTL_JS) != results.end() && !results.at(CodeFragmentType::CHTL_JS).empty()) {
        ss << "<script>\n" << results.at(CodeFragmentType::CHTL_JS) << "\n</script>\n";
    }
    
    return ss.str();
}

void CompilerDispatcher::validateCompilationResults(const std::map<CodeFragmentType, std::string>& results) {
    // 检查是否有编译结果
    if (results.empty()) {
        reportError("No compilation results generated");
        return;
    }
    
    // 检查CHTL编译结果
    if (results.find(CodeFragmentType::CHTL) != results.end()) {
        if (results.at(CodeFragmentType::CHTL).empty()) {
            reportError("CHTL compilation produced empty result");
        }
    }
    
    // 可以添加更多验证逻辑
}

void CompilerDispatcher::logError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "Error";
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    ss << ": " << message;
    
    errors_.push_back(ss.str());
}

void CompilerDispatcher::validateInput(const std::string& sourceCode) {
    if (sourceCode.empty()) {
        reportError("Input source code is empty");
        return;
    }
    
    if (sourceCode.length() > 1024 * 1024) { // 1MB limit
        reportError("Input source code is too large");
        return;
    }
}

void CompilerDispatcher::startTimer() {
    compilation_time_ = 0; // 重置计时器
    // 实际实现中应该记录开始时间
}

void CompilerDispatcher::endTimer() {
    // 实际实现中应该计算耗时
    compilation_time_ = 100; // 模拟100ms编译时间
}

void CompilerDispatcher::updateStatistics(size_t inputSize, size_t outputSize) {
    input_size_ = inputSize;
    output_size_ = outputSize;
}

} // namespace CHTL