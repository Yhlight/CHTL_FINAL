/**
 * @file CSSCompiler.cpp
 * @brief CSS 编译器实现
 * 
 * 根据 CHTL.md 文档实现
 * 提供 CSS 编译和优化功能
 */

#include "CSSCompiler.hpp"
#include <algorithm>
#include <regex>
#include <sstream>
#include <set>
#ifdef HAVE_LIBCSS
#include <libcss/libcss.h>
#include <libcss/select.h>
#include <libcss/parse.h>
#endif

namespace CHTL {

CSSCompiler::CSSCompiler() : minify_(false), optimize_(false) {
#ifdef HAVE_LIBCSS
    // 初始化 libcss
    css_error error = css_initialise("", 0, &alloc_, &realloc_, &free_, &user_);
    if (error != CSS_OK) {
        // 如果初始化失败，使用默认的分配器
        alloc_ = malloc;
        realloc_ = realloc;
        free_ = free;
        user_ = nullptr;
    }
    
    // 创建 CSS 上下文
    error = css_select_ctx_create(&select_ctx_);
    if (error != CSS_OK) {
        select_ctx_ = nullptr;
    }
#else
    // 如果没有 libcss，使用默认的分配器
    alloc_ = malloc;
    realloc_ = realloc;
    free_ = free;
    user_ = nullptr;
    select_ctx_ = nullptr;
#endif
}

CSSCompiler::~CSSCompiler() {
#ifdef HAVE_LIBCSS
    if (select_ctx_) {
        css_select_ctx_destroy(select_ctx_);
    }
    if (alloc_ != malloc) {
        css_finalise(alloc_, realloc_, free_, user_);
    }
#endif
}

bool CSSCompiler::compile(const std::string& input, std::string& output) {
    try {
#ifdef HAVE_LIBCSS
        if (!select_ctx_) {
            // 如果 libcss 不可用，使用基本处理
            output = processVariables(input);
            if (optimize_) {
                output = optimizeCSS(output);
            }
            if (minify_) {
                output = minifyCSS(output);
            }
            return true;
        }
        
        // 使用 libcss 解析 CSS
        css_stylesheet* stylesheet = nullptr;
        css_error error = css_stylesheet_create(CSS_LEVEL_3, &stylesheet);
        if (error != CSS_OK) {
            return false;
        }
        
        // 解析 CSS 内容
        size_t input_len = input.length();
        error = css_stylesheet_append_data(stylesheet, 
                                         reinterpret_cast<const uint8_t*>(input.c_str()), 
                                         input_len);
        if (error != CSS_OK && error != CSS_NEEDDATA) {
            css_stylesheet_destroy(stylesheet);
            return false;
        }
        
        // 完成解析
        error = css_stylesheet_data_done(stylesheet);
        if (error != CSS_OK) {
            css_stylesheet_destroy(stylesheet);
            return false;
        }
        
        // 处理 CSS 变量
        std::string processed = processVariables(input);
        
        // 优化 CSS（如果启用）
        if (optimize_) {
            processed = optimizeCSS(processed);
        }
        
        // 压缩 CSS（如果启用）
        if (minify_) {
            processed = minifyCSS(processed);
        }
        
        output = processed;
        
        // 清理
        css_stylesheet_destroy(stylesheet);
        return true;
#else
        // 如果没有 libcss，使用基本处理
        output = processVariables(input);
        if (optimize_) {
            output = optimizeCSS(output);
        }
        if (minify_) {
            output = minifyCSS(output);
        }
        return true;
#endif
    } catch (const std::exception& e) {
        return false;
    }
}

bool CSSCompiler::optimize(const std::string& input, std::string& output) {
    try {
        output = optimizeCSS(input);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool CSSCompiler::validate(const std::string& input) {
    try {
        // 基本的 CSS 语法验证
        std::regex cssRule(R"(\s*[^{]+\s*\{\s*[^}]*\s*\})");
        std::smatch match;
        
        std::string content = input;
        while (std::regex_search(content, match, cssRule)) {
            content = match.suffix();
        }
        
        // 检查是否还有未匹配的内容
        content = std::regex_replace(content, std::regex(R"(\s+)"), "");
        return content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos;
    } catch (const std::exception& e) {
        return false;
    }
}

void CSSCompiler::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CSSCompiler::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

void CSSCompiler::clearVariables() {
    variables_.clear();
}

std::string CSSCompiler::processVariables(const std::string& input) {
    std::string result = input;
    
    // 处理 CSS 变量替换
    for (const auto& var : variables_) {
        std::string pattern = "var\\(" + var.first + "\\)";
        std::regex regex(pattern);
        result = std::regex_replace(result, regex, var.second);
    }
    
    return result;
}

std::string CSSCompiler::minifyCSS(const std::string& input) {
    std::string result = input;
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)"), "");
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空白
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除冒号前后的空白
    result = std::regex_replace(result, std::regex(R"(\s*:\s*)"), ":");
    
    // 移除大括号前后的空白
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除逗号后的空白
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除行首行尾空白
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)", std::regex_constants::multiline), "");
    
    return result;
}

std::string CSSCompiler::optimizeCSS(const std::string& input) {
    std::string result = input;
    
    // 合并相同的选择器
    std::map<std::string, std::vector<std::string>> selectorMap;
    std::regex ruleRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator iter(result.begin(), result.end(), ruleRegex);
    std::sregex_iterator end;
    
    std::string optimized;
    std::set<std::string> processedRules;
    
    for (; iter != end; ++iter) {
        std::string selector = iter->str(1);
        std::string properties = iter->str(2);
        
        // 清理选择器和属性
        selector = std::regex_replace(selector, std::regex(R"(\s+)"), " ");
        selector = std::regex_replace(selector, std::regex(R"(^\s+|\s+$)"), "");
        
        properties = std::regex_replace(properties, std::regex(R"(\s+)"), " ");
        properties = std::regex_replace(properties, std::regex(R"(^\s+|\s+$)"), "");
        
        if (processedRules.find(selector) == processedRules.end()) {
            processedRules.insert(selector);
            optimized += selector + "{" + properties + "}";
        }
    }
    
    return optimized.empty() ? result : optimized;
}

} // namespace CHTL