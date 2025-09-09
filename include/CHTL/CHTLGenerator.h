#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * @brief HTML 生成器
 * 
 * 负责将 AST 转换为 HTML 代码
 */
class HTMLGenerator : public NodeVisitor {
public:
    explicit HTMLGenerator(std::shared_ptr<CHTLContext> context);
    ~HTMLGenerator() = default;

    // 生成 HTML
    std::string generate(std::shared_ptr<BaseNode> root);
    
    // NodeVisitor 接口实现
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_output;
    int m_indentLevel;
    
    // 辅助方法
    void indent();
    void newline();
    void append(const std::string& text);
    void appendIndented(const std::string& text);
    
    // HTML 生成
    std::string generateElement(ElementNode& node);
    std::string generateAttributes(const std::unordered_map<std::string, std::string>& attributes);
    std::string generateText(TextNode& node);
    std::string generateComment(CommentNode& node);
    
    // 样式生成
    std::string generateStyle(StyleNode& node);
    std::string generateGlobalStyle();
    
    // 脚本生成
    std::string generateScript(ScriptNode& node);
    std::string generateGlobalScript();
    
    // 模板处理
    void processTemplates();
    void processCustoms();
    void processImports();
    
    // 命名空间处理
    void processNamespaces();
    
    // 配置处理
    void processConfiguration();
};

/**
 * @brief CSS 生成器
 * 
 * 负责生成 CSS 代码
 */
class CSSGenerator : public NodeVisitor {
public:
    explicit CSSGenerator(std::shared_ptr<CHTLContext> context);
    ~CSSGenerator() = default;

    // 生成 CSS
    std::string generate(std::shared_ptr<BaseNode> root);
    
    // NodeVisitor 接口实现
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_output;
    
    // 样式处理
    void processStyleBlocks();
    void processTemplates();
    void processCustoms();
    
    // CSS 生成
    std::string generateCSSRule(const std::string& selector, 
                               const std::unordered_map<std::string, std::string>& properties);
    std::string generateCSSProperty(const std::string& name, const std::string& value);
};

/**
 * @brief JavaScript 生成器
 * 
 * 负责生成 JavaScript 代码
 */
class JavaScriptGenerator : public NodeVisitor {
public:
    explicit JavaScriptGenerator(std::shared_ptr<CHTLContext> context);
    ~JavaScriptGenerator() = default;

    // 生成 JavaScript
    std::string generate(std::shared_ptr<BaseNode> root);
    
    // NodeVisitor 接口实现
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(TemplateNode& node) override;
    void visit(CustomNode& node) override;
    void visit(StyleNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ImportNode& node) override;
    void visit(ConfigNode& node) override;
    void visit(NamespaceNode& node) override;
    void visit(OperatorNode& node) override;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_output;
    
    // 脚本处理
    void processScriptBlocks();
    void processCHTLJSCode();
    
    // JavaScript 生成
    std::string generateJavaScriptCode(const std::string& code);
    std::string generateCHTLJSCode(const std::string& code);
};

/**
 * @brief CHTL 代码生成器
 * 
 * 统一管理各种代码生成器
 */
class CHTLGenerator {
public:
    explicit CHTLGenerator(std::shared_ptr<CHTLContext> context);
    ~CHTLGenerator() = default;

    // 生成所有代码
    struct GeneratedCode {
        std::string html;
        std::string css;
        std::string javascript;
    };
    
    GeneratedCode generate(std::shared_ptr<BaseNode> root);
    
    // 单独生成
    std::string generateHTML(std::shared_ptr<BaseNode> root);
    std::string generateCSS(std::shared_ptr<BaseNode> root);
    std::string generateJavaScript(std::shared_ptr<BaseNode> root);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<HTMLGenerator> m_htmlGenerator;
    std::unique_ptr<CSSGenerator> m_cssGenerator;
    std::unique_ptr<JavaScriptGenerator> m_jsGenerator;
};

} // namespace CHTL