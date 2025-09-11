#pragma once

#include "ASTNode.h"

namespace CHTL {

/**
 * AST访问者接口
 * 实现访问者模式，用于遍历和处理AST节点
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // 基础节点访问
    virtual void visitElement(ElementNode& node) = 0;
    virtual void visitText(TextNode& node) = 0;
    virtual void visitStyle(StyleNode& node) = 0;
    virtual void visitScript(ScriptNode& node) = 0;
    
    // 模板节点访问
    virtual void visitTemplate(TemplateNode& node) = 0;
    
    // 自定义节点访问
    virtual void visitCustom(CustomNode& node) = 0;
    
    // 原始嵌入节点访问
    virtual void visitOrigin(OriginNode& node) = 0;
    
    // 导入和命名空间节点访问
    virtual void visitImport(ImportNode& node) = 0;
    virtual void visitNamespace(NamespaceNode& node) = 0;
    
    // 配置节点访问
    virtual void visitConfiguration(ConfigurationNode& node) = 0;
    
    // 约束节点访问
    virtual void visitConstraint(ConstraintNode& node) = 0;
    
    // 使用节点访问
    virtual void visitUse(UseNode& node) = 0;
    
    // 样式规则节点访问
    virtual void visitStyleRule(StyleRuleNode& node) = 0;
    
    // 表达式节点访问
    virtual void visitExpression(ExpressionNode& node) = 0;
    virtual void visitBinaryOp(BinaryOpNode& node) = 0;
    virtual void visitConditional(ConditionalNode& node) = 0;
    virtual void visitReference(ReferenceNode& node) = 0;
    
    // 选择器节点访问
    virtual void visitSelector(SelectorNode& node) = 0;
    
    // CHTL JS节点访问
    virtual void visitCHTLJSFunction(CHTLJSFunctionNode& node) = 0;
    virtual void visitCHTLJSVir(CHTLJSVirNode& node) = 0;
    
    // 通用访问方法
    virtual void visitNode(ASTNode& node);
};

} // namespace CHTL