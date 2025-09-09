
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJSParser.h"


namespace chtl.antlr {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by CHTLJSParser.
 */
class  CHTLJSVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CHTLJSParser.
   */
    virtual std::any visitProgram(CHTLJSParser::ProgramContext *context) = 0;

    virtual std::any visitStatement(CHTLJSParser::StatementContext *context) = 0;

    virtual std::any visitExpressionStatement(CHTLJSParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitDeclarationStatement(CHTLJSParser::DeclarationStatementContext *context) = 0;

    virtual std::any visitControlStatement(CHTLJSParser::ControlStatementContext *context) = 0;

    virtual std::any visitChthljsStatement(CHTLJSParser::ChthljsStatementContext *context) = 0;

    virtual std::any visitExpression(CHTLJSParser::ExpressionContext *context) = 0;

    virtual std::any visitAssignmentExpression(CHTLJSParser::AssignmentExpressionContext *context) = 0;

    virtual std::any visitConditionalExpression(CHTLJSParser::ConditionalExpressionContext *context) = 0;

    virtual std::any visitLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext *context) = 0;

    virtual std::any visitLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext *context) = 0;

    virtual std::any visitEqualityExpression(CHTLJSParser::EqualityExpressionContext *context) = 0;

    virtual std::any visitRelationalExpression(CHTLJSParser::RelationalExpressionContext *context) = 0;

    virtual std::any visitAdditiveExpression(CHTLJSParser::AdditiveExpressionContext *context) = 0;

    virtual std::any visitMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext *context) = 0;

    virtual std::any visitUnaryExpression(CHTLJSParser::UnaryExpressionContext *context) = 0;

    virtual std::any visitPostfixExpression(CHTLJSParser::PostfixExpressionContext *context) = 0;

    virtual std::any visitPrimaryExpression(CHTLJSParser::PrimaryExpressionContext *context) = 0;

    virtual std::any visitLiteral(CHTLJSParser::LiteralContext *context) = 0;

    virtual std::any visitIdentifier(CHTLJSParser::IdentifierContext *context) = 0;

    virtual std::any visitChthljsSelector(CHTLJSParser::ChthljsSelectorContext *context) = 0;

    virtual std::any visitSelectorContent(CHTLJSParser::SelectorContentContext *context) = 0;

    virtual std::any visitSelector(CHTLJSParser::SelectorContext *context) = 0;

    virtual std::any visitProperty(CHTLJSParser::PropertyContext *context) = 0;

    virtual std::any visitIndex(CHTLJSParser::IndexContext *context) = 0;

    virtual std::any visitFunctionExpression(CHTLJSParser::FunctionExpressionContext *context) = 0;

    virtual std::any visitParameterList(CHTLJSParser::ParameterListContext *context) = 0;

    virtual std::any visitObjectExpression(CHTLJSParser::ObjectExpressionContext *context) = 0;

    virtual std::any visitPropertyAssignment(CHTLJSParser::PropertyAssignmentContext *context) = 0;

    virtual std::any visitPropertyName(CHTLJSParser::PropertyNameContext *context) = 0;

    virtual std::any visitArrayExpression(CHTLJSParser::ArrayExpressionContext *context) = 0;

    virtual std::any visitParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext *context) = 0;

    virtual std::any visitVariableDeclaration(CHTLJSParser::VariableDeclarationContext *context) = 0;

    virtual std::any visitVariableDeclarator(CHTLJSParser::VariableDeclaratorContext *context) = 0;

    virtual std::any visitFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext *context) = 0;

    virtual std::any visitIfStatement(CHTLJSParser::IfStatementContext *context) = 0;

    virtual std::any visitForStatement(CHTLJSParser::ForStatementContext *context) = 0;

    virtual std::any visitWhileStatement(CHTLJSParser::WhileStatementContext *context) = 0;

    virtual std::any visitSwitchStatement(CHTLJSParser::SwitchStatementContext *context) = 0;

    virtual std::any visitCaseClause(CHTLJSParser::CaseClauseContext *context) = 0;

    virtual std::any visitDefaultClause(CHTLJSParser::DefaultClauseContext *context) = 0;

    virtual std::any visitFileloaderStatement(CHTLJSParser::FileloaderStatementContext *context) = 0;

    virtual std::any visitListenStatement(CHTLJSParser::ListenStatementContext *context) = 0;

    virtual std::any visitDelegateStatement(CHTLJSParser::DelegateStatementContext *context) = 0;

    virtual std::any visitAnimateStatement(CHTLJSParser::AnimateStatementContext *context) = 0;

    virtual std::any visitVirStatement(CHTLJSParser::VirStatementContext *context) = 0;

    virtual std::any visitRouterStatement(CHTLJSParser::RouterStatementContext *context) = 0;

    virtual std::any visitUtilStatement(CHTLJSParser::UtilStatementContext *context) = 0;

    virtual std::any visitBlock(CHTLJSParser::BlockContext *context) = 0;

    virtual std::any visitArgumentList(CHTLJSParser::ArgumentListContext *context) = 0;

    virtual std::any visitAssignmentOperator(CHTLJSParser::AssignmentOperatorContext *context) = 0;


};

}  // namespace chtl.antlr
