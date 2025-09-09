
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "CHTLParser.h"


namespace chtl.antlr {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by CHTLParser.
 */
class  CHTLVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by CHTLParser.
   */
    virtual std::any visitProgram(CHTLParser::ProgramContext *context) = 0;

    virtual std::any visitElement(CHTLParser::ElementContext *context) = 0;

    virtual std::any visitHtmlElement(CHTLParser::HtmlElementContext *context) = 0;

    virtual std::any visitTextElement(CHTLParser::TextElementContext *context) = 0;

    virtual std::any visitStyleElement(CHTLParser::StyleElementContext *context) = 0;

    virtual std::any visitScriptElement(CHTLParser::ScriptElementContext *context) = 0;

    virtual std::any visitAttributes(CHTLParser::AttributesContext *context) = 0;

    virtual std::any visitAttribute(CHTLParser::AttributeContext *context) = 0;

    virtual std::any visitValue(CHTLParser::ValueContext *context) = 0;

    virtual std::any visitBlock(CHTLParser::BlockContext *context) = 0;

    virtual std::any visitText(CHTLParser::TextContext *context) = 0;

    virtual std::any visitTemplate(CHTLParser::TemplateContext *context) = 0;

    virtual std::any visitTemplateType(CHTLParser::TemplateTypeContext *context) = 0;

    virtual std::any visitCustom(CHTLParser::CustomContext *context) = 0;

    virtual std::any visitCustomType(CHTLParser::CustomTypeContext *context) = 0;

    virtual std::any visitOrigin(CHTLParser::OriginContext *context) = 0;

    virtual std::any visitOriginType(CHTLParser::OriginTypeContext *context) = 0;

    virtual std::any visitImportStatement(CHTLParser::ImportStatementContext *context) = 0;

    virtual std::any visitImportType(CHTLParser::ImportTypeContext *context) = 0;

    virtual std::any visitPath(CHTLParser::PathContext *context) = 0;

    virtual std::any visitNamespace(CHTLParser::NamespaceContext *context) = 0;

    virtual std::any visitConfiguration(CHTLParser::ConfigurationContext *context) = 0;

    virtual std::any visitUseStatement(CHTLParser::UseStatementContext *context) = 0;

    virtual std::any visitComment(CHTLParser::CommentContext *context) = 0;

    virtual std::any visitExpression(CHTLParser::ExpressionContext *context) = 0;

    virtual std::any visitTerm(CHTLParser::TermContext *context) = 0;

    virtual std::any visitFunctionCall(CHTLParser::FunctionCallContext *context) = 0;

    virtual std::any visitPropertyAccess(CHTLParser::PropertyAccessContext *context) = 0;

    virtual std::any visitIndexAccess(CHTLParser::IndexAccessContext *context) = 0;

    virtual std::any visitOperator(CHTLParser::OperatorContext *context) = 0;


};

}  // namespace chtl.antlr
