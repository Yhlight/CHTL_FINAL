#pragma once

#include "ASTNode.h"
#include "CHTLContext.h"
#include "ASTVisitor.h"
#include <string>
#include <fstream>
#include <memory>

namespace CHTL {

/**
 * CHTL代码生成器
 * 负责将AST转换为HTML、CSS、JS代码
 */
class CHTLGenerator : public ASTVisitor {
public:
    CHTLGenerator(std::shared_ptr<ASTNode> ast, CHTLContext& context);
    ~CHTLGenerator() = default;
    
    // 代码生成主函数
    void generate(const std::string& outputFile);
    void generateHTML(const std::string& outputFile);
    void generateCSS(const std::string& outputFile);
    void generateJS(const std::string& outputFile);
    
    // 获取生成的代码
    const std::string& getHTML() const { return htmlCode_; }
    const std::string& getCSS() const { return cssCode_; }
    const std::string& getJS() const { return jsCode_; }
    
    // AST访问者接口实现
    void visitElement(ElementNode& node) override;
    void visitText(TextNode& node) override;
    void visitStyle(StyleNode& node) override;
    void visitScript(ScriptNode& node) override;
    void visitTemplate(TemplateNode& node) override;
    void visitCustom(CustomNode& node) override;
    void visitOrigin(OriginNode& node) override;
    void visitImport(ImportNode& node) override;
    void visitNamespace(NamespaceNode& node) override;
    void visitConfiguration(ConfigurationNode& node) override;
    void visitConstraint(ConstraintNode& node) override;
    void visitUse(UseNode& node) override;
    void visitStyleRule(StyleRuleNode& node) override;
    void visitExpression(ExpressionNode& node) override;
    void visitBinaryOp(BinaryOpNode& node) override;
    void visitConditional(ConditionalNode& node) override;
    void visitReference(ReferenceNode& node) override;
    void visitSelector(SelectorNode& node) override;
    void visitCHTLJSFunction(CHTLJSFunctionNode& node) override;
    void visitCHTLJSVir(CHTLJSVirNode& node) override;
    
private:
    // 输入
    std::shared_ptr<ASTNode> ast_;
    CHTLContext& context_;
    
    // 输出代码
    std::string htmlCode_;
    std::string cssCode_;
    std::string jsCode_;
    
    // 生成状态
    bool inStyleBlock_;
    bool inScriptBlock_;
    int indentLevel_;
    
    // 核心生成方法
    void generateNode(std::shared_ptr<ASTNode> node);
    void generateElement(std::shared_ptr<ElementNode> element);
    void generateText(std::shared_ptr<TextNode> text);
    void generateStyle(std::shared_ptr<StyleNode> style);
    void generateScript(std::shared_ptr<ScriptNode> script);
    void generateTemplate(std::shared_ptr<TemplateNode> template_);
    void generateCustom(std::shared_ptr<CustomNode> custom);
    void generateOrigin(std::shared_ptr<OriginNode> origin);
    void generateImport(std::shared_ptr<ImportNode> import_);
    void generateNamespace(std::shared_ptr<NamespaceNode> namespace_);
    void generateConfiguration(std::shared_ptr<ConfigurationNode> config);
    void generateConstraint(std::shared_ptr<ConstraintNode> constraint);
    void generateUse(std::shared_ptr<UseNode> use);
    
    // 辅助方法
    std::string getIndent() const;
    void addIndent();
    void removeIndent();
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJS(const std::string& text);
    std::string generateAttributes(const std::unordered_map<std::string, AttributeValue>& attributes);
    std::string generateStyleProperties(const std::unordered_map<std::string, std::string>& properties);
    std::string processTemplate(const std::string& template_, const std::string& name, const std::string& type);
    std::string processCustom(const std::string& custom, const std::string& name, const std::string& type);
    std::string processExpression(const std::string& expression);
    
    // 新增的辅助方法
    std::string generateSelector(std::shared_ptr<ASTNode> selector);
    std::string generateExpression(std::shared_ptr<ASTNode> expr);
    std::string getOperatorString(TokenType op);
    
    // 文件操作
    void writeToFile(const std::string& filename, const std::string& content);
    std::string readFile(const std::string& filename);
    
    // 错误处理
    void reportError(const std::string& message);
};

} // namespace CHTL