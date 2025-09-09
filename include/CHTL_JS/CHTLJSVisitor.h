#pragma once

#include "CHTLJSNode.h"

namespace CHTL_JS {

// 访问者模式基类
class CHTLJSVisitor {
public:
    virtual ~CHTLJSVisitor() = default;
    
    // 基础节点访问
    virtual void visit(IdentifierNode& node) = 0;
    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(ExpressionNode& node) = 0;
    virtual void visit(StatementNode& node) = 0;
    virtual void visit(BlockNode& node) = 0;
    
    // 函数节点访问
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(FunctionCallNode& node) = 0;
    virtual void visit(ArrowFunctionNode& node) = 0;
    
    // 对象节点访问
    virtual void visit(ObjectNode& node) = 0;
    virtual void visit(ObjectPropertyNode& node) = 0;
    virtual void visit(ArrayNode& node) = 0;
    virtual void visit(ArrayElementNode& node) = 0;
    
    // 控制流节点访问
    virtual void visit(IfStatementNode& node) = 0;
    virtual void visit(ForStatementNode& node) = 0;
    virtual void visit(WhileStatementNode& node) = 0;
    virtual void visit(SwitchStatementNode& node) = 0;
    virtual void visit(CaseStatementNode& node) = 0;
    virtual void visit(BreakStatementNode& node) = 0;
    virtual void visit(ContinueStatementNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    
    // 变量节点访问
    virtual void visit(VariableDeclarationNode& node) = 0;
    virtual void visit(VariableAssignmentNode& node) = 0;
    
    // 操作符节点访问
    virtual void visit(OperatorNode& node) = 0;
    
    // CHTL JS 特定节点访问
    virtual void visit(VirtualObjectNode& node) = 0;
    virtual void visit(ListenNode& node) = 0;
    virtual void visit(DelegateNode& node) = 0;
    virtual void visit(AnimateNode& node) = 0;
    virtual void visit(RouterNode& node) = 0;
    virtual void visit(FileLoaderNode& node) = 0;
    virtual void visit(UtilNode& node) = 0;
    virtual void visit(PrintNode& node) = 0;
    virtual void visit(INeverAwayNode& node) = 0;
};

// 访问者适配器（提供默认空实现）
class CHTLJSVisitorAdapter : public CHTLJSVisitor {
public:
    void visit(IdentifierNode& node) override {}
    void visit(LiteralNode& node) override {}
    void visit(ExpressionNode& node) override {}
    void visit(StatementNode& node) override {}
    void visit(BlockNode& node) override {}
    void visit(FunctionNode& node) override {}
    void visit(FunctionCallNode& node) override {}
    void visit(ArrowFunctionNode& node) override {}
    void visit(ObjectNode& node) override {}
    void visit(ObjectPropertyNode& node) override {}
    void visit(ArrayNode& node) override {}
    void visit(ArrayElementNode& node) override {}
    void visit(IfStatementNode& node) override {}
    void visit(ForStatementNode& node) override {}
    void visit(WhileStatementNode& node) override {}
    void visit(SwitchStatementNode& node) override {}
    void visit(CaseStatementNode& node) override {}
    void visit(BreakStatementNode& node) override {}
    void visit(ContinueStatementNode& node) override {}
    void visit(ReturnStatementNode& node) override {}
    void visit(VariableDeclarationNode& node) override {}
    void visit(VariableAssignmentNode& node) override {}
    void visit(OperatorNode& node) override {}
    void visit(VirtualObjectNode& node) override {}
    void visit(ListenNode& node) override {}
    void visit(DelegateNode& node) override {}
    void visit(AnimateNode& node) override {}
    void visit(RouterNode& node) override {}
    void visit(FileLoaderNode& node) override {}
    void visit(UtilNode& node) override {}
    void visit(PrintNode& node) override {}
    void visit(INeverAwayNode& node) override {}
};

} // namespace CHTL_JS