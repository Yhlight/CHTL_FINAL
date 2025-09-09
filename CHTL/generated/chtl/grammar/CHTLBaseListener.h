
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLListener.h"


namespace chtl.antlr {

/**
 * This class provides an empty implementation of CHTLListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  CHTLBaseListener : public CHTLListener {
public:

  virtual void enterProgram(CHTLParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(CHTLParser::ProgramContext * /*ctx*/) override { }

  virtual void enterElement(CHTLParser::ElementContext * /*ctx*/) override { }
  virtual void exitElement(CHTLParser::ElementContext * /*ctx*/) override { }

  virtual void enterHtmlElement(CHTLParser::HtmlElementContext * /*ctx*/) override { }
  virtual void exitHtmlElement(CHTLParser::HtmlElementContext * /*ctx*/) override { }

  virtual void enterTextElement(CHTLParser::TextElementContext * /*ctx*/) override { }
  virtual void exitTextElement(CHTLParser::TextElementContext * /*ctx*/) override { }

  virtual void enterStyleElement(CHTLParser::StyleElementContext * /*ctx*/) override { }
  virtual void exitStyleElement(CHTLParser::StyleElementContext * /*ctx*/) override { }

  virtual void enterScriptElement(CHTLParser::ScriptElementContext * /*ctx*/) override { }
  virtual void exitScriptElement(CHTLParser::ScriptElementContext * /*ctx*/) override { }

  virtual void enterAttributes(CHTLParser::AttributesContext * /*ctx*/) override { }
  virtual void exitAttributes(CHTLParser::AttributesContext * /*ctx*/) override { }

  virtual void enterAttribute(CHTLParser::AttributeContext * /*ctx*/) override { }
  virtual void exitAttribute(CHTLParser::AttributeContext * /*ctx*/) override { }

  virtual void enterValue(CHTLParser::ValueContext * /*ctx*/) override { }
  virtual void exitValue(CHTLParser::ValueContext * /*ctx*/) override { }

  virtual void enterBlock(CHTLParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(CHTLParser::BlockContext * /*ctx*/) override { }

  virtual void enterText(CHTLParser::TextContext * /*ctx*/) override { }
  virtual void exitText(CHTLParser::TextContext * /*ctx*/) override { }

  virtual void enterTemplate(CHTLParser::TemplateContext * /*ctx*/) override { }
  virtual void exitTemplate(CHTLParser::TemplateContext * /*ctx*/) override { }

  virtual void enterTemplateType(CHTLParser::TemplateTypeContext * /*ctx*/) override { }
  virtual void exitTemplateType(CHTLParser::TemplateTypeContext * /*ctx*/) override { }

  virtual void enterCustom(CHTLParser::CustomContext * /*ctx*/) override { }
  virtual void exitCustom(CHTLParser::CustomContext * /*ctx*/) override { }

  virtual void enterCustomType(CHTLParser::CustomTypeContext * /*ctx*/) override { }
  virtual void exitCustomType(CHTLParser::CustomTypeContext * /*ctx*/) override { }

  virtual void enterOrigin(CHTLParser::OriginContext * /*ctx*/) override { }
  virtual void exitOrigin(CHTLParser::OriginContext * /*ctx*/) override { }

  virtual void enterOriginType(CHTLParser::OriginTypeContext * /*ctx*/) override { }
  virtual void exitOriginType(CHTLParser::OriginTypeContext * /*ctx*/) override { }

  virtual void enterImportStatement(CHTLParser::ImportStatementContext * /*ctx*/) override { }
  virtual void exitImportStatement(CHTLParser::ImportStatementContext * /*ctx*/) override { }

  virtual void enterImportType(CHTLParser::ImportTypeContext * /*ctx*/) override { }
  virtual void exitImportType(CHTLParser::ImportTypeContext * /*ctx*/) override { }

  virtual void enterPath(CHTLParser::PathContext * /*ctx*/) override { }
  virtual void exitPath(CHTLParser::PathContext * /*ctx*/) override { }

  virtual void enterNamespace(CHTLParser::NamespaceContext * /*ctx*/) override { }
  virtual void exitNamespace(CHTLParser::NamespaceContext * /*ctx*/) override { }

  virtual void enterConfiguration(CHTLParser::ConfigurationContext * /*ctx*/) override { }
  virtual void exitConfiguration(CHTLParser::ConfigurationContext * /*ctx*/) override { }

  virtual void enterUseStatement(CHTLParser::UseStatementContext * /*ctx*/) override { }
  virtual void exitUseStatement(CHTLParser::UseStatementContext * /*ctx*/) override { }

  virtual void enterComment(CHTLParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(CHTLParser::CommentContext * /*ctx*/) override { }

  virtual void enterExpression(CHTLParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(CHTLParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterTerm(CHTLParser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(CHTLParser::TermContext * /*ctx*/) override { }

  virtual void enterFunctionCall(CHTLParser::FunctionCallContext * /*ctx*/) override { }
  virtual void exitFunctionCall(CHTLParser::FunctionCallContext * /*ctx*/) override { }

  virtual void enterPropertyAccess(CHTLParser::PropertyAccessContext * /*ctx*/) override { }
  virtual void exitPropertyAccess(CHTLParser::PropertyAccessContext * /*ctx*/) override { }

  virtual void enterIndexAccess(CHTLParser::IndexAccessContext * /*ctx*/) override { }
  virtual void exitIndexAccess(CHTLParser::IndexAccessContext * /*ctx*/) override { }

  virtual void enterOperator(CHTLParser::OperatorContext * /*ctx*/) override { }
  virtual void exitOperator(CHTLParser::OperatorContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace chtl.antlr
