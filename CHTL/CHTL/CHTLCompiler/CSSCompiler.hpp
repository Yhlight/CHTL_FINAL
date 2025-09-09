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
#include <memory>

#ifdef HAVE_LIBCSS
#include <libcss/libcss.h>
#endif

// 前向声明
namespace CHTL {
    class PropertyExpressionParser;
    class PropertyExpressionNode;
}

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
    
    // 处理属性表达式
    std::string processPropertyExpression(const std::string& value);
    std::string processArithmeticExpression(const std::string& value);
    std::string processPropertyReference(const std::string& value);
    std::string processConditionalExpression(const std::string& value);
    std::string processDynamicExpression(const std::string& value);
    
private:
    std::map<std::string, std::string> variables_;
    bool minify_;
    bool optimize_;
    
    // libcss 相关
#ifdef HAVE_LIBCSS
    css_allocator_func alloc_;
    css_reallocator_func realloc_;
    css_deallocator_func free_;
    void* user_;
    css_select_ctx* select_ctx_;
#endif
    
    // 处理 CSS 变量替换
    std::string processVariables(const std::string& input);
    
    // 压缩 CSS
    std::string minifyCSS(const std::string& input);
    
    // 优化 CSS
    std::string optimizeCSS(const std::string& input);
};

} // namespace CHTL