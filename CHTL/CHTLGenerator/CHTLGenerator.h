#ifndef CHTLGENERATOR_H
#define CHTLGENERATOR_H

#include "../CHTLParser/CHTLASTNode.h"
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace CHTL {

/**
 * CHTL代码生成器
 * 将AST转换为HTML/CSS代码
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator();

    // 代码生成
    std::string generate(const std::shared_ptr<ProgramNode>& program);
    std::string generate(const std::shared_ptr<CHTLASTNode>& node);
    
    // 样式处理
    std::string generateGlobalStyles() const;
    void addGlobalStyle(const std::string& style);
    
    // 配置管理
    void setConfiguration(const std::map<std::string, std::string>& config);
    std::string getConfiguration(const std::string& key) const;

private:
    std::map<std::string, std::string> configuration;
    std::vector<std::string> globalStyles;
    
    // 节点代码生成
    std::string generateProgram(const std::shared_ptr<ProgramNode>& node);
    std::string generateElement(const std::shared_ptr<ElementNode>& node);
    std::string generateText(const std::shared_ptr<TextNode>& node);
    std::string generateAttribute(const std::shared_ptr<AttributeNode>& node);
    std::string generateStyleBlock(const std::shared_ptr<StyleBlockNode>& node);
    std::string generateStyleRule(const std::shared_ptr<StyleRuleNode>& node);
    std::string generateStyleSelector(const std::shared_ptr<StyleSelectorNode>& node);
    std::string generateStyleProperty(const std::shared_ptr<StylePropertyNode>& node);
    std::string generateScriptBlock(const std::shared_ptr<ScriptBlockNode>& node);
    std::string generateTemplate(const std::shared_ptr<TemplateNode>& node);
    std::string generateCustom(const std::shared_ptr<CustomNode>& node);
    std::string generateOrigin(const std::shared_ptr<OriginNode>& node);
    std::string generateImport(const std::shared_ptr<ImportNode>& node);
    std::string generateConfiguration(const std::shared_ptr<ConfigurationNode>& node);
    std::string generateNamespace(const std::shared_ptr<NamespaceNode>& node);
    std::string generateInfo(const std::shared_ptr<InfoNode>& node);
    std::string generateExport(const std::shared_ptr<ExportNode>& node);
    std::string generateLiteral(const std::shared_ptr<LiteralNode>& node);
    std::string generateIdentifier(const std::shared_ptr<IdentifierNode>& node);
    std::string generateExpression(const std::shared_ptr<ExpressionNode>& node);
    std::string generateBinaryExpression(const std::shared_ptr<BinaryExpressionNode>& node);
    std::string generateConditionalExpression(const std::shared_ptr<ConditionalExpressionNode>& node);
    std::string generatePropertyReference(const std::shared_ptr<PropertyReferenceNode>& node);
    std::string generateFunctionCall(const std::shared_ptr<FunctionCallNode>& node);
    std::string generateUseStatement(const std::shared_ptr<UseStatementNode>& node);
    std::string generateExceptClause(const std::shared_ptr<ExceptClauseNode>& node);
    
    // 样式处理
    std::string processStyleBlock(const std::shared_ptr<StyleBlockNode>& node, const std::string& elementId = "");
    std::string processStyleRule(const std::shared_ptr<StyleRuleNode>& node, const std::string& elementId = "");
    std::string processStyleProperty(const std::shared_ptr<StylePropertyNode>& node);
    std::string processStyleValue(const std::shared_ptr<CHTLASTNode>& value);
    
    // 属性运算处理
    std::string processArithmeticExpression(const std::shared_ptr<BinaryExpressionNode>& node);
    std::string processPropertyExpression(const std::shared_ptr<PropertyReferenceNode>& node);
    std::string processConditionalExpression(const std::shared_ptr<ConditionalExpressionNode>& node);
    
    // 选择器处理
    std::string resolveSelector(const std::string& selector, const std::string& elementId = "");
    std::string generateUniqueClass(const std::string& baseName);
    std::string generateUniqueId(const std::string& baseName);
    
    // 变量处理
    std::string resolveVariable(const std::string& name);
    std::string processVariableReference(const std::string& name);
    
    // 模板处理
    std::string processTemplateReference(const std::string& name);
    std::string processTemplateStyle(const std::string& name);
    std::string processTemplateElement(const std::string& name);
    std::string processTemplateVar(const std::string& name);
    
    // 自定义处理
    std::string processCustomReference(const std::string& name);
    std::string processCustomStyle(const std::string& name);
    std::string processCustomElement(const std::string& name);
    std::string processCustomVar(const std::string& name);
    
    // 工具方法
    std::string escapeHtml(const std::string& str);
    std::string escapeCss(const std::string& str);
    std::string escapeJs(const std::string& str);
    std::string indent(int level);
    bool needsQuotes(const std::string& str);
    bool isArithmeticOperator(const std::string& op) const;
    
    // 上下文管理
    std::map<std::string, std::shared_ptr<CHTLASTNode>> templates;
    std::map<std::string, std::shared_ptr<CHTLASTNode>> customs;
    std::map<std::string, std::string> variables;
    std::set<std::string> currentNamespace;
    
    // 样式上下文
    std::string currentElementId;
    std::string currentElementClass;
    std::vector<std::string> styleStack;
    
    // 计数器
    int classCounter;
    int idCounter;
};

} // namespace CHTL

#endif // CHTLGENERATOR_H