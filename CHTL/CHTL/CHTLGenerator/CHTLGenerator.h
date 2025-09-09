#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace CHTL {

class CHTLGenerator {
private:
    std::string htmlOutput;
    std::string cssOutput;
    std::string jsOutput;
    
    // 生成选项
    bool prettyPrint;
    bool minify;
    bool includeComments;
    
    // 上下文管理
    std::vector<std::shared_ptr<BaseNode>> contextStack;
    std::map<std::string, std::string> variables;
    std::map<std::string, std::shared_ptr<BaseNode>> templates;
    std::map<std::string, std::shared_ptr<BaseNode>> customs;
    
    // 样式管理
    std::vector<std::string> globalStyles;
    std::vector<std::string> localStyles;
    
    // 脚本管理
    std::vector<std::string> globalScripts;
    std::vector<std::string> localScripts;
    
    // 辅助方法
    std::string generateElement(std::shared_ptr<BaseNode> node);
    std::string generateText(std::shared_ptr<BaseNode> node);
    std::string generateComment(std::shared_ptr<BaseNode> node);
    std::string generateTemplate(std::shared_ptr<BaseNode> node);
    std::string generateCustom(std::shared_ptr<BaseNode> node);
    std::string generateStyle(std::shared_ptr<BaseNode> node);
    std::string generateScript(std::shared_ptr<BaseNode> node);
    std::string generateOrigin(std::shared_ptr<BaseNode> node);
    std::string generateImport(std::shared_ptr<BaseNode> node);
    std::string generateNamespace(std::shared_ptr<BaseNode> node);
    std::string generateConfiguration(std::shared_ptr<BaseNode> node);
    
    // 样式生成
    std::string generateInlineStyle(std::shared_ptr<BaseNode> node);
    std::string generateClassStyle(std::shared_ptr<BaseNode> node);
    std::string generateIdStyle(std::shared_ptr<BaseNode> node);
    std::string generatePseudoStyle(std::shared_ptr<BaseNode> node);
    std::string generateConditionalStyle(std::shared_ptr<BaseNode> node);
    
    // 脚本生成
    std::string generateInlineScript(std::shared_ptr<BaseNode> node);
    std::string generateGlobalScript(std::shared_ptr<BaseNode> node);
    
    // 模板处理
    void processTemplate(std::shared_ptr<BaseNode> node);
    void processCustom(std::shared_ptr<BaseNode> node);
    std::string resolveTemplate(const std::string& templateName);
    std::string resolveCustom(const std::string& customName);
    
    // 变量处理
    void processVariables(std::shared_ptr<BaseNode> node);
    std::string resolveVariable(const std::string& varName);
    
    // 属性处理
    std::string processAttributes(std::shared_ptr<BaseNode> node);
    std::string processAttributeValue(const std::string& value);
    
    // 表达式处理
    std::string processExpression(const std::string& expression);
    std::string processConditionalExpression(const std::string& expression);
    std::string processLogicalExpression(const std::string& expression);
    
    // 格式化
    std::string formatHTML(const std::string& html);
    std::string formatCSS(const std::string& css);
    std::string formatJS(const std::string& js);
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    // 工具方法
    std::string escapeHTML(const std::string& str);
    std::string escapeCSS(const std::string& str);
    std::string escapeJS(const std::string& str);
    std::string indent(int level);
    bool isVoidElement(const std::string& tagName);
    
public:
    CHTLGenerator();
    CHTLGenerator(bool prettyPrint, bool minify, bool includeComments);
    ~CHTLGenerator() = default;
    
    // 主要生成方法
    std::string generate(std::shared_ptr<BaseNode> ast);
    std::string generateHTML(std::shared_ptr<BaseNode> ast);
    std::string generateCSS(std::shared_ptr<BaseNode> ast);
    std::string generateJS(std::shared_ptr<BaseNode> ast);
    
    // 输出获取
    const std::string& getHTMLOutput() const { return htmlOutput; }
    const std::string& getCSSOutput() const { return cssOutput; }
    const std::string& getJSOutput() const { return jsOutput; }
    
    // 选项设置
    void setPrettyPrint(bool pretty) { prettyPrint = pretty; }
    void setMinify(bool min) { minify = min; }
    void setIncludeComments(bool include) { includeComments = include; }
    
    // 上下文管理
    void pushContext(std::shared_ptr<BaseNode> node);
    void popContext();
    std::shared_ptr<BaseNode> getCurrentContext() const;
    
    // 模板管理
    void registerTemplate(const std::string& name, std::shared_ptr<BaseNode> template);
    std::shared_ptr<BaseNode> getTemplate(const std::string& name) const;
    void clearTemplates();
    
    // 自定义管理
    void registerCustom(const std::string& name, std::shared_ptr<BaseNode> custom);
    std::shared_ptr<BaseNode> getCustom(const std::string& name) const;
    void clearCustoms();
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    void clearVariables();
    
    // 重置
    void reset();
    void clearOutputs();
    
    // 调试
    void enableDebugMode(bool enable = true);
    std::string getDebugInfo() const;
    
    // 统计信息
    struct GenerateStats {
        size_t elementsGenerated;
        size_t stylesGenerated;
        size_t scriptsGenerated;
        size_t templatesProcessed;
        size_t customsProcessed;
        double generateTime;
    };
    
    GenerateStats getStats() const;
    void resetStats();
    
private:
    bool debugMode;
    GenerateStats stats;
};

} // namespace CHTL