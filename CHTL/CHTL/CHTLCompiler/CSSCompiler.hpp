/**
 * @file CSSCompiler.hpp
 * @brief CSS 编译器接口
 * 
 * 根据 CHTL.md 文档实现
 * 提供 CSS 编译和优化功能
 */

#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

/**
 * @brief CSS 编译器
 * 
 * 根据 CHTL.md 文档实现
 * 支持 CSS 编译、优化和验证
 */
class CSSCompiler {
public:
    CSSCompiler();
    ~CSSCompiler();
    
    // 编译 CSS 代码
    bool compile(const std::string& input, std::string& output);
    
    // 优化 CSS 代码
    bool optimize(const std::string& input, std::string& output);
    
    // 验证 CSS 语法
    bool validate(const std::string& input);
    
    // 添加 CSS 变量
    void addVariable(const std::string& name, const std::string& value);
    
    // 获取 CSS 变量
    std::string getVariable(const std::string& name) const;
    
    // 清除所有变量
    void clearVariables();
    
    // 设置编译选项
    void setMinify(bool minify) { minify_ = minify; }
    void setOptimize(bool optimize) { optimize_ = optimize; }
    
private:
    std::map<std::string, std::string> variables_;
    bool minify_;
    bool optimize_;
    
    // 处理 CSS 变量替换
    std::string processVariables(const std::string& input);
    
    // 压缩 CSS
    std::string minifyCSS(const std::string& input);
    
    // 优化 CSS
    std::string optimizeCSS(const std::string& input);
};

} // namespace CHTL