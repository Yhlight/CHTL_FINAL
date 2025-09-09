
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLVisitor.h"


namespace chtl.antlr {

/**
 * This class provides an empty implementation of CHTLVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CHTLBaseVisitor : public CHTLVisitor {
public:

  virtual std::any visitProgram(CHTLParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElement(CHTLParser::ElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHtmlElement(CHTLParser::HtmlElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTextElement(CHTLParser::TextElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStyleElement(CHTLParser::StyleElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScriptElement(CHTLParser::ScriptElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttributes(CHTLParser::AttributesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttribute(CHTLParser::AttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue(CHTLParser::ValueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(CHTLParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitText(CHTLParser::TextContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplate(CHTLParser::TemplateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTemplateType(CHTLParser::TemplateTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustom(CHTLParser::CustomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCustomType(CHTLParser::CustomTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrigin(CHTLParser::OriginContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOriginType(CHTLParser::OriginTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportStatement(CHTLParser::ImportStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImportType(CHTLParser::ImportTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPath(CHTLParser::PathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamespace(CHTLParser::NamespaceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfiguration(CHTLParser::ConfigurationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUseStatement(CHTLParser::UseStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComment(CHTLParser::CommentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(CHTLParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTerm(CHTLParser::TermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionCall(CHTLParser::FunctionCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPropertyAccess(CHTLParser::PropertyAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIndexAccess(CHTLParser::IndexAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator(CHTLParser::OperatorContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace chtl.antlr
