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

#ifdef HAVE_V8
#include <v8.h>
#include <libplatform/libplatform.h>
#endif

namespace CHTL {

JSCompiler::JSCompiler() : minify_(false), optimize_(false) {
#ifdef HAVE_V8
    // 初始化 V8 平台
    v8::V8::InitializeICUDefaultLocation(".");
    v8::V8::InitializeExternalStartupData(".");
    platform_ = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform_.get());
    v8::V8::Initialize();
    
    // 创建 V8 隔离
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate_ = v8::Isolate::New(create_params);
#else
    // 如果没有 V8，使用基本实现
    platform_ = nullptr;
    isolate_ = nullptr;
#endif
}

JSCompiler::~JSCompiler() {
#ifdef HAVE_V8
    if (isolate_) {
        isolate_->Dispose();
    }
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
#endif
}

bool JSCompiler::compile(const std::string& input, std::string& output) {
    try {
#ifdef HAVE_V8
        if (!isolate_) {
            output = input;
            return true;
        }
        
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 注入全局变量和函数
        std::string processed = injectGlobals(input);
        
        // 创建 V8 字符串
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, processed.c_str()).ToLocalChecked();
        
        // 编译脚本
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // 执行脚本
        v8::Local<v8::Value> script_result = script->Run(context).ToLocalChecked();
        
        // 转换结果为字符串
        v8::String::Utf8Value utf8(isolate_, script_result);
        output = std::string(*utf8);
        
        return true;
#else
        // 如果没有 V8，使用基本实现
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
#endif
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSCompiler::execute(const std::string& code, std::string& result) {
    try {
#ifdef HAVE_V8
        if (!isolate_) {
            result = "Error: V8 isolate not initialized";
            return false;
        }
        
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 注入全局变量和函数
        std::string processed = injectGlobals(code);
        
        // 创建 V8 字符串
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, processed.c_str()).ToLocalChecked();
        
        // 编译脚本
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // 执行脚本
        v8::Local<v8::Value> script_result = script->Run(context).ToLocalChecked();
        
        // 转换结果为字符串
        v8::String::Utf8Value utf8(isolate_, script_result);
        result = std::string(*utf8);
        lastResult_ = result;
        
        return true;
#else
        // 如果没有 V8，使用基本实现
        std::string processed = injectGlobals(code);
        result = "// JavaScript execution result (V8 not available)\n" + processed;
        lastResult_ = result;
        return true;
#endif
    } catch (const std::exception& e) {
        result = "Error: " + std::string(e.what());
        lastResult_ = result;
        return false;
    }
}

bool JSCompiler::validate(const std::string& input) {
    try {
#ifdef HAVE_V8
        if (!isolate_) {
            return validateSyntax(input);
        }
        
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 尝试编译脚本
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, input.c_str()).ToLocalChecked();
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        return true;
#else
        // 如果没有 V8，使用基本实现
        return validateSyntax(input);
#endif
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSCompiler::optimize(const std::string& input, std::string& output) {
    try {
#ifdef HAVE_V8
        if (!isolate_) {
            output = optimizeJS(input);
            return true;
        }
        
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 注入全局变量和函数
        std::string processed = injectGlobals(input);
        
        // 创建 V8 字符串
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, processed.c_str()).ToLocalChecked();
        
        // 编译脚本
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // 执行脚本
        v8::Local<v8::Value> script_result = script->Run(context).ToLocalChecked();
        
        // 转换结果为字符串
        v8::String::Utf8Value utf8(isolate_, script_result);
        output = std::string(*utf8);
        
        return true;
#else
        // 如果没有 V8，使用基本实现
        output = optimizeJS(input);
        return true;
#endif
    } catch (const std::exception& e) {
        return false;
    }
}

void JSCompiler::addGlobalVariable(const std::string& name, const std::string& value) {
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 设置全局变量
        v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate_, name.c_str()).ToLocalChecked();
        v8::Local<v8::String> val = v8::String::NewFromUtf8(isolate_, value.c_str()).ToLocalChecked();
        context->Global()->Set(key, val);
    }
#endif
    
    // 如果没有 V8，使用基本实现
    globalVariables_[name] = value;
}

void JSCompiler::addGlobalFunction(const std::string& name, const std::string& code) {
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 设置全局函数
        v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate_, name.c_str()).ToLocalChecked();
        v8::Local<v8::String> val = v8::String::NewFromUtf8(isolate_, code.c_str()).ToLocalChecked();
        context->Global()->Set(key, val);
    }
#endif
    
    // 如果没有 V8，使用基本实现
    globalFunctions_[name] = code;
}

void JSCompiler::clearGlobals() {
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 清除全局变量和函数
        context->Global()->Clear();
    }
#endif
    
    // 如果没有 V8，使用基本实现
    globalVariables_.clear();
    globalFunctions_.clear();
}

std::string JSCompiler::injectGlobals(const std::string& code) {
    std::ostringstream result;
    
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
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
#endif
    
    // 如果没有 V8，使用基本实现
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
    
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 注入全局变量和函数
        std::string processed = injectGlobals(input);
        
        // 创建 V8 字符串
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, processed.c_str()).ToLocalChecked();
        
        // 编译脚本
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // 执行脚本
        v8::Local<v8::Value> script_result = script->Run(context).ToLocalChecked();
        
        // 转换结果为字符串
        v8::String::Utf8Value utf8(isolate_, script_result);
        result = std::string(*utf8);
        
        return result;
    }
#endif
    
    // 如果没有 V8，使用基本实现
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
    
#ifdef HAVE_V8
    if (isolate_) {
        // 创建 V8 作用域
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        // 创建上下文
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        v8::Context::Scope context_scope(context);
        
        // 注入全局变量和函数
        std::string processed = injectGlobals(input);
        
        // 创建 V8 字符串
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, processed.c_str()).ToLocalChecked();
        
        // 编译脚本
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // 执行脚本
        v8::Local<v8::Value> script_result = script->Run(context).ToLocalChecked();
        
        // 转换结果为字符串
        v8::String::Utf8Value utf8(isolate_, script_result);
        result = std::string(*utf8);
        
        return result;
    }
#endif
    
    // 如果没有 V8，使用基本实现
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
#ifdef HAVE_V8
        if (isolate_) {
            // 创建 V8 作用域
            v8::Isolate::Scope isolate_scope(isolate_);
            v8::HandleScope handle_scope(isolate_);
            
            // 创建上下文
            v8::Local<v8::Context> context = v8::Context::New(isolate_);
            v8::Context::Scope context_scope(context);
            
            // 尝试编译脚本
            v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, input.c_str()).ToLocalChecked();
            v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
            
            return true;
        }
#endif
        
        // 如果没有 V8，使用基本实现
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