/**
 * @file CSSCompiler.cpp
 * @brief CSS 编译器实现
 * 
 * 根据 CHTL.md 文档实现
 * 提供 CSS 编译和优化功能
 */

#include "CSSCompiler.hpp"
#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <set>
#include "../CHTLParser/PropertyExpressionParser.hpp"
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
    // 这些变量在头文件中没有定义，所以不需要初始化
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
            output = processVariables(input, false); // 默认不是全局样式
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
        std::string processed = processVariables(input, false); // 默认不是全局样式
        
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
        output = processVariables(input, false); // 默认不是全局样式
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

std::string CSSCompiler::processVariables(const std::string& input, bool isGlobalStyle) {
    std::string result = input;
    
    // 处理 CSS 变量替换
    for (const auto& var : variables_) {
        std::string pattern = "var\\(" + var.first + "\\)";
        std::regex regex(pattern);
        result = std::regex_replace(result, regex, var.second);
    }
    
    // 处理属性表达式
    result = processArithmeticExpression(result, isGlobalStyle);
    result = processPropertyReference(result, isGlobalStyle);
    result = processConditionalExpression(result, isGlobalStyle);
    result = processDynamicExpression(result, isGlobalStyle);
    
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

std::string CSSCompiler::processPropertyExpression(const std::string& value, bool isGlobalStyle) {
    // 检查全局样式支持
    if (isGlobalStyle && !isGlobalStyleSupported(value)) {
        throw std::runtime_error("Property expression not supported in global style: " + value);
    }
    
    try {
        PropertyExpressionParser parser(value);
        auto ast = parser.parse();
        return ast->toCSS();
    } catch (const std::exception& e) {
        // 如果解析失败，返回原始值
        return value;
    }
}

std::string CSSCompiler::processArithmeticExpression(const std::string& value, bool isGlobalStyle) {
    // 检查是否包含算术运算符
    if (value.find('+') != std::string::npos || 
        value.find('-') != std::string::npos || 
        value.find('*') != std::string::npos || 
        value.find('/') != std::string::npos || 
        value.find('%') != std::string::npos || 
        value.find("**") != std::string::npos) {
        return processPropertyExpression(value, isGlobalStyle);
    }
    return value;
}

std::string CSSCompiler::processPropertyReference(const std::string& value, bool isGlobalStyle) {
    // 检查是否包含属性引用 (选择器.属性)
    std::regex refRegex(R"(([.#]?[\w-]+)\.([\w-]+))");
    std::smatch match;
    
    if (std::regex_search(value, match, refRegex)) {
        // 全局样式不支持属性引用
        if (isGlobalStyle) {
            throw std::runtime_error("Property reference not supported in global style: " + value);
        }
        
        std::string selector = match[1].str();
        std::string property = match[2].str();
        
        // 转换为CSS变量引用
        return "var(--" + selector + "-" + property + ")";
    }
    
    return value;
}

std::string CSSCompiler::processConditionalExpression(const std::string& value, bool isGlobalStyle) {
    // 检查是否包含条件表达式 (条件 ? 选项 : 选项)
    if (value.find('?') != std::string::npos && value.find(':') != std::string::npos) {
        return processPropertyExpression(value, isGlobalStyle);
    }
    return value;
}

std::string CSSCompiler::processDynamicExpression(const std::string& value, bool isGlobalStyle) {
    // 检查是否包含动态引用 ({{变量}}->属性)
    if (value.find("{{") != std::string::npos && value.find("}}") != std::string::npos) {
        // 全局样式不支持动态表达式
        if (isGlobalStyle) {
            throw std::runtime_error("Dynamic expression not supported in global style: " + value);
        }
        return processPropertyExpression(value, isGlobalStyle);
    }
    return value;
}

bool CSSCompiler::isGlobalStyleSupported(const std::string& value) {
    // 全局样式只支持属性运算，不支持引用属性和动态表达式
    
    // 检查是否包含属性引用 (选择器.属性)
    std::regex refRegex(R"(([.#]?[\w-]+)\.([\w-]+))");
    if (std::regex_search(value, refRegex)) {
        return false; // 不支持属性引用
    }
    
    // 检查是否包含动态表达式 ({{变量}}->属性)
    if (value.find("{{") != std::string::npos && value.find("}}->") != std::string::npos) {
        return false; // 不支持动态表达式
    }
    
    // 检查是否包含引用属性的条件表达式
    // 如果条件表达式中包含选择器.属性，则不支持
    std::regex conditionalRefRegex(R"([\w\s]*([.#]?[\w-]+)\.([\w-]+)[\w\s]*\?[\w\s]*:[\w\s]*)");
    if (std::regex_search(value, conditionalRefRegex)) {
        return false; // 不支持引用属性的条件表达式
    }
    
    // 其他情况都支持（包括纯算术运算和基本条件表达式）
    return true;
}

} // namespace CHTL