#pragma once

#include "CHTLNode/BaseNode.h"
#include "CHTLContext.h"
#include <memory>
#include <string>

namespace CHTL {

/**
 * @brief CHTL代码生成器
 * 将抽象语法树转换为HTML代码
 */
class CHTLGenerator {
public:
    CHTLGenerator(std::shared_ptr<CHTLContext> context);
    ~CHTLGenerator();
    
    // 生成HTML代码
    std::string generateHTML(std::shared_ptr<BaseNode> root);
    
    // 生成CSS代码
    std::string generateCSS(std::shared_ptr<BaseNode> root);
    
    // 生成JavaScript代码
    std::string generateJavaScript(std::shared_ptr<BaseNode> root);
    
    // 生成完整文档
    std::string generateDocument(std::shared_ptr<BaseNode> root);
    
    // 设置输出选项
    void setPrettyPrint(bool enabled);
    void setMinify(bool enabled);
    void setIncludeComments(bool enabled);
    
    // 获取生成统计信息
    size_t getGeneratedLines() const;
    size_t getGeneratedSize() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    bool m_prettyPrint;
    bool m_minify;
    bool m_includeComments;
    size_t m_generatedLines;
    size_t m_generatedSize;
    
    // HTML生成
    std::string generateElement(std::shared_ptr<BaseNode> node, int indent = 0);
    std::string generateText(std::shared_ptr<BaseNode> node);
    std::string generateComment(std::shared_ptr<BaseNode> node);
    std::string generateAttributes(std::shared_ptr<BaseNode> node);
    std::string generateStyleAttributes(std::shared_ptr<BaseNode> node);
    
    // CSS生成
    std::string generateStyleBlock(std::shared_ptr<BaseNode> node);
    std::string generateStyleRule(std::shared_ptr<BaseNode> node);
    std::string generateCSSSelector(std::shared_ptr<BaseNode> node);
    std::string generateStyleProperties(std::shared_ptr<BaseNode> node);
    std::string generateConditionalProperties(std::shared_ptr<BaseNode> node);
    
    // JavaScript生成
    std::string generateScriptBlock(std::shared_ptr<BaseNode> node);
    std::string generateCHTLJSFunction(std::shared_ptr<BaseNode> node);
    std::string generateJSSelector(std::shared_ptr<BaseNode> node);
    std::string generateListener(std::shared_ptr<BaseNode> node);
    std::string generateDelegate(std::shared_ptr<BaseNode> node);
    std::string generateAnimation(std::shared_ptr<BaseNode> node);
    std::string generateVirtualObject(std::shared_ptr<BaseNode> node);
    std::string generateRoute(std::shared_ptr<BaseNode> node);
    
    // 模板生成
    std::string generateTemplate(std::shared_ptr<BaseNode> node);
    std::string generateTemplateInheritance(std::shared_ptr<BaseNode> node);
    std::string generateTemplateVariables(std::shared_ptr<BaseNode> node);
    
    // 自定义生成
    std::string generateCustom(std::shared_ptr<BaseNode> node);
    std::string generateCustomSpecializations(std::shared_ptr<BaseNode> node);
    std::string generateCustomOperations(std::shared_ptr<BaseNode> node);
    
    // 原始嵌入生成
    std::string generateOrigin(std::shared_ptr<BaseNode> node);
    
    // 导入生成
    std::string generateImport(std::shared_ptr<BaseNode> node);
    
    // 配置生成
    std::string generateConfig(std::shared_ptr<BaseNode> node);
    
    // 命名空间生成
    std::string generateNamespace(std::shared_ptr<BaseNode> node);
    
    // 操作符生成
    std::string generateOperator(std::shared_ptr<BaseNode> node);
    
    // 辅助方法
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJavaScript(const std::string& text);
    std::string indent(int level);
    std::string formatAttribute(const std::string& key, const std::string& value);
    std::string formatStyleProperty(const std::string& property, const std::string& value);
    
    // 遍历方法
    void generateCSSFromNode(std::shared_ptr<BaseNode> node, std::stringstream& ss, int indent);
    void generateJavaScriptFromNode(std::shared_ptr<BaseNode> node, std::stringstream& ss, int indent);
    
    // 合并方法
    std::string mergeHTMLAndCSS(const std::string& html, const std::string& css);
    std::string mergeHTMLAndJavaScript(const std::string& html, const std::string& js);
    
    // 模板处理
    std::string processTemplate(std::shared_ptr<BaseNode> node);
    std::string processTemplateVariables(std::shared_ptr<BaseNode> node);
    std::string processTemplateInheritance(std::shared_ptr<BaseNode> node);
    
    // 自定义处理
    std::string processCustom(std::shared_ptr<BaseNode> node);
    std::string processCustomSpecializations(std::shared_ptr<BaseNode> node);
    std::string processCustomOperations(std::shared_ptr<BaseNode> node);
    
    // 样式处理
    std::string processStyle(std::shared_ptr<BaseNode> node);
    std::string processStyleSelectors(std::shared_ptr<BaseNode> node);
    std::string processStyleProperties(std::shared_ptr<BaseNode> node);
    std::string processConditionalProperties(std::shared_ptr<BaseNode> node);
    
    // 脚本处理
    std::string processScript(std::shared_ptr<BaseNode> node);
    std::string processCHTLJS(std::shared_ptr<BaseNode> node);
    std::string processSelectors(std::shared_ptr<BaseNode> node);
    std::string processListeners(std::shared_ptr<BaseNode> node);
    std::string processDelegates(std::shared_ptr<BaseNode> node);
    std::string processAnimations(std::shared_ptr<BaseNode> node);
    std::string processVirtualObjects(std::shared_ptr<BaseNode> node);
    std::string processRoutes(std::shared_ptr<BaseNode> node);
};

} // namespace CHTL