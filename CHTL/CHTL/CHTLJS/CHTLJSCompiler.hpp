#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "CHTLJSNode/CHTLJSBaseNode.hpp"
#include "CHTLJSNode/FileLoaderNode.hpp"
#include "CHTLJSNode/EnhancedSelectorNode.hpp"
#include "CHTLJSNode/ListenNode.hpp"
#include "CHTLJSNode/DelegateNode.hpp"
#include "CHTLJSNode/AnimateNode.hpp"
#include "CHTLJSNode/VirNode.hpp"
#include "CHTLJSNode/RouterNode.hpp"

namespace CHTL {

/**
 * @brief CHTL JS编译器
 * 
 * 编译CHTL JS扩展语法为JavaScript代码
 */
class CHTLJSCompiler {
public:
    // 构造函数
    CHTLJSCompiler();
    
    // 编译CHTL JS代码
    std::string compile(const std::string& chtljsCode);
    
    // 编译文件载入器
    std::string compileFileLoader(const FileLoaderNode& fileLoader);
    
    // 编译增强选择器
    std::string compileEnhancedSelector(const EnhancedSelectorNode& selector);
    
    // 编译事件监听器
    std::string compileListen(const ListenNode& listen);
    
    // 编译事件委托
    std::string compileDelegate(const DelegateNode& delegate);
    
    // 编译动画
    std::string compileAnimate(const AnimateNode& animate);
    
    // 编译虚对象
    std::string compileVir(const VirNode& vir);
    
    // 编译路由
    std::string compileRouter(const RouterNode& router);
    
    // 编译局部脚本
    std::string compileLocalScript(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& nodes);
    
    // 编译全局脚本
    std::string compileGlobalScript(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& nodes);
    
    // 设置编译选项
    void setCompileOption(const std::string& option, const std::string& value);
    
    // 获取编译选项
    std::string getCompileOption(const std::string& option) const;
    
    // 设置输出格式
    void setOutputFormat(const std::string& format);
    
    // 获取输出格式
    std::string getOutputFormat() const;
    
    // 添加自定义编译器
    void addCustomCompiler(const std::string& nodeType, 
                          std::function<std::string(const CHTLJSBaseNode&)> compiler);
    
    // 移除自定义编译器
    void removeCustomCompiler(const std::string& nodeType);
    
    // 获取所有编译选项
    std::map<std::string, std::string> getAllCompileOptions() const;
    
    // 清除所有编译选项
    void clearCompileOptions();
    
    // 验证CHTL JS代码
    bool validate(const std::string& chtljsCode);
    
    // 获取验证错误
    std::vector<std::string> getValidationErrors() const;
    
    // 优化编译结果
    std::string optimize(const std::string& compiledCode);
    
    // 设置优化级别
    void setOptimizationLevel(int level);
    
    // 获取优化级别
    int getOptimizationLevel() const;
    
private:
    std::map<std::string, std::string> compileOptions_;
    std::string outputFormat_;
    std::map<std::string, std::function<std::string(const CHTLJSBaseNode&)>> customCompilers_;
    std::vector<std::string> validationErrors_;
    int optimizationLevel_;
    
    // 初始化默认编译选项
    void initializeDefaultOptions();
    
    // 验证节点
    bool validateNode(const CHTLJSBaseNode& node);
    
    // 优化代码
    std::string optimizeCode(const std::string& code);
    
    // 压缩代码
    std::string minifyCode(const std::string& code);
    
    // 美化代码
    std::string beautifyCode(const std::string& code);
    
    // 添加源映射
    std::string addSourceMap(const std::string& code, const std::string& sourceMap);
    
    // 生成源映射
    std::string generateSourceMap(const std::string& originalCode, const std::string& compiledCode);
    
    // 处理错误
    void handleError(const std::string& error);
    
    // 处理警告
    void handleWarning(const std::string& warning);
    
    // 清理代码
    std::string cleanCode(const std::string& code);
    
    // 转义字符串
    std::string escapeString(const std::string& str);
    
    // 取消转义字符串
    std::string unescapeString(const std::string& str);
    
    // 验证JavaScript语法
    bool validateJavaScriptSyntax(const std::string& code);
    
    // 验证CSS语法
    bool validateCSSSyntax(const std::string& code);
    
    // 验证HTML语法
    bool validateHTMLSyntax(const std::string& code);
};

} // namespace CHTL