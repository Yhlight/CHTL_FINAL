#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL代码生成器
 */
class CHTLGenerator {
public:
    CHTLGenerator(std::shared_ptr<CHTLContext> context);
    ~CHTLGenerator() = default;

    /**
     * @brief 生成HTML代码
     * @param root AST根节点
     * @return 生成的HTML代码
     */
    std::string generateHTML(std::shared_ptr<CHTLNode> root);
    
    /**
     * @brief 生成CSS代码
     * @param root AST根节点
     * @return 生成的CSS代码
     */
    std::string generateCSS(std::shared_ptr<CHTLNode> root);
    
    /**
     * @brief 生成JavaScript代码
     * @param root AST根节点
     * @return 生成的JavaScript代码
     */
    std::string generateJS(std::shared_ptr<CHTLNode> root);
    
    /**
     * @brief 生成完整输出
     * @param root AST根节点
     * @return 生成的完整HTML文档
     */
    std::string generateComplete(std::shared_ptr<CHTLNode> root);

    /**
     * @brief 设置输出选项
     * @param minify 是否压缩输出
     * @param includeComments 是否包含注释
     */
    void setOutputOptions(bool minify, bool includeComments);

private:
    std::shared_ptr<CHTLContext> m_context;
    bool m_minify;
    bool m_includeComments;
    
    // 生成状态
    std::string m_currentNamespace;
    std::unordered_map<std::string, std::string> m_generatedClasses;
    std::unordered_map<std::string, std::string> m_generatedIds;
    std::unordered_map<std::string, std::string> m_templateVariables;
    
    // HTML生成
    std::string generateElementHTML(std::shared_ptr<ElementNode> element);
    std::string generateTextHTML(std::shared_ptr<TextNode> text);
    std::string generateCommentHTML(std::shared_ptr<CHTLNode> comment);
    std::string generateAttributes(const AttributeMap& attributes);
    std::string generateAttributeValue(const AttributeValue& value);
    
    // CSS生成
    std::string generateStyleCSS(std::shared_ptr<StyleNode> style);
    std::string generateStyleRule(const std::string& selector, 
                                 const std::unordered_map<std::string, std::string>& properties);
    std::string generateStyleProperty(const std::string& property, const std::string& value);
    std::string generateConditionalCSS(const std::string& expression);
    
    // JavaScript生成
    std::string generateScriptJS(std::shared_ptr<ScriptNode> script);
    std::string generateCHTLJS(std::shared_ptr<CHTLNode> node);
    
    // 模板处理
    void processTemplates(std::shared_ptr<CHTLNode> root);
    void processTemplateInheritance(std::shared_ptr<TemplateNode> templateNode);
    void processTemplateVariables(std::shared_ptr<CHTLNode> node);
    
    // 自定义处理
    void processCustomSpecialization(std::shared_ptr<CHTLNode> customNode);
    void processCustomDeletion(std::shared_ptr<CHTLNode> customNode);
    void processCustomInsertion(std::shared_ptr<CHTLNode> customNode);
    
    // 导入处理
    void processImports(std::shared_ptr<CHTLNode> root);
    std::string processImportFile(const std::string& filePath, const std::string& type);
    
    // 命名空间处理
    void processNamespaces(std::shared_ptr<CHTLNode> root);
    std::string resolveNamespace(const std::string& name, const std::string& currentNamespace);
    
    // 样式处理
    void processStyleBlocks(std::shared_ptr<CHTLNode> root);
    void processLocalStyles(std::shared_ptr<StyleNode> styleNode);
    void processGlobalStyles(std::shared_ptr<StyleNode> styleNode);
    void processStyleSelectors(std::shared_ptr<StyleNode> styleNode);
    
    // 条件表达式处理
    std::string processConditionalExpression(const std::string& expression);
    std::string processLogicalExpression(const std::string& expression);
    std::string processComparisonExpression(const std::string& expression);
    std::string processPropertyReference(const std::string& reference);
    
    // 工具函数
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJS(const std::string& text);
    std::string generateClassName(const std::string& base);
    std::string generateId(const std::string& base);
    std::string indent(int level);
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    // 配置处理
    void processConfiguration(std::shared_ptr<CHTLNode> configNode);
    void processNameConfiguration(std::shared_ptr<CHTLNode> configNode);
    void updateKeywordMap(const std::unordered_map<std::string, std::string>& keywords);
};

} // namespace CHTL