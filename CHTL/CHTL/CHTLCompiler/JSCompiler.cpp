/**
 * @file JSCompiler.cpp
 * @brief JavaScript 编译器实现
 * 
 * 根据 CHTL.md 文档实现
 * 提供 JavaScript 编译和执行功能
 */

#include "JSCompiler.hpp"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>
#include <set>

namespace CHTL {

JSCompiler::JSCompiler() : minify_(false), optimize_(false) {}

JSCompiler::~JSCompiler() = default;

bool JSCompiler::compile(const std::string& input, std::string& output) {
    try {
        // 注入全局变量和函数
        std::string processed = injectGlobals(input);
        
        // 优化 JavaScript（如果启用）
        if (optimize_) {
            processed = optimizeJS(processed);
        }
        
        // 压缩 JavaScript（如果启用）
        if (minify_) {
            processed = minifyJS(processed);
        }
        
        output = processed;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSCompiler::execute(const std::string& code, std::string& result) {
    try {
        // 注入全局变量和函数
        std::string processed = injectGlobals(code);
        
        // 简单的 JavaScript 执行模拟
        // 在实际实现中，这里应该使用 V8 引擎或 QuickJS
        result = "// JavaScript execution result\n" + processed;
        lastResult_ = result;
        
        return true;
    } catch (const std::exception& e) {
        result = "Error: " + std::string(e.what());
        lastResult_ = result;
        return false;
    }
}

bool JSCompiler::validate(const std::string& input) {
    try {
        return validateSyntax(input);
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSCompiler::optimize(const std::string& input, std::string& output) {
    try {
        output = optimizeJS(input);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void JSCompiler::addGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables_[name] = value;
}

void JSCompiler::addGlobalFunction(const std::string& name, const std::string& code) {
    globalFunctions_[name] = code;
}

void JSCompiler::clearGlobals() {
    globalVariables_.clear();
    globalFunctions_.clear();
}

std::string JSCompiler::injectGlobals(const std::string& code) {
    std::ostringstream result;
    
    // 注入全局变量
    for (const auto& var : globalVariables_) {
        result << "var " << var.first << " = " << var.second << ";\n";
    }
    
    // 注入全局函数
    for (const auto& func : globalFunctions_) {
        result << func.second << "\n";
    }
    
    result << code;
    return result.str();
}

std::string JSCompiler::minifyJS(const std::string& input) {
    std::string result = input;
    
    // 移除单行注释
    result = std::regex_replace(result, std::regex(R"(\/\/.*$)"), "", std::regex_constants::match_default);
    
    // 移除多行注释
    result = std::regex_replace(result, std::regex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)"), "");
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空白
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除逗号后的空白
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除操作符前后的空白
    result = std::regex_replace(result, std::regex(R"(\s*([+\-*/=<>!&|])\s*)"), "$1");
    
    // 移除行首行尾空白
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)", std::regex_constants::match_default), "");
    
    return result;
}

std::string JSCompiler::optimizeJS(const std::string& input) {
    std::string result = input;
    
    // 简单的优化：移除未使用的变量声明
    // 在实际实现中，这里应该使用更复杂的优化算法
    
    // 合并连续的 var 声明
    std::regex varRegex(R"(var\s+(\w+)\s*=\s*([^;]+);\s*var\s+(\w+)\s*=\s*([^;]+);)");
    result = std::regex_replace(result, varRegex, "var $1 = $2, $3 = $4;");
    
    // 移除重复的变量声明
    std::set<std::string> declaredVars;
    std::regex varDeclRegex(R"(var\s+(\w+))");
    std::sregex_iterator iter(result.begin(), result.end(), varDeclRegex);
    std::sregex_iterator end;
    
    std::string optimized;
    size_t lastPos = 0;
    
    for (; iter != end; ++iter) {
        std::string varName = iter->str(1);
        size_t pos = iter->position();
        
        if (declaredVars.find(varName) == declaredVars.end()) {
            declaredVars.insert(varName);
            optimized += result.substr(lastPos, pos - lastPos);
            lastPos = pos;
        }
    }
    
    optimized += result.substr(lastPos);
    
    return optimized.empty() ? result : optimized;
}

bool JSCompiler::validateSyntax(const std::string& input) {
    try {
        // 基本的 JavaScript 语法验证
        std::string content = input;
        
        // 检查括号匹配
        int parenCount = 0;
        int braceCount = 0;
        int bracketCount = 0;
        
        for (char c : content) {
            switch (c) {
                case '(': parenCount++; break;
                case ')': parenCount--; break;
                case '{': braceCount++; break;
                case '}': braceCount--; break;
                case '[': bracketCount++; break;
                case ']': bracketCount--; break;
            }
        }
        
        return parenCount == 0 && braceCount == 0 && bracketCount == 0;
    } catch (const std::exception& e) {
        return false;
    }
}

} // namespace CHTL