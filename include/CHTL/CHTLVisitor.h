#pragma once

#include "CHTLNode.h"

namespace CHTL {

// 访问者模式基类
class CHTLVisitor {
public:
    virtual ~CHTLVisitor() = default;
    
    // 基础节点访问
    virtual void visit(TextNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    
    // 模板节点访问
    virtual void visit(TemplateStyleNode& node) = 0;
    virtual void visit(TemplateElementNode& node) = 0;
    virtual void visit(TemplateVarNode& node) = 0;
    
    // 自定义节点访问
    virtual void visit(CustomStyleNode& node) = 0;
    virtual void visit(CustomElementNode& node) = 0;
    virtual void visit(CustomVarNode& node) = 0;
    
    // 其他节点访问
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(ConfigNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(OperatorNode& node) = 0;
};

// 访问者适配器（提供默认空实现）
class CHTLVisitorAdapter : public CHTLVisitor {
public:
    void visit(TextNode& node) override {}
    void visit(ElementNode& node) override {}
    void visit(CommentNode& node) override {}
    void visit(StyleNode& node) override {}
    void visit(ScriptNode& node) override {}
    void visit(TemplateStyleNode& node) override {}
    void visit(TemplateElementNode& node) override {}
    void visit(TemplateVarNode& node) override {}
    void visit(CustomStyleNode& node) override {}
    void visit(CustomElementNode& node) override {}
    void visit(CustomVarNode& node) override {}
    void visit(OriginNode& node) override {}
    void visit(ImportNode& node) override {}
    void visit(ConfigNode& node) override {}
    void visit(NamespaceNode& node) override {}
    void visit(OperatorNode& node) override {}
};

} // namespace CHTL