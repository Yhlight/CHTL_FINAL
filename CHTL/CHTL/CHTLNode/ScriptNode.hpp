#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 脚本节点
 * 
 * 表示CHTL中的script块，支持CHTL JS语法
 */
class ScriptNode : public BaseNode {
public:
    enum class ScriptType {
        LOCAL,      // 局部脚本
        GLOBAL      // 全局脚本
    };
    
    ScriptNode(ScriptType type, size_t line = 0, size_t column = 0);
    
    // 获取脚本类型
    ScriptType getScriptType() const { return scriptType_; }
    
    // 设置脚本类型
    void setScriptType(ScriptType type) { scriptType_ = type; }
    
    // 添加CHTL JS代码
    void addCHTLJSCode(const std::string& code);
    
    // 获取CHTL JS代码
    const std::vector<std::string>& getCHTLJSCode() const { return chtljsCode_; }
    
    // 添加JavaScript代码
    void addJavaScriptCode(const std::string& code);
    
    // 获取JavaScript代码
    const std::vector<std::string>& getJavaScriptCode() const { return javascriptCode_; }
    
    // 添加增强选择器
    void addEnhancedSelector(const std::string& selector);
    
    // 获取增强选择器
    const std::vector<std::string>& getEnhancedSelectors() const { return enhancedSelectors_; }
    
    // 添加CHTL JS函数
    void addCHTLJSFunction(const std::string& functionName, const std::string& functionCode);
    
    // 获取CHTL JS函数
    const std::map<std::string, std::string>& getCHTLJSFunctions() const { return chtljsFunctions_; }
    
    // 转换为HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    ScriptType scriptType_;
    std::vector<std::string> chtljsCode_;
    std::vector<std::string> javascriptCode_;
    std::vector<std::string> enhancedSelectors_;
    std::map<std::string, std::string> chtljsFunctions_;
};

} // namespace CHTL