/**
 * @file CompilerManager.hpp
 * @brief 编译器管理器
 * 
 * 根据 CHTL.md 文档实现
 * 统一管理 CSS 和 JavaScript 编译器
 */

#pragma once

#include "CSSCompiler.hpp"
#include "JSCompiler.hpp"
#include <memory>

namespace CHTL {

/**
 * @brief 编译器管理器
 * 
 * 根据 CHTL.md 文档实现
 * 统一管理 CSS 和 JavaScript 编译器
 */
class CompilerManager {
public:
    CompilerManager();
    ~CompilerManager();
    
    // 获取 CSS 编译器
    CSSCompiler& getCSSCompiler() { return *cssCompiler_; }
    
    // 获取 JavaScript 编译器
    JSCompiler& getJSCompiler() { return *jsCompiler_; }
    
    // 编译 CSS 代码
    bool compileCSS(const std::string& input, std::string& output);
    
    // 编译 JavaScript 代码
    bool compileJS(const std::string& input, std::string& output);
    
    // 验证 CSS 语法
    bool validateCSS(const std::string& input);
    
    // 验证 JavaScript 语法
    bool validateJS(const std::string& input);
    
    // 设置编译选项
    void setMinify(bool minify);
    void setOptimize(bool optimize);
    
    // 添加 CSS 变量
    void addCSSVariable(const std::string& name, const std::string& value);
    
    // 添加 JavaScript 全局变量
    void addJSGlobalVariable(const std::string& name, const std::string& value);
    
    // 添加 JavaScript 全局函数
    void addJSGlobalFunction(const std::string& name, const std::string& code);
    
    // 清除所有变量和函数
    void clearAll();
    
private:
    std::unique_ptr<CSSCompiler> cssCompiler_;
    std::unique_ptr<JSCompiler> jsCompiler_;
};

} // namespace CHTL