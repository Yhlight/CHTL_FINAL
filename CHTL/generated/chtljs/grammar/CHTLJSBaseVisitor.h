
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJSVisitor.h"


namespace chtl.antlr {

/**
 * This class provides an empty implementation of CHTLJSVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CHTLJSBaseVisitor : public CHTLJSVisitor {
public:

  virtual std::any visitProgram(CHTLJSParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(CHTLJSParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionStatement(CHTLJSParser::ExpressionStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclarationStatement(CHTLJSParser::DeclarationStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitControlStatement(CHTLJSParser::ControlStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChthljsStatement(CHTLJSParser::ChthljsStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(CHTLJSParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentExpression(CHTLJSParser::AssignmentExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditionalExpression(CHTLJSParser::ConditionalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExpression(CHTLJSParser::EqualityExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExpression(CHTLJSParser::RelationalExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExpression(CHTLJSParser::AdditiveExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExpression(CHTLJSParser::UnaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostfixExpression(CHTLJSParser::PostfixExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryExpression(CHTLJSParser::PrimaryExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiteral(CHTLJSParser::LiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(CHTLJSParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChthljsSelector(CHTLJSParser::ChthljsSelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelectorContent(CHTLJSParser::SelectorContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelector(CHTLJSParser::SelectorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty(CHTLJSParser::PropertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIndex(CHTLJSParser::IndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionExpression(CHTLJSParser::FunctionExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameterList(CHTLJSParser::ParameterListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitObjectExpression(CHTLJSParser::ObjectExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyAssignment(CHTLJSParser::PropertyAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyName(CHTLJSParser::PropertyNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayExpression(CHTLJSParser::ArrayExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclaration(CHTLJSParser::VariableDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariableDeclarator(CHTLJSParser::VariableDeclaratorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStatement(CHTLJSParser::IfStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitForStatement(CHTLJSParser::ForStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStatement(CHTLJSParser::WhileStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitchStatement(CHTLJSParser::SwitchStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCaseClause(CHTLJSParser::CaseClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefaultClause(CHTLJSParser::DefaultClauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFileloaderStatement(CHTLJSParser::FileloaderStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitListenStatement(CHTLJSParser::ListenStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelegateStatement(CHTLJSParser::DelegateStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnimateStatement(CHTLJSParser::AnimateStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVirStatement(CHTLJSParser::VirStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRouterStatement(CHTLJSParser::RouterStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUtilStatement(CHTLJSParser::UtilStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(CHTLJSParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArgumentList(CHTLJSParser::ArgumentListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignmentOperator(CHTLJSParser::AssignmentOperatorContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace chtl.antlr
