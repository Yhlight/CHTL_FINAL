
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLParser.h"


namespace chtl.antlr {

/**
 * This interface defines an abstract listener for a parse tree produced by CHTLParser.
 */
class  CHTLListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(CHTLParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(CHTLParser::ProgramContext *ctx) = 0;

  virtual void enterElement(CHTLParser::ElementContext *ctx) = 0;
  virtual void exitElement(CHTLParser::ElementContext *ctx) = 0;

  virtual void enterHtmlElement(CHTLParser::HtmlElementContext *ctx) = 0;
  virtual void exitHtmlElement(CHTLParser::HtmlElementContext *ctx) = 0;

  virtual void enterTextElement(CHTLParser::TextElementContext *ctx) = 0;
  virtual void exitTextElement(CHTLParser::TextElementContext *ctx) = 0;

  virtual void enterStyleElement(CHTLParser::StyleElementContext *ctx) = 0;
  virtual void exitStyleElement(CHTLParser::StyleElementContext *ctx) = 0;

  virtual void enterScriptElement(CHTLParser::ScriptElementContext *ctx) = 0;
  virtual void exitScriptElement(CHTLParser::ScriptElementContext *ctx) = 0;

  virtual void enterAttributes(CHTLParser::AttributesContext *ctx) = 0;
  virtual void exitAttributes(CHTLParser::AttributesContext *ctx) = 0;

  virtual void enterAttribute(CHTLParser::AttributeContext *ctx) = 0;
  virtual void exitAttribute(CHTLParser::AttributeContext *ctx) = 0;

  virtual void enterValue(CHTLParser::ValueContext *ctx) = 0;
  virtual void exitValue(CHTLParser::ValueContext *ctx) = 0;

  virtual void enterBlock(CHTLParser::BlockContext *ctx) = 0;
  virtual void exitBlock(CHTLParser::BlockContext *ctx) = 0;

  virtual void enterText(CHTLParser::TextContext *ctx) = 0;
  virtual void exitText(CHTLParser::TextContext *ctx) = 0;

  virtual void enterTemplate(CHTLParser::TemplateContext *ctx) = 0;
  virtual void exitTemplate(CHTLParser::TemplateContext *ctx) = 0;

  virtual void enterTemplateType(CHTLParser::TemplateTypeContext *ctx) = 0;
  virtual void exitTemplateType(CHTLParser::TemplateTypeContext *ctx) = 0;

  virtual void enterCustom(CHTLParser::CustomContext *ctx) = 0;
  virtual void exitCustom(CHTLParser::CustomContext *ctx) = 0;

  virtual void enterCustomType(CHTLParser::CustomTypeContext *ctx) = 0;
  virtual void exitCustomType(CHTLParser::CustomTypeContext *ctx) = 0;

  virtual void enterOrigin(CHTLParser::OriginContext *ctx) = 0;
  virtual void exitOrigin(CHTLParser::OriginContext *ctx) = 0;

  virtual void enterOriginType(CHTLParser::OriginTypeContext *ctx) = 0;
  virtual void exitOriginType(CHTLParser::OriginTypeContext *ctx) = 0;

  virtual void enterImportStatement(CHTLParser::ImportStatementContext *ctx) = 0;
  virtual void exitImportStatement(CHTLParser::ImportStatementContext *ctx) = 0;

  virtual void enterImportType(CHTLParser::ImportTypeContext *ctx) = 0;
  virtual void exitImportType(CHTLParser::ImportTypeContext *ctx) = 0;

  virtual void enterPath(CHTLParser::PathContext *ctx) = 0;
  virtual void exitPath(CHTLParser::PathContext *ctx) = 0;

  virtual void enterNamespace(CHTLParser::NamespaceContext *ctx) = 0;
  virtual void exitNamespace(CHTLParser::NamespaceContext *ctx) = 0;

  virtual void enterConfiguration(CHTLParser::ConfigurationContext *ctx) = 0;
  virtual void exitConfiguration(CHTLParser::ConfigurationContext *ctx) = 0;

  virtual void enterUseStatement(CHTLParser::UseStatementContext *ctx) = 0;
  virtual void exitUseStatement(CHTLParser::UseStatementContext *ctx) = 0;

  virtual void enterComment(CHTLParser::CommentContext *ctx) = 0;
  virtual void exitComment(CHTLParser::CommentContext *ctx) = 0;

  virtual void enterExpression(CHTLParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(CHTLParser::ExpressionContext *ctx) = 0;

  virtual void enterTerm(CHTLParser::TermContext *ctx) = 0;
  virtual void exitTerm(CHTLParser::TermContext *ctx) = 0;

  virtual void enterFunctionCall(CHTLParser::FunctionCallContext *ctx) = 0;
  virtual void exitFunctionCall(CHTLParser::FunctionCallContext *ctx) = 0;

  virtual void enterPropertyAccess(CHTLParser::PropertyAccessContext *ctx) = 0;
  virtual void exitPropertyAccess(CHTLParser::PropertyAccessContext *ctx) = 0;

  virtual void enterIndexAccess(CHTLParser::IndexAccessContext *ctx) = 0;
  virtual void exitIndexAccess(CHTLParser::IndexAccessContext *ctx) = 0;

  virtual void enterOperator(CHTLParser::OperatorContext *ctx) = 0;
  virtual void exitOperator(CHTLParser::OperatorContext *ctx) = 0;


};

}  // namespace chtl.antlr
