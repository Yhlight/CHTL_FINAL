#include "Compiler/CompilerDispatcher.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : minify_html_(false), minify_css_(false), minify_js_(false), 
      strict_mode_(false), debug_mode_(false), error_count_(0), warning_count_(0) {
    
    // 初始化编译器实例
    scanner_ = std::make_unique<UnifiedScanner>();
    chtl_lexer_ = std::make_unique<CHTLLexer>();
    chtl_parser_ = std::make_unique<CHTLParser>();
    chtl_generator_ = std::make_unique<CHTLGenerator>();
    chtl_js_lexer_ = std::make_unique<CHTLJSLexer>();
    chtl_js_parser_ = std::make_unique<CHTLJSParser>();
    chtl_js_generator_ = std::make_unique<CHTLJSGenerator>();
    
    // 初始化统计信息
    resetStats();
}

CompileResult CompilerDispatcher::compile(const std::string& source_code) {
    CompileResult result;
    
    try {
        // 识别编译器类型
        CompilerType type = identifyCompilerType(source_code);
        
        switch (type) {
            case CompilerType::CHTL:
                result = compileCHTL(source_code);
                break;
            case CompilerType::CHTL_JS:
                result = compileCHTLJS(source_code);
                break;
            case CompilerType::CSS:
                result = compileCSS(source_code);
                break;
            case CompilerType::JAVASCRIPT:
                result = compileJavaScript(source_code);
                break;
            default:
                addError(result, "Unknown compiler type");
                return result;
        }
        
        // 验证编译结果
        if (!validateResult(result)) {
            addError(result, "Compilation validation failed");
        }
        
        // 更新统计信息
        stats_["total_compilations"]++;
        if (result.success) {
            stats_["successful_compilations"]++;
        } else {
            stats_["failed_compilations"]++;
        }
        
    } catch (const std::exception& e) {
        addError(result, "Compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compileFile(const std::string& file_path) {
    CompileResult result;
    
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            addError(result, "Cannot open file: " + file_path);
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source_code = buffer.str();
        
        result = compile(source_code);
        
        // 更新统计信息
        stats_["files_compiled"]++;
        
    } catch (const std::exception& e) {
        addError(result, "File compilation error: " + std::string(e.what()));
    }
    
    return result;
}

void CompilerDispatcher::setOutputOptions(bool minify_html, bool minify_css, bool minify_js, bool strict_mode) {
    minify_html_ = minify_html;
    minify_css_ = minify_css;
    minify_js_ = minify_js;
    strict_mode_ = strict_mode;
    
    // 设置各个编译器的选项
    if (chtl_generator_) {
        chtl_generator_->setMinify(minify_html_);
    }
    if (chtl_js_generator_) {
        chtl_js_generator_->setMinify(minify_js_);
        chtl_js_generator_->setStrictMode(strict_mode_);
    }
}

void CompilerDispatcher::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
    
    // 设置各个编译器的调试模式
    if (chtl_lexer_) {
        chtl_lexer_->setDebugMode(enabled);
    }
    if (chtl_parser_) {
        chtl_parser_->setDebugMode(enabled);
    }
    if (chtl_js_lexer_) {
        chtl_js_lexer_->setDebugMode(enabled);
    }
    if (chtl_js_parser_) {
        chtl_js_parser_->setDebugMode(enabled);
    }
}

std::unordered_map<std::string, int> CompilerDispatcher::getCompileStats() const {
    return stats_;
}

void CompilerDispatcher::resetStats() {
    stats_.clear();
    stats_["total_compilations"] = 0;
    stats_["successful_compilations"] = 0;
    stats_["failed_compilations"] = 0;
    stats_["files_compiled"] = 0;
    stats_["errors"] = 0;
    stats_["warnings"] = 0;
    error_count_ = 0;
    warning_count_ = 0;
}

CompilerType CompilerDispatcher::identifyCompilerType(const std::string& source_code) {
    // 简单的编译器类型识别逻辑
    // 可以根据文件扩展名、代码特征等来识别
    
    // 检查是否包含CHTL JS关键字
    if (source_code.find("listen") != std::string::npos ||
        source_code.find("animate") != std::string::npos ||
        source_code.find("delegate") != std::string::npos ||
        source_code.find("router") != std::string::npos ||
        source_code.find("fileloader") != std::string::npos ||
        source_code.find("vir") != std::string::npos ||
        source_code.find("iNeverAway") != std::string::npos ||
        source_code.find("util") != std::string::npos) {
        return CompilerType::CHTL_JS;
    }
    
    // 检查是否包含CHTL关键字
    if (source_code.find("html") != std::string::npos ||
        source_code.find("head") != std::string::npos ||
        source_code.find("body") != std::string::npos ||
        source_code.find("div") != std::string::npos ||
        source_code.find("text") != std::string::npos ||
        source_code.find("style") != std::string::npos) {
        return CompilerType::CHTL;
    }
    
    // 检查是否包含CSS特征
    if (source_code.find("{") != std::string::npos &&
        source_code.find("}") != std::string::npos &&
        source_code.find(":") != std::string::npos &&
        source_code.find(";") != std::string::npos) {
        return CompilerType::CSS;
    }
    
    // 检查是否包含JavaScript特征
    if (source_code.find("function") != std::string::npos ||
        source_code.find("var") != std::string::npos ||
        source_code.find("let") != std::string::npos ||
        source_code.find("const") != std::string::npos ||
        source_code.find("if") != std::string::npos ||
        source_code.find("for") != std::string::npos ||
        source_code.find("while") != std::string::npos) {
        return CompilerType::JAVASCRIPT;
    }
    
    return CompilerType::UNKNOWN;
}

CompileResult CompilerDispatcher::compileCHTL(const std::string& source_code) {
    CompileResult result;
    
    try {
        // 使用统一扫描器分离代码
        auto fragments = scanner_->scan(source_code);
        
        // 编译CHTL片段
        for (const auto& fragment : fragments) {
            if (fragment.type == UnifiedScanner::CodeType::CHTL) {
                // 词法分析
                auto tokens = chtl_lexer_->tokenize(fragment.content);
                
                // 语法分析
                auto ast = chtl_parser_->parse(tokens);
                
                if (ast) {
                    // 代码生成
                    std::string html = chtl_generator_->generateHTML(ast);
                    std::string css = chtl_generator_->generateCSS(ast);
                    std::string js = chtl_generator_->generateJavaScript(ast);
                    
                    result.html += html;
                    result.css += css;
                    result.javascript += js;
                } else {
                    auto errors = chtl_parser_->getErrors();
                    for (const auto& error : errors) {
                        addError(result, error);
                    }
                }
            }
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        addError(result, "CHTL compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compileCHTLJS(const std::string& source_code) {
    CompileResult result;
    
    try {
        // 使用统一扫描器分离代码
        auto fragments = scanner_->scan(source_code);
        
        // 编译CHTL JS片段
        for (const auto& fragment : fragments) {
            if (fragment.type == UnifiedScanner::CodeType::CHTL_JS) {
                // 词法分析
                auto tokens = chtl_js_lexer_->tokenize(fragment.content);
                
                // 语法分析
                auto ast = chtl_js_parser_->parse(tokens);
                
                if (ast) {
                    // 代码生成
                    std::string js = chtl_js_generator_->generateJavaScript(ast);
                    result.javascript += js;
                } else {
                    auto errors = chtl_js_parser_->getErrors();
                    for (const auto& error : errors) {
                        addError(result, error);
                    }
                }
            }
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        addError(result, "CHTL JS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compileCSS(const std::string& source_code) {
    CompileResult result;
    
    try {
        // 简单的CSS编译（直接返回）
        result.css = source_code;
        result.success = true;
        
    } catch (const std::exception& e) {
        addError(result, "CSS compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compileJavaScript(const std::string& source_code) {
    CompileResult result;
    
    try {
        // 简单的JavaScript编译（直接返回）
        result.javascript = source_code;
        result.success = true;
        
    } catch (const std::exception& e) {
        addError(result, "JavaScript compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::mergeResults(const std::vector<CompileResult>& results) {
    CompileResult merged_result;
    
    for (const auto& result : results) {
        if (result.success) {
            merged_result.html += result.html;
            merged_result.css += result.css;
            merged_result.javascript += result.javascript;
        } else {
            // 合并错误信息
            for (const auto& error : result.errors) {
                addError(merged_result, error);
            }
        }
    }
    
    merged_result.success = merged_result.errors.empty();
    return merged_result;
}

bool CompilerDispatcher::validateResult(const CompileResult& result) {
    // 简单的验证逻辑
    if (result.success) {
        // 检查是否有基本的输出
        return !result.html.empty() || !result.css.empty() || !result.javascript.empty();
    }
    
    return false;
}

void CompilerDispatcher::addError(CompileResult& result, const std::string& error) {
    result.errors.push_back(error);
    result.success = false;
    error_count_++;
    stats_["errors"] = error_count_;
}

void CompilerDispatcher::addWarning(CompileResult& result, const std::string& warning) {
    result.warnings.push_back(warning);
    warning_count_++;
    stats_["warnings"] = warning_count_;
}

} // namespace CHTL