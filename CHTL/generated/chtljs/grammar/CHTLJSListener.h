
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJSParser.h"


namespace chtl.antlr {

/**
 * This interface defines an abstract listener for a parse tree produced by CHTLJSParser.
 */
class  CHTLJSListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(CHTLJSParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(CHTLJSParser::ProgramContext *ctx) = 0;

  virtual void enterStatement(CHTLJSParser::StatementContext *ctx) = 0;
  virtual void exitStatement(CHTLJSParser::StatementContext *ctx) = 0;

  virtual void enterExpressionStatement(CHTLJSParser::ExpressionStatementContext *ctx) = 0;
  virtual void exitExpressionStatement(CHTLJSParser::ExpressionStatementContext *ctx) = 0;

  virtual void enterDeclarationStatement(CHTLJSParser::DeclarationStatementContext *ctx) = 0;
  virtual void exitDeclarationStatement(CHTLJSParser::DeclarationStatementContext *ctx) = 0;

  virtual void enterControlStatement(CHTLJSParser::ControlStatementContext *ctx) = 0;
  virtual void exitControlStatement(CHTLJSParser::ControlStatementContext *ctx) = 0;

  virtual void enterChthljsStatement(CHTLJSParser::ChthljsStatementContext *ctx) = 0;
  virtual void exitChthljsStatement(CHTLJSParser::ChthljsStatementContext *ctx) = 0;

  virtual void enterExpression(CHTLJSParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(CHTLJSParser::ExpressionContext *ctx) = 0;

  virtual void enterAssignmentExpression(CHTLJSParser::AssignmentExpressionContext *ctx) = 0;
  virtual void exitAssignmentExpression(CHTLJSParser::AssignmentExpressionContext *ctx) = 0;

  virtual void enterConditionalExpression(CHTLJSParser::ConditionalExpressionContext *ctx) = 0;
  virtual void exitConditionalExpression(CHTLJSParser::ConditionalExpressionContext *ctx) = 0;

  virtual void enterLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext *ctx) = 0;
  virtual void exitLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext *ctx) = 0;

  virtual void enterLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext *ctx) = 0;
  virtual void exitLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext *ctx) = 0;

  virtual void enterEqualityExpression(CHTLJSParser::EqualityExpressionContext *ctx) = 0;
  virtual void exitEqualityExpression(CHTLJSParser::EqualityExpressionContext *ctx) = 0;

  virtual void enterRelationalExpression(CHTLJSParser::RelationalExpressionContext *ctx) = 0;
  virtual void exitRelationalExpression(CHTLJSParser::RelationalExpressionContext *ctx) = 0;

  virtual void enterAdditiveExpression(CHTLJSParser::AdditiveExpressionContext *ctx) = 0;
  virtual void exitAdditiveExpression(CHTLJSParser::AdditiveExpressionContext *ctx) = 0;

  virtual void enterMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext *ctx) = 0;
  virtual void exitMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext *ctx) = 0;

  virtual void enterUnaryExpression(CHTLJSParser::UnaryExpressionContext *ctx) = 0;
  virtual void exitUnaryExpression(CHTLJSParser::UnaryExpressionContext *ctx) = 0;

  virtual void enterPostfixExpression(CHTLJSParser::PostfixExpressionContext *ctx) = 0;
  virtual void exitPostfixExpression(CHTLJSParser::PostfixExpressionContext *ctx) = 0;

  virtual void enterPrimaryExpression(CHTLJSParser::PrimaryExpressionContext *ctx) = 0;
  virtual void exitPrimaryExpression(CHTLJSParser::PrimaryExpressionContext *ctx) = 0;

  virtual void enterLiteral(CHTLJSParser::LiteralContext *ctx) = 0;
  virtual void exitLiteral(CHTLJSParser::LiteralContext *ctx) = 0;

  virtual void enterIdentifier(CHTLJSParser::IdentifierContext *ctx) = 0;
  virtual void exitIdentifier(CHTLJSParser::IdentifierContext *ctx) = 0;

  virtual void enterChthljsSelector(CHTLJSParser::ChthljsSelectorContext *ctx) = 0;
  virtual void exitChthljsSelector(CHTLJSParser::ChthljsSelectorContext *ctx) = 0;

  virtual void enterSelectorContent(CHTLJSParser::SelectorContentContext *ctx) = 0;
  virtual void exitSelectorContent(CHTLJSParser::SelectorContentContext *ctx) = 0;

  virtual void enterSelector(CHTLJSParser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CHTLJSParser::SelectorContext *ctx) = 0;

  virtual void enterProperty(CHTLJSParser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CHTLJSParser::PropertyContext *ctx) = 0;

  virtual void enterIndex(CHTLJSParser::IndexContext *ctx) = 0;
  virtual void exitIndex(CHTLJSParser::IndexContext *ctx) = 0;

  virtual void enterFunctionExpression(CHTLJSParser::FunctionExpressionContext *ctx) = 0;
  virtual void exitFunctionExpression(CHTLJSParser::FunctionExpressionContext *ctx) = 0;

  virtual void enterParameterList(CHTLJSParser::ParameterListContext *ctx) = 0;
  virtual void exitParameterList(CHTLJSParser::ParameterListContext *ctx) = 0;

  virtual void enterObjectExpression(CHTLJSParser::ObjectExpressionContext *ctx) = 0;
  virtual void exitObjectExpression(CHTLJSParser::ObjectExpressionContext *ctx) = 0;

  virtual void enterPropertyAssignment(CHTLJSParser::PropertyAssignmentContext *ctx) = 0;
  virtual void exitPropertyAssignment(CHTLJSParser::PropertyAssignmentContext *ctx) = 0;

  virtual void enterPropertyName(CHTLJSParser::PropertyNameContext *ctx) = 0;
  virtual void exitPropertyName(CHTLJSParser::PropertyNameContext *ctx) = 0;

  virtual void enterArrayExpression(CHTLJSParser::ArrayExpressionContext *ctx) = 0;
  virtual void exitArrayExpression(CHTLJSParser::ArrayExpressionContext *ctx) = 0;

  virtual void enterParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext *ctx) = 0;
  virtual void exitParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext *ctx) = 0;

  virtual void enterVariableDeclaration(CHTLJSParser::VariableDeclarationContext *ctx) = 0;
  virtual void exitVariableDeclaration(CHTLJSParser::VariableDeclarationContext *ctx) = 0;

  virtual void enterVariableDeclarator(CHTLJSParser::VariableDeclaratorContext *ctx) = 0;
  virtual void exitVariableDeclarator(CHTLJSParser::VariableDeclaratorContext *ctx) = 0;

  virtual void enterFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext *ctx) = 0;
  virtual void exitFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext *ctx) = 0;

  virtual void enterIfStatement(CHTLJSParser::IfStatementContext *ctx) = 0;
  virtual void exitIfStatement(CHTLJSParser::IfStatementContext *ctx) = 0;

  virtual void enterForStatement(CHTLJSParser::ForStatementContext *ctx) = 0;
  virtual void exitForStatement(CHTLJSParser::ForStatementContext *ctx) = 0;

  virtual void enterWhileStatement(CHTLJSParser::WhileStatementContext *ctx) = 0;
  virtual void exitWhileStatement(CHTLJSParser::WhileStatementContext *ctx) = 0;

  virtual void enterSwitchStatement(CHTLJSParser::SwitchStatementContext *ctx) = 0;
  virtual void exitSwitchStatement(CHTLJSParser::SwitchStatementContext *ctx) = 0;

  virtual void enterCaseClause(CHTLJSParser::CaseClauseContext *ctx) = 0;
  virtual void exitCaseClause(CHTLJSParser::CaseClauseContext *ctx) = 0;

  virtual void enterDefaultClause(CHTLJSParser::DefaultClauseContext *ctx) = 0;
  virtual void exitDefaultClause(CHTLJSParser::DefaultClauseContext *ctx) = 0;

  virtual void enterFileloaderStatement(CHTLJSParser::FileloaderStatementContext *ctx) = 0;
  virtual void exitFileloaderStatement(CHTLJSParser::FileloaderStatementContext *ctx) = 0;

  virtual void enterListenStatement(CHTLJSParser::ListenStatementContext *ctx) = 0;
  virtual void exitListenStatement(CHTLJSParser::ListenStatementContext *ctx) = 0;

  virtual void enterDelegateStatement(CHTLJSParser::DelegateStatementContext *ctx) = 0;
  virtual void exitDelegateStatement(CHTLJSParser::DelegateStatementContext *ctx) = 0;

  virtual void enterAnimateStatement(CHTLJSParser::AnimateStatementContext *ctx) = 0;
  virtual void exitAnimateStatement(CHTLJSParser::AnimateStatementContext *ctx) = 0;

  virtual void enterVirStatement(CHTLJSParser::VirStatementContext *ctx) = 0;
  virtual void exitVirStatement(CHTLJSParser::VirStatementContext *ctx) = 0;

  virtual void enterRouterStatement(CHTLJSParser::RouterStatementContext *ctx) = 0;
  virtual void exitRouterStatement(CHTLJSParser::RouterStatementContext *ctx) = 0;

  virtual void enterUtilStatement(CHTLJSParser::UtilStatementContext *ctx) = 0;
  virtual void exitUtilStatement(CHTLJSParser::UtilStatementContext *ctx) = 0;

  virtual void enterBlock(CHTLJSParser::BlockContext *ctx) = 0;
  virtual void exitBlock(CHTLJSParser::BlockContext *ctx) = 0;

  virtual void enterArgumentList(CHTLJSParser::ArgumentListContext *ctx) = 0;
  virtual void exitArgumentList(CHTLJSParser::ArgumentListContext *ctx) = 0;

  virtual void enterAssignmentOperator(CHTLJSParser::AssignmentOperatorContext *ctx) = 0;
  virtual void exitAssignmentOperator(CHTLJSParser::AssignmentOperatorContext *ctx) = 0;


};

}  // namespace chtl.antlr
