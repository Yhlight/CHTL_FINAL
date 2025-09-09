/**
 * @file JSCompiler.hpp
 * @brief JavaScript 编译器接口
 * 
 * 根据 CHTL.md 文档实现
 * 提供 JavaScript 编译和执行功能
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace CHTL {

/**
 * @brief JavaScript 编译器
 * 
 * 根据 CHTL.md 文档实现
 * 支持 JavaScript 编译、执行和优化
 */
class JSCompiler {
public:
    JSCompiler();
    ~JSCompiler();
    
    // 编译 JavaScript 代码
    bool compile(const std::string& input, std::string& output);
    
    // 执行 JavaScript 代码
    bool execute(const std::string& code, std::string& result);
    
    // 验证 JavaScript 语法
    bool validate(const std::string& input);
    
    // 优化 JavaScript 代码
    bool optimize(const std::string& input, std::string& output);
    
    // 添加全局变量
    void addGlobalVariable(const std::string& name, const std::string& value);
    
    // 添加全局函数
    void addGlobalFunction(const std::string& name, const std::string& code);
    
    // 设置编译选项
    void setMinify(bool minify) { minify_ = minify; }
    void setOptimize(bool optimize) { optimize_ = optimize; }
    
    // 获取执行结果
    std::string getLastResult() const { return lastResult_; }
    
    // 清除全局变量和函数
    void clearGlobals();
    
private:
    std::map<std::string, std::string> globalVariables_;
    std::map<std::string, std::string> globalFunctions_;
    std::string lastResult_;
    bool minify_;
    bool optimize_;
    
    // 处理全局变量和函数注入
    std::string injectGlobals(const std::string& code);
    
    // 压缩 JavaScript
    std::string minifyJS(const std::string& input);
    
    // 优化 JavaScript
    std::string optimizeJS(const std::string& input);
    
    // 简单的 JavaScript 语法验证
    bool validateSyntax(const std::string& input);
};

} // namespace CHTL