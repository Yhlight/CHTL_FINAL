
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLJSListener.h"


namespace chtl.antlr {

/**
 * This class provides an empty implementation of CHTLJSListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  CHTLJSBaseListener : public CHTLJSListener {
public:

  virtual void enterProgram(CHTLJSParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(CHTLJSParser::ProgramContext * /*ctx*/) override { }

  virtual void enterStatement(CHTLJSParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(CHTLJSParser::StatementContext * /*ctx*/) override { }

  virtual void enterExpressionStatement(CHTLJSParser::ExpressionStatementContext * /*ctx*/) override { }
  virtual void exitExpressionStatement(CHTLJSParser::ExpressionStatementContext * /*ctx*/) override { }

  virtual void enterDeclarationStatement(CHTLJSParser::DeclarationStatementContext * /*ctx*/) override { }
  virtual void exitDeclarationStatement(CHTLJSParser::DeclarationStatementContext * /*ctx*/) override { }

  virtual void enterControlStatement(CHTLJSParser::ControlStatementContext * /*ctx*/) override { }
  virtual void exitControlStatement(CHTLJSParser::ControlStatementContext * /*ctx*/) override { }

  virtual void enterChthljsStatement(CHTLJSParser::ChthljsStatementContext * /*ctx*/) override { }
  virtual void exitChthljsStatement(CHTLJSParser::ChthljsStatementContext * /*ctx*/) override { }

  virtual void enterExpression(CHTLJSParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(CHTLJSParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterAssignmentExpression(CHTLJSParser::AssignmentExpressionContext * /*ctx*/) override { }
  virtual void exitAssignmentExpression(CHTLJSParser::AssignmentExpressionContext * /*ctx*/) override { }

  virtual void enterConditionalExpression(CHTLJSParser::ConditionalExpressionContext * /*ctx*/) override { }
  virtual void exitConditionalExpression(CHTLJSParser::ConditionalExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalOrExpression(CHTLJSParser::LogicalOrExpressionContext * /*ctx*/) override { }

  virtual void enterLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext * /*ctx*/) override { }
  virtual void exitLogicalAndExpression(CHTLJSParser::LogicalAndExpressionContext * /*ctx*/) override { }

  virtual void enterEqualityExpression(CHTLJSParser::EqualityExpressionContext * /*ctx*/) override { }
  virtual void exitEqualityExpression(CHTLJSParser::EqualityExpressionContext * /*ctx*/) override { }

  virtual void enterRelationalExpression(CHTLJSParser::RelationalExpressionContext * /*ctx*/) override { }
  virtual void exitRelationalExpression(CHTLJSParser::RelationalExpressionContext * /*ctx*/) override { }

  virtual void enterAdditiveExpression(CHTLJSParser::AdditiveExpressionContext * /*ctx*/) override { }
  virtual void exitAdditiveExpression(CHTLJSParser::AdditiveExpressionContext * /*ctx*/) override { }

  virtual void enterMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext * /*ctx*/) override { }
  virtual void exitMultiplicativeExpression(CHTLJSParser::MultiplicativeExpressionContext * /*ctx*/) override { }

  virtual void enterUnaryExpression(CHTLJSParser::UnaryExpressionContext * /*ctx*/) override { }
  virtual void exitUnaryExpression(CHTLJSParser::UnaryExpressionContext * /*ctx*/) override { }

  virtual void enterPostfixExpression(CHTLJSParser::PostfixExpressionContext * /*ctx*/) override { }
  virtual void exitPostfixExpression(CHTLJSParser::PostfixExpressionContext * /*ctx*/) override { }

  virtual void enterPrimaryExpression(CHTLJSParser::PrimaryExpressionContext * /*ctx*/) override { }
  virtual void exitPrimaryExpression(CHTLJSParser::PrimaryExpressionContext * /*ctx*/) override { }

  virtual void enterLiteral(CHTLJSParser::LiteralContext * /*ctx*/) override { }
  virtual void exitLiteral(CHTLJSParser::LiteralContext * /*ctx*/) override { }

  virtual void enterIdentifier(CHTLJSParser::IdentifierContext * /*ctx*/) override { }
  virtual void exitIdentifier(CHTLJSParser::IdentifierContext * /*ctx*/) override { }

  virtual void enterChthljsSelector(CHTLJSParser::ChthljsSelectorContext * /*ctx*/) override { }
  virtual void exitChthljsSelector(CHTLJSParser::ChthljsSelectorContext * /*ctx*/) override { }

  virtual void enterSelectorContent(CHTLJSParser::SelectorContentContext * /*ctx*/) override { }
  virtual void exitSelectorContent(CHTLJSParser::SelectorContentContext * /*ctx*/) override { }

  virtual void enterSelector(CHTLJSParser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(CHTLJSParser::SelectorContext * /*ctx*/) override { }

  virtual void enterProperty(CHTLJSParser::PropertyContext * /*ctx*/) override { }
  virtual void exitProperty(CHTLJSParser::PropertyContext * /*ctx*/) override { }

  virtual void enterIndex(CHTLJSParser::IndexContext * /*ctx*/) override { }
  virtual void exitIndex(CHTLJSParser::IndexContext * /*ctx*/) override { }

  virtual void enterFunctionExpression(CHTLJSParser::FunctionExpressionContext * /*ctx*/) override { }
  virtual void exitFunctionExpression(CHTLJSParser::FunctionExpressionContext * /*ctx*/) override { }

  virtual void enterParameterList(CHTLJSParser::ParameterListContext * /*ctx*/) override { }
  virtual void exitParameterList(CHTLJSParser::ParameterListContext * /*ctx*/) override { }

  virtual void enterObjectExpression(CHTLJSParser::ObjectExpressionContext * /*ctx*/) override { }
  virtual void exitObjectExpression(CHTLJSParser::ObjectExpressionContext * /*ctx*/) override { }

  virtual void enterPropertyAssignment(CHTLJSParser::PropertyAssignmentContext * /*ctx*/) override { }
  virtual void exitPropertyAssignment(CHTLJSParser::PropertyAssignmentContext * /*ctx*/) override { }

  virtual void enterPropertyName(CHTLJSParser::PropertyNameContext * /*ctx*/) override { }
  virtual void exitPropertyName(CHTLJSParser::PropertyNameContext * /*ctx*/) override { }

  virtual void enterArrayExpression(CHTLJSParser::ArrayExpressionContext * /*ctx*/) override { }
  virtual void exitArrayExpression(CHTLJSParser::ArrayExpressionContext * /*ctx*/) override { }

  virtual void enterParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext * /*ctx*/) override { }
  virtual void exitParenthesizedExpression(CHTLJSParser::ParenthesizedExpressionContext * /*ctx*/) override { }

  virtual void enterVariableDeclaration(CHTLJSParser::VariableDeclarationContext * /*ctx*/) override { }
  virtual void exitVariableDeclaration(CHTLJSParser::VariableDeclarationContext * /*ctx*/) override { }

  virtual void enterVariableDeclarator(CHTLJSParser::VariableDeclaratorContext * /*ctx*/) override { }
  virtual void exitVariableDeclarator(CHTLJSParser::VariableDeclaratorContext * /*ctx*/) override { }

  virtual void enterFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext * /*ctx*/) override { }
  virtual void exitFunctionDeclaration(CHTLJSParser::FunctionDeclarationContext * /*ctx*/) override { }

  virtual void enterIfStatement(CHTLJSParser::IfStatementContext * /*ctx*/) override { }
  virtual void exitIfStatement(CHTLJSParser::IfStatementContext * /*ctx*/) override { }

  virtual void enterForStatement(CHTLJSParser::ForStatementContext * /*ctx*/) override { }
  virtual void exitForStatement(CHTLJSParser::ForStatementContext * /*ctx*/) override { }

  virtual void enterWhileStatement(CHTLJSParser::WhileStatementContext * /*ctx*/) override { }
  virtual void exitWhileStatement(CHTLJSParser::WhileStatementContext * /*ctx*/) override { }

  virtual void enterSwitchStatement(CHTLJSParser::SwitchStatementContext * /*ctx*/) override { }
  virtual void exitSwitchStatement(CHTLJSParser::SwitchStatementContext * /*ctx*/) override { }

  virtual void enterCaseClause(CHTLJSParser::CaseClauseContext * /*ctx*/) override { }
  virtual void exitCaseClause(CHTLJSParser::CaseClauseContext * /*ctx*/) override { }

  virtual void enterDefaultClause(CHTLJSParser::DefaultClauseContext * /*ctx*/) override { }
  virtual void exitDefaultClause(CHTLJSParser::DefaultClauseContext * /*ctx*/) override { }

  virtual void enterFileloaderStatement(CHTLJSParser::FileloaderStatementContext * /*ctx*/) override { }
  virtual void exitFileloaderStatement(CHTLJSParser::FileloaderStatementContext * /*ctx*/) override { }

  virtual void enterListenStatement(CHTLJSParser::ListenStatementContext * /*ctx*/) override { }
  virtual void exitListenStatement(CHTLJSParser::ListenStatementContext * /*ctx*/) override { }

  virtual void enterDelegateStatement(CHTLJSParser::DelegateStatementContext * /*ctx*/) override { }
  virtual void exitDelegateStatement(CHTLJSParser::DelegateStatementContext * /*ctx*/) override { }

  virtual void enterAnimateStatement(CHTLJSParser::AnimateStatementContext * /*ctx*/) override { }
  virtual void exitAnimateStatement(CHTLJSParser::AnimateStatementContext * /*ctx*/) override { }

  virtual void enterVirStatement(CHTLJSParser::VirStatementContext * /*ctx*/) override { }
  virtual void exitVirStatement(CHTLJSParser::VirStatementContext * /*ctx*/) override { }

  virtual void enterRouterStatement(CHTLJSParser::RouterStatementContext * /*ctx*/) override { }
  virtual void exitRouterStatement(CHTLJSParser::RouterStatementContext * /*ctx*/) override { }

  virtual void enterUtilStatement(CHTLJSParser::UtilStatementContext * /*ctx*/) override { }
  virtual void exitUtilStatement(CHTLJSParser::UtilStatementContext * /*ctx*/) override { }

  virtual void enterBlock(CHTLJSParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(CHTLJSParser::BlockContext * /*ctx*/) override { }

  virtual void enterArgumentList(CHTLJSParser::ArgumentListContext * /*ctx*/) override { }
  virtual void exitArgumentList(CHTLJSParser::ArgumentListContext * /*ctx*/) override { }

  virtual void enterAssignmentOperator(CHTLJSParser::AssignmentOperatorContext * /*ctx*/) override { }
  virtual void exitAssignmentOperator(CHTLJSParser::AssignmentOperatorContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace chtl.antlr
