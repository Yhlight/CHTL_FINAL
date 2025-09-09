
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"


namespace chtl.antlr {


class  CHTLParser : public antlr4::Parser {
public:
  enum {
    TEMPLATE = 1, CUSTOM = 2, ORIGIN = 3, IMPORT = 4, NAMESPACE = 5, CONFIGURATION = 6, 
    INFO = 7, EXPORT = 8, TEXT = 9, STYLE = 10, SCRIPT = 11, USE = 12, EXCEPT = 13, 
    DELETE = 14, INSERT = 15, INHERIT = 16, FROM = 17, AS = 18, FILELOADER = 19, 
    LISTEN = 20, DELEGATE = 21, ANIMATE = 22, VIR = 23, ROUTER = 24, UTIL = 25, 
    INEVERAWAY = 26, AFTER = 27, BEFORE = 28, REPLACE = 29, AT_TOP = 30, 
    AT_BOTTOM = 31, HTML = 32, CSS = 33, JAVASCRIPT = 34, VUE = 35, HTML5 = 36, 
    LEFT_BRACE = 37, RIGHT_BRACE = 38, LEFT_BRACKET = 39, RIGHT_BRACKET = 40, 
    LEFT_PAREN = 41, RIGHT_PAREN = 42, SEMICOLON = 43, COMMA = 44, DOT = 45, 
    COLON = 46, EQUAL = 47, PLUS = 48, MINUS = 49, MULTIPLY = 50, DIVIDE = 51, 
    MODULO = 52, AND = 53, OR = 54, NOT = 55, GREATER = 56, LESS = 57, GREATER_EQUAL = 58, 
    LESS_EQUAL = 59, EQUAL_EQUAL = 60, NOT_EQUAL = 61, AT = 62, HASH = 63, 
    DOLLAR = 64, QUESTION = 65, EXCLAMATION = 66, AMPERSAND = 67, PIPE = 68, 
    TILDE = 69, CARET = 70, ARROW = 71, STRING = 72, UNQUOTED_LITERAL = 73, 
    NUMBER = 74, IDENTIFIER = 75, COMMENT = 76, GENERATOR_COMMENT = 77, 
    MULTILINE_COMMENT = 78, WHITESPACE = 79, TEXT_CONTENT = 80, ELEMENT = 81, 
    VAR = 82
  };

  enum {
    RuleProgram = 0, RuleElement = 1, RuleHtmlElement = 2, RuleTextElement = 3, 
    RuleStyleElement = 4, RuleScriptElement = 5, RuleAttributes = 6, RuleAttribute = 7, 
    RuleValue = 8, RuleBlock = 9, RuleText = 10, RuleTemplate = 11, RuleTemplateType = 12, 
    RuleCustom = 13, RuleCustomType = 14, RuleOrigin = 15, RuleOriginType = 16, 
    RuleImportStatement = 17, RuleImportType = 18, RulePath = 19, RuleNamespace = 20, 
    RuleConfiguration = 21, RuleUseStatement = 22, RuleComment = 23, RuleExpression = 24, 
    RuleTerm = 25, RuleFunctionCall = 26, RulePropertyAccess = 27, RuleIndexAccess = 28, 
    RuleOperator = 29
  };

  explicit CHTLParser(antlr4::TokenStream *input);

  CHTLParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~CHTLParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ProgramContext;
  class ElementContext;
  class HtmlElementContext;
  class TextElementContext;
  class StyleElementContext;
  class ScriptElementContext;
  class AttributesContext;
  class AttributeContext;
  class ValueContext;
  class BlockContext;
  class TextContext;
  class TemplateContext;
  class TemplateTypeContext;
  class CustomContext;
  class CustomTypeContext;
  class OriginContext;
  class OriginTypeContext;
  class ImportStatementContext;
  class ImportTypeContext;
  class PathContext;
  class NamespaceContext;
  class ConfigurationContext;
  class UseStatementContext;
  class CommentContext;
  class ExpressionContext;
  class TermContext;
  class FunctionCallContext;
  class PropertyAccessContext;
  class IndexAccessContext;
  class OperatorContext; 

  class  ProgramContext : public antlr4::ParserRuleContext {
  public:
    ProgramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<ElementContext *> element();
    ElementContext* element(size_t i);
    std::vector<TemplateContext *> template_();
    TemplateContext* template_(size_t i);
    std::vector<CustomContext *> custom();
    CustomContext* custom(size_t i);
    std::vector<OriginContext *> origin();
    OriginContext* origin(size_t i);
    std::vector<ImportStatementContext *> importStatement();
    ImportStatementContext* importStatement(size_t i);
    std::vector<NamespaceContext *> namespace_();
    NamespaceContext* namespace_(size_t i);
    std::vector<ConfigurationContext *> configuration();
    ConfigurationContext* configuration(size_t i);
    std::vector<UseStatementContext *> useStatement();
    UseStatementContext* useStatement(size_t i);
    std::vector<CommentContext *> comment();
    CommentContext* comment(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgramContext* program();

  class  ElementContext : public antlr4::ParserRuleContext {
  public:
    ElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    HtmlElementContext *htmlElement();
    TextElementContext *textElement();
    StyleElementContext *styleElement();
    ScriptElementContext *scriptElement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ElementContext* element();

  class  HtmlElementContext : public antlr4::ParserRuleContext {
  public:
    HtmlElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    AttributesContext *attributes();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  HtmlElementContext* htmlElement();

  class  TextElementContext : public antlr4::ParserRuleContext {
  public:
    TextElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TEXT();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TextElementContext* textElement();

  class  StyleElementContext : public antlr4::ParserRuleContext {
  public:
    StyleElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STYLE();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StyleElementContext* styleElement();

  class  ScriptElementContext : public antlr4::ParserRuleContext {
  public:
    ScriptElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SCRIPT();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ScriptElementContext* scriptElement();

  class  AttributesContext : public antlr4::ParserRuleContext {
  public:
    AttributesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AttributeContext *> attribute();
    AttributeContext* attribute(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributesContext* attributes();

  class  AttributeContext : public antlr4::ParserRuleContext {
  public:
    AttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    ValueContext *value();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *SEMICOLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributeContext* attribute();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *UNQUOTED_LITERAL();
    antlr4::tree::TerminalNode *NUMBER();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ValueContext* value();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEFT_BRACE();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    std::vector<ElementContext *> element();
    ElementContext* element(size_t i);
    std::vector<TextContext *> text();
    TextContext* text(size_t i);
    std::vector<CommentContext *> comment();
    CommentContext* comment(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockContext* block();

  class  TextContext : public antlr4::ParserRuleContext {
  public:
    TextContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TEXT_CONTENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TextContext* text();

  class  TemplateContext : public antlr4::ParserRuleContext {
  public:
    TemplateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TEMPLATE();
    TemplateTypeContext *templateType();
    antlr4::tree::TerminalNode *IDENTIFIER();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TemplateContext* template_();

  class  TemplateTypeContext : public antlr4::ParserRuleContext {
  public:
    TemplateTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STYLE();
    antlr4::tree::TerminalNode *ELEMENT();
    antlr4::tree::TerminalNode *VAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TemplateTypeContext* templateType();

  class  CustomContext : public antlr4::ParserRuleContext {
  public:
    CustomContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CUSTOM();
    CustomTypeContext *customType();
    antlr4::tree::TerminalNode *IDENTIFIER();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CustomContext* custom();

  class  CustomTypeContext : public antlr4::ParserRuleContext {
  public:
    CustomTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STYLE();
    antlr4::tree::TerminalNode *ELEMENT();
    antlr4::tree::TerminalNode *VAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CustomTypeContext* customType();

  class  OriginContext : public antlr4::ParserRuleContext {
  public:
    OriginContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ORIGIN();
    OriginTypeContext *originType();
    antlr4::tree::TerminalNode *IDENTIFIER();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OriginContext* origin();

  class  OriginTypeContext : public antlr4::ParserRuleContext {
  public:
    OriginTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HTML();
    antlr4::tree::TerminalNode *CSS();
    antlr4::tree::TerminalNode *JAVASCRIPT();
    antlr4::tree::TerminalNode *VUE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OriginTypeContext* originType();

  class  ImportStatementContext : public antlr4::ParserRuleContext {
  public:
    ImportStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORT();
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *FROM();
    PathContext *path();
    ImportTypeContext *importType();
    antlr4::tree::TerminalNode *AS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportStatementContext* importStatement();

  class  ImportTypeContext : public antlr4::ParserRuleContext {
  public:
    ImportTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TEMPLATE();
    antlr4::tree::TerminalNode *CUSTOM();
    antlr4::tree::TerminalNode *ORIGIN();
    antlr4::tree::TerminalNode *CONFIGURATION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportTypeContext* importType();

  class  PathContext : public antlr4::ParserRuleContext {
  public:
    PathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *UNQUOTED_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PathContext* path();

  class  NamespaceContext : public antlr4::ParserRuleContext {
  public:
    NamespaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NAMESPACE();
    antlr4::tree::TerminalNode *IDENTIFIER();
    BlockContext *block();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NamespaceContext* namespace_();

  class  ConfigurationContext : public antlr4::ParserRuleContext {
  public:
    ConfigurationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONFIGURATION();
    BlockContext *block();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConfigurationContext* configuration();

  class  UseStatementContext : public antlr4::ParserRuleContext {
  public:
    UseStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *USE();
    antlr4::tree::TerminalNode *HTML5();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UseStatementContext* useStatement();

  class  CommentContext : public antlr4::ParserRuleContext {
  public:
    CommentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMENT();
    antlr4::tree::TerminalNode *GENERATOR_COMMENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CommentContext* comment();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    std::vector<OperatorContext *> operator_();
    OperatorContext* operator_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *UNQUOTED_LITERAL();
    antlr4::tree::TerminalNode *NUMBER();
    FunctionCallContext *functionCall();
    PropertyAccessContext *propertyAccess();
    IndexAccessContext *indexAccess();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TermContext* term();

  class  FunctionCallContext : public antlr4::ParserRuleContext {
  public:
    FunctionCallContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionCallContext* functionCall();

  class  PropertyAccessContext : public antlr4::ParserRuleContext {
  public:
    PropertyAccessContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertyAccessContext* propertyAccess();

  class  IndexAccessContext : public antlr4::ParserRuleContext {
  public:
    IndexAccessContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LEFT_BRACKET();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RIGHT_BRACKET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IndexAccessContext* indexAccess();

  class  OperatorContext : public antlr4::ParserRuleContext {
  public:
    OperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *MULTIPLY();
    antlr4::tree::TerminalNode *DIVIDE();
    antlr4::tree::TerminalNode *MODULO();
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *NOT();
    antlr4::tree::TerminalNode *GREATER();
    antlr4::tree::TerminalNode *LESS();
    antlr4::tree::TerminalNode *GREATER_EQUAL();
    antlr4::tree::TerminalNode *LESS_EQUAL();
    antlr4::tree::TerminalNode *EQUAL_EQUAL();
    antlr4::tree::TerminalNode *NOT_EQUAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OperatorContext* operator_();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

}  // namespace chtl.antlr
