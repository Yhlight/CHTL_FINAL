
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1


#include "CHTLListener.h"
#include "CHTLVisitor.h"

#include "CHTLParser.h"


using namespace antlrcpp;
using namespace chtl.antlr;

using namespace antlr4;

namespace {

struct CHTLParserStaticData final {
  CHTLParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CHTLParserStaticData(const CHTLParserStaticData&) = delete;
  CHTLParserStaticData(CHTLParserStaticData&&) = delete;
  CHTLParserStaticData& operator=(const CHTLParserStaticData&) = delete;
  CHTLParserStaticData& operator=(CHTLParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag chtlParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
CHTLParserStaticData *chtlParserStaticData = nullptr;

void chtlParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (chtlParserStaticData != nullptr) {
    return;
  }
#else
  assert(chtlParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CHTLParserStaticData>(
    std::vector<std::string>{
      "program", "element", "htmlElement", "textElement", "styleElement", 
      "scriptElement", "attributes", "attribute", "value", "block", "text", 
      "template", "templateType", "custom", "customType", "origin", "originType", 
      "importStatement", "importType", "path", "namespace", "configuration", 
      "useStatement", "comment", "expression", "term", "functionCall", "propertyAccess", 
      "indexAccess", "operator"
    },
    std::vector<std::string>{
      "", "'[Template]'", "'[Custom]'", "'[Origin]'", "'[Import]'", "'[Namespace]'", 
      "'[Configuration]'", "'[Info]'", "'[Export]'", "'text'", "'style'", 
      "'script'", "'use'", "'except'", "'delete'", "'insert'", "'inherit'", 
      "'from'", "'as'", "'fileloader'", "'listen'", "'delegate'", "'animate'", 
      "'vir'", "'router'", "'util'", "'iNeverAway'", "'after'", "'before'", 
      "'replace'", "'at top'", "'at bottom'", "'HTML'", "'CSS'", "'JavaScript'", 
      "'Vue'", "'HTML5'", "'{'", "'}'", "'['", "']'", "'('", "')'", "';'", 
      "','", "'.'", "':'", "'='", "'+'", "'-'", "'*'", "'/'", "'%'", "'&&'", 
      "'||'", "", "'>'", "'<'", "'>='", "'<='", "'=='", "'!='", "'@'", "'#'", 
      "'$'", "'\\u003F'", "", "'&'", "'|'", "'~'", "'^'", "'->'"
    },
    std::vector<std::string>{
      "", "TEMPLATE", "CUSTOM", "ORIGIN", "IMPORT", "NAMESPACE", "CONFIGURATION", 
      "INFO", "EXPORT", "TEXT", "STYLE", "SCRIPT", "USE", "EXCEPT", "DELETE", 
      "INSERT", "INHERIT", "FROM", "AS", "FILELOADER", "LISTEN", "DELEGATE", 
      "ANIMATE", "VIR", "ROUTER", "UTIL", "INEVERAWAY", "AFTER", "BEFORE", 
      "REPLACE", "AT_TOP", "AT_BOTTOM", "HTML", "CSS", "JAVASCRIPT", "VUE", 
      "HTML5", "LEFT_BRACE", "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET", 
      "LEFT_PAREN", "RIGHT_PAREN", "SEMICOLON", "COMMA", "DOT", "COLON", 
      "EQUAL", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULO", "AND", "OR", 
      "NOT", "GREATER", "LESS", "GREATER_EQUAL", "LESS_EQUAL", "EQUAL_EQUAL", 
      "NOT_EQUAL", "AT", "HASH", "DOLLAR", "QUESTION", "EXCLAMATION", "AMPERSAND", 
      "PIPE", "TILDE", "CARET", "ARROW", "STRING", "UNQUOTED_LITERAL", "NUMBER", 
      "IDENTIFIER", "COMMENT", "GENERATOR_COMMENT", "MULTILINE_COMMENT", 
      "WHITESPACE", "TEXT_CONTENT", "ELEMENT", "VAR"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,82,223,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,5,0,70,8,0,10,0,12,0,
  	73,9,0,1,0,1,0,1,1,1,1,1,1,1,1,3,1,81,8,1,1,2,1,2,3,2,85,8,2,1,2,3,2,
  	88,8,2,1,3,1,3,1,3,1,4,1,4,1,4,1,5,1,5,1,5,1,6,4,6,100,8,6,11,6,12,6,
  	101,1,7,1,7,1,7,1,7,3,7,108,8,7,1,8,1,8,1,8,1,8,3,8,114,8,8,1,9,1,9,1,
  	9,1,9,5,9,120,8,9,10,9,12,9,123,9,9,1,9,1,9,1,10,1,10,1,11,1,11,1,11,
  	1,11,1,11,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,14,1,14,1,15,1,15,1,15,
  	1,15,1,15,1,16,1,16,1,17,1,17,3,17,152,8,17,1,17,1,17,1,17,1,17,1,17,
  	3,17,159,8,17,1,18,1,18,1,19,1,19,1,20,1,20,1,20,1,20,1,21,1,21,3,21,
  	171,8,21,1,21,1,21,1,22,1,22,1,22,1,23,1,23,1,24,1,24,1,24,1,24,5,24,
  	184,8,24,10,24,12,24,187,9,24,1,25,1,25,1,25,1,25,1,25,1,25,1,25,3,25,
  	196,8,25,1,26,1,26,1,26,1,26,1,26,5,26,203,8,26,10,26,12,26,206,9,26,
  	3,26,208,8,26,1,26,1,26,1,27,1,27,1,27,1,27,1,28,1,28,1,28,1,28,1,28,
  	1,29,1,29,1,29,0,0,30,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,
  	36,38,40,42,44,46,48,50,52,54,56,58,0,8,1,0,46,47,2,0,10,10,81,82,1,0,
  	32,35,2,0,1,3,6,6,1,0,72,73,2,0,36,36,75,75,1,0,76,77,1,0,48,61,226,0,
  	71,1,0,0,0,2,80,1,0,0,0,4,82,1,0,0,0,6,89,1,0,0,0,8,92,1,0,0,0,10,95,
  	1,0,0,0,12,99,1,0,0,0,14,103,1,0,0,0,16,113,1,0,0,0,18,115,1,0,0,0,20,
  	126,1,0,0,0,22,128,1,0,0,0,24,133,1,0,0,0,26,135,1,0,0,0,28,140,1,0,0,
  	0,30,142,1,0,0,0,32,147,1,0,0,0,34,149,1,0,0,0,36,160,1,0,0,0,38,162,
  	1,0,0,0,40,164,1,0,0,0,42,168,1,0,0,0,44,174,1,0,0,0,46,177,1,0,0,0,48,
  	179,1,0,0,0,50,195,1,0,0,0,52,197,1,0,0,0,54,211,1,0,0,0,56,215,1,0,0,
  	0,58,220,1,0,0,0,60,70,3,2,1,0,61,70,3,22,11,0,62,70,3,26,13,0,63,70,
  	3,30,15,0,64,70,3,34,17,0,65,70,3,40,20,0,66,70,3,42,21,0,67,70,3,44,
  	22,0,68,70,3,46,23,0,69,60,1,0,0,0,69,61,1,0,0,0,69,62,1,0,0,0,69,63,
  	1,0,0,0,69,64,1,0,0,0,69,65,1,0,0,0,69,66,1,0,0,0,69,67,1,0,0,0,69,68,
  	1,0,0,0,70,73,1,0,0,0,71,69,1,0,0,0,71,72,1,0,0,0,72,74,1,0,0,0,73,71,
  	1,0,0,0,74,75,5,0,0,1,75,1,1,0,0,0,76,81,3,4,2,0,77,81,3,6,3,0,78,81,
  	3,8,4,0,79,81,3,10,5,0,80,76,1,0,0,0,80,77,1,0,0,0,80,78,1,0,0,0,80,79,
  	1,0,0,0,81,3,1,0,0,0,82,84,5,75,0,0,83,85,3,12,6,0,84,83,1,0,0,0,84,85,
  	1,0,0,0,85,87,1,0,0,0,86,88,3,18,9,0,87,86,1,0,0,0,87,88,1,0,0,0,88,5,
  	1,0,0,0,89,90,5,9,0,0,90,91,3,18,9,0,91,7,1,0,0,0,92,93,5,10,0,0,93,94,
  	3,18,9,0,94,9,1,0,0,0,95,96,5,11,0,0,96,97,3,18,9,0,97,11,1,0,0,0,98,
  	100,3,14,7,0,99,98,1,0,0,0,100,101,1,0,0,0,101,99,1,0,0,0,101,102,1,0,
  	0,0,102,13,1,0,0,0,103,104,5,75,0,0,104,105,7,0,0,0,105,107,3,16,8,0,
  	106,108,5,43,0,0,107,106,1,0,0,0,107,108,1,0,0,0,108,15,1,0,0,0,109,114,
  	5,72,0,0,110,114,5,73,0,0,111,114,5,74,0,0,112,114,3,48,24,0,113,109,
  	1,0,0,0,113,110,1,0,0,0,113,111,1,0,0,0,113,112,1,0,0,0,114,17,1,0,0,
  	0,115,121,5,37,0,0,116,120,3,2,1,0,117,120,3,20,10,0,118,120,3,46,23,
  	0,119,116,1,0,0,0,119,117,1,0,0,0,119,118,1,0,0,0,120,123,1,0,0,0,121,
  	119,1,0,0,0,121,122,1,0,0,0,122,124,1,0,0,0,123,121,1,0,0,0,124,125,5,
  	38,0,0,125,19,1,0,0,0,126,127,5,80,0,0,127,21,1,0,0,0,128,129,5,1,0,0,
  	129,130,3,24,12,0,130,131,5,75,0,0,131,132,3,18,9,0,132,23,1,0,0,0,133,
  	134,7,1,0,0,134,25,1,0,0,0,135,136,5,2,0,0,136,137,3,28,14,0,137,138,
  	5,75,0,0,138,139,3,18,9,0,139,27,1,0,0,0,140,141,7,1,0,0,141,29,1,0,0,
  	0,142,143,5,3,0,0,143,144,3,32,16,0,144,145,5,75,0,0,145,146,3,18,9,0,
  	146,31,1,0,0,0,147,148,7,2,0,0,148,33,1,0,0,0,149,151,5,4,0,0,150,152,
  	3,36,18,0,151,150,1,0,0,0,151,152,1,0,0,0,152,153,1,0,0,0,153,154,5,75,
  	0,0,154,155,5,17,0,0,155,158,3,38,19,0,156,157,5,18,0,0,157,159,5,75,
  	0,0,158,156,1,0,0,0,158,159,1,0,0,0,159,35,1,0,0,0,160,161,7,3,0,0,161,
  	37,1,0,0,0,162,163,7,4,0,0,163,39,1,0,0,0,164,165,5,5,0,0,165,166,5,75,
  	0,0,166,167,3,18,9,0,167,41,1,0,0,0,168,170,5,6,0,0,169,171,5,75,0,0,
  	170,169,1,0,0,0,170,171,1,0,0,0,171,172,1,0,0,0,172,173,3,18,9,0,173,
  	43,1,0,0,0,174,175,5,12,0,0,175,176,7,5,0,0,176,45,1,0,0,0,177,178,7,
  	6,0,0,178,47,1,0,0,0,179,185,3,50,25,0,180,181,3,58,29,0,181,182,3,50,
  	25,0,182,184,1,0,0,0,183,180,1,0,0,0,184,187,1,0,0,0,185,183,1,0,0,0,
  	185,186,1,0,0,0,186,49,1,0,0,0,187,185,1,0,0,0,188,196,5,75,0,0,189,196,
  	5,72,0,0,190,196,5,73,0,0,191,196,5,74,0,0,192,196,3,52,26,0,193,196,
  	3,54,27,0,194,196,3,56,28,0,195,188,1,0,0,0,195,189,1,0,0,0,195,190,1,
  	0,0,0,195,191,1,0,0,0,195,192,1,0,0,0,195,193,1,0,0,0,195,194,1,0,0,0,
  	196,51,1,0,0,0,197,198,5,75,0,0,198,207,5,41,0,0,199,204,3,48,24,0,200,
  	201,5,44,0,0,201,203,3,48,24,0,202,200,1,0,0,0,203,206,1,0,0,0,204,202,
  	1,0,0,0,204,205,1,0,0,0,205,208,1,0,0,0,206,204,1,0,0,0,207,199,1,0,0,
  	0,207,208,1,0,0,0,208,209,1,0,0,0,209,210,5,42,0,0,210,53,1,0,0,0,211,
  	212,5,75,0,0,212,213,5,45,0,0,213,214,5,75,0,0,214,55,1,0,0,0,215,216,
  	5,75,0,0,216,217,5,39,0,0,217,218,3,48,24,0,218,219,5,40,0,0,219,57,1,
  	0,0,0,220,221,7,7,0,0,221,59,1,0,0,0,17,69,71,80,84,87,101,107,113,119,
  	121,151,158,170,185,195,204,207
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  chtlParserStaticData = staticData.release();
}

}

CHTLParser::CHTLParser(TokenStream *input) : CHTLParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CHTLParser::CHTLParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CHTLParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *chtlParserStaticData->atn, chtlParserStaticData->decisionToDFA, chtlParserStaticData->sharedContextCache, options);
}

CHTLParser::~CHTLParser() {
  delete _interpreter;
}

const atn::ATN& CHTLParser::getATN() const {
  return *chtlParserStaticData->atn;
}

std::string CHTLParser::getGrammarFileName() const {
  return "CHTL.g4";
}

const std::vector<std::string>& CHTLParser::getRuleNames() const {
  return chtlParserStaticData->ruleNames;
}

const dfa::Vocabulary& CHTLParser::getVocabulary() const {
  return chtlParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CHTLParser::getSerializedATN() const {
  return chtlParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

CHTLParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ProgramContext::EOF() {
  return getToken(CHTLParser::EOF, 0);
}

std::vector<CHTLParser::ElementContext *> CHTLParser::ProgramContext::element() {
  return getRuleContexts<CHTLParser::ElementContext>();
}

CHTLParser::ElementContext* CHTLParser::ProgramContext::element(size_t i) {
  return getRuleContext<CHTLParser::ElementContext>(i);
}

std::vector<CHTLParser::TemplateContext *> CHTLParser::ProgramContext::template_() {
  return getRuleContexts<CHTLParser::TemplateContext>();
}

CHTLParser::TemplateContext* CHTLParser::ProgramContext::template_(size_t i) {
  return getRuleContext<CHTLParser::TemplateContext>(i);
}

std::vector<CHTLParser::CustomContext *> CHTLParser::ProgramContext::custom() {
  return getRuleContexts<CHTLParser::CustomContext>();
}

CHTLParser::CustomContext* CHTLParser::ProgramContext::custom(size_t i) {
  return getRuleContext<CHTLParser::CustomContext>(i);
}

std::vector<CHTLParser::OriginContext *> CHTLParser::ProgramContext::origin() {
  return getRuleContexts<CHTLParser::OriginContext>();
}

CHTLParser::OriginContext* CHTLParser::ProgramContext::origin(size_t i) {
  return getRuleContext<CHTLParser::OriginContext>(i);
}

std::vector<CHTLParser::ImportStatementContext *> CHTLParser::ProgramContext::importStatement() {
  return getRuleContexts<CHTLParser::ImportStatementContext>();
}

CHTLParser::ImportStatementContext* CHTLParser::ProgramContext::importStatement(size_t i) {
  return getRuleContext<CHTLParser::ImportStatementContext>(i);
}

std::vector<CHTLParser::NamespaceContext *> CHTLParser::ProgramContext::namespace_() {
  return getRuleContexts<CHTLParser::NamespaceContext>();
}

CHTLParser::NamespaceContext* CHTLParser::ProgramContext::namespace_(size_t i) {
  return getRuleContext<CHTLParser::NamespaceContext>(i);
}

std::vector<CHTLParser::ConfigurationContext *> CHTLParser::ProgramContext::configuration() {
  return getRuleContexts<CHTLParser::ConfigurationContext>();
}

CHTLParser::ConfigurationContext* CHTLParser::ProgramContext::configuration(size_t i) {
  return getRuleContext<CHTLParser::ConfigurationContext>(i);
}

std::vector<CHTLParser::UseStatementContext *> CHTLParser::ProgramContext::useStatement() {
  return getRuleContexts<CHTLParser::UseStatementContext>();
}

CHTLParser::UseStatementContext* CHTLParser::ProgramContext::useStatement(size_t i) {
  return getRuleContext<CHTLParser::UseStatementContext>(i);
}

std::vector<CHTLParser::CommentContext *> CHTLParser::ProgramContext::comment() {
  return getRuleContexts<CHTLParser::CommentContext>();
}

CHTLParser::CommentContext* CHTLParser::ProgramContext::comment(size_t i) {
  return getRuleContext<CHTLParser::CommentContext>(i);
}


size_t CHTLParser::ProgramContext::getRuleIndex() const {
  return CHTLParser::RuleProgram;
}

void CHTLParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void CHTLParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


std::any CHTLParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ProgramContext* CHTLParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, CHTLParser::RuleProgram);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(71);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7806) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 7) != 0)) {
      setState(69);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CHTLParser::TEXT:
        case CHTLParser::STYLE:
        case CHTLParser::SCRIPT:
        case CHTLParser::IDENTIFIER: {
          setState(60);
          element();
          break;
        }

        case CHTLParser::TEMPLATE: {
          setState(61);
          template_();
          break;
        }

        case CHTLParser::CUSTOM: {
          setState(62);
          custom();
          break;
        }

        case CHTLParser::ORIGIN: {
          setState(63);
          origin();
          break;
        }

        case CHTLParser::IMPORT: {
          setState(64);
          importStatement();
          break;
        }

        case CHTLParser::NAMESPACE: {
          setState(65);
          namespace_();
          break;
        }

        case CHTLParser::CONFIGURATION: {
          setState(66);
          configuration();
          break;
        }

        case CHTLParser::USE: {
          setState(67);
          useStatement();
          break;
        }

        case CHTLParser::COMMENT:
        case CHTLParser::GENERATOR_COMMENT: {
          setState(68);
          comment();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(73);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(74);
    match(CHTLParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ElementContext ------------------------------------------------------------------

CHTLParser::ElementContext::ElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLParser::HtmlElementContext* CHTLParser::ElementContext::htmlElement() {
  return getRuleContext<CHTLParser::HtmlElementContext>(0);
}

CHTLParser::TextElementContext* CHTLParser::ElementContext::textElement() {
  return getRuleContext<CHTLParser::TextElementContext>(0);
}

CHTLParser::StyleElementContext* CHTLParser::ElementContext::styleElement() {
  return getRuleContext<CHTLParser::StyleElementContext>(0);
}

CHTLParser::ScriptElementContext* CHTLParser::ElementContext::scriptElement() {
  return getRuleContext<CHTLParser::ScriptElementContext>(0);
}


size_t CHTLParser::ElementContext::getRuleIndex() const {
  return CHTLParser::RuleElement;
}

void CHTLParser::ElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElement(this);
}

void CHTLParser::ElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElement(this);
}


std::any CHTLParser::ElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitElement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ElementContext* CHTLParser::element() {
  ElementContext *_localctx = _tracker.createInstance<ElementContext>(_ctx, getState());
  enterRule(_localctx, 2, CHTLParser::RuleElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(80);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(76);
        htmlElement();
        break;
      }

      case CHTLParser::TEXT: {
        enterOuterAlt(_localctx, 2);
        setState(77);
        textElement();
        break;
      }

      case CHTLParser::STYLE: {
        enterOuterAlt(_localctx, 3);
        setState(78);
        styleElement();
        break;
      }

      case CHTLParser::SCRIPT: {
        enterOuterAlt(_localctx, 4);
        setState(79);
        scriptElement();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HtmlElementContext ------------------------------------------------------------------

CHTLParser::HtmlElementContext::HtmlElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::HtmlElementContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::AttributesContext* CHTLParser::HtmlElementContext::attributes() {
  return getRuleContext<CHTLParser::AttributesContext>(0);
}

CHTLParser::BlockContext* CHTLParser::HtmlElementContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::HtmlElementContext::getRuleIndex() const {
  return CHTLParser::RuleHtmlElement;
}

void CHTLParser::HtmlElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHtmlElement(this);
}

void CHTLParser::HtmlElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHtmlElement(this);
}


std::any CHTLParser::HtmlElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitHtmlElement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::HtmlElementContext* CHTLParser::htmlElement() {
  HtmlElementContext *_localctx = _tracker.createInstance<HtmlElementContext>(_ctx, getState());
  enterRule(_localctx, 4, CHTLParser::RuleHtmlElement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(82);
    match(CHTLParser::IDENTIFIER);
    setState(84);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      setState(83);
      attributes();
      break;
    }

    default:
      break;
    }
    setState(87);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLParser::LEFT_BRACE) {
      setState(86);
      block();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TextElementContext ------------------------------------------------------------------

CHTLParser::TextElementContext::TextElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::TextElementContext::TEXT() {
  return getToken(CHTLParser::TEXT, 0);
}

CHTLParser::BlockContext* CHTLParser::TextElementContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::TextElementContext::getRuleIndex() const {
  return CHTLParser::RuleTextElement;
}

void CHTLParser::TextElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTextElement(this);
}

void CHTLParser::TextElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTextElement(this);
}


std::any CHTLParser::TextElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitTextElement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::TextElementContext* CHTLParser::textElement() {
  TextElementContext *_localctx = _tracker.createInstance<TextElementContext>(_ctx, getState());
  enterRule(_localctx, 6, CHTLParser::RuleTextElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(89);
    match(CHTLParser::TEXT);
    setState(90);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StyleElementContext ------------------------------------------------------------------

CHTLParser::StyleElementContext::StyleElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::StyleElementContext::STYLE() {
  return getToken(CHTLParser::STYLE, 0);
}

CHTLParser::BlockContext* CHTLParser::StyleElementContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::StyleElementContext::getRuleIndex() const {
  return CHTLParser::RuleStyleElement;
}

void CHTLParser::StyleElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStyleElement(this);
}

void CHTLParser::StyleElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStyleElement(this);
}


std::any CHTLParser::StyleElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitStyleElement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::StyleElementContext* CHTLParser::styleElement() {
  StyleElementContext *_localctx = _tracker.createInstance<StyleElementContext>(_ctx, getState());
  enterRule(_localctx, 8, CHTLParser::RuleStyleElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(92);
    match(CHTLParser::STYLE);
    setState(93);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ScriptElementContext ------------------------------------------------------------------

CHTLParser::ScriptElementContext::ScriptElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ScriptElementContext::SCRIPT() {
  return getToken(CHTLParser::SCRIPT, 0);
}

CHTLParser::BlockContext* CHTLParser::ScriptElementContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::ScriptElementContext::getRuleIndex() const {
  return CHTLParser::RuleScriptElement;
}

void CHTLParser::ScriptElementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterScriptElement(this);
}

void CHTLParser::ScriptElementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitScriptElement(this);
}


std::any CHTLParser::ScriptElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitScriptElement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ScriptElementContext* CHTLParser::scriptElement() {
  ScriptElementContext *_localctx = _tracker.createInstance<ScriptElementContext>(_ctx, getState());
  enterRule(_localctx, 10, CHTLParser::RuleScriptElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(95);
    match(CHTLParser::SCRIPT);
    setState(96);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributesContext ------------------------------------------------------------------

CHTLParser::AttributesContext::AttributesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLParser::AttributeContext *> CHTLParser::AttributesContext::attribute() {
  return getRuleContexts<CHTLParser::AttributeContext>();
}

CHTLParser::AttributeContext* CHTLParser::AttributesContext::attribute(size_t i) {
  return getRuleContext<CHTLParser::AttributeContext>(i);
}


size_t CHTLParser::AttributesContext::getRuleIndex() const {
  return CHTLParser::RuleAttributes;
}

void CHTLParser::AttributesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttributes(this);
}

void CHTLParser::AttributesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttributes(this);
}


std::any CHTLParser::AttributesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitAttributes(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::AttributesContext* CHTLParser::attributes() {
  AttributesContext *_localctx = _tracker.createInstance<AttributesContext>(_ctx, getState());
  enterRule(_localctx, 12, CHTLParser::RuleAttributes);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(99); 
    _errHandler->sync(this);
    alt = 1;
    do {
      switch (alt) {
        case 1: {
              setState(98);
              attribute();
              break;
            }

      default:
        throw NoViableAltException(this);
      }
      setState(101); 
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributeContext ------------------------------------------------------------------

CHTLParser::AttributeContext::AttributeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::AttributeContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::ValueContext* CHTLParser::AttributeContext::value() {
  return getRuleContext<CHTLParser::ValueContext>(0);
}

tree::TerminalNode* CHTLParser::AttributeContext::COLON() {
  return getToken(CHTLParser::COLON, 0);
}

tree::TerminalNode* CHTLParser::AttributeContext::EQUAL() {
  return getToken(CHTLParser::EQUAL, 0);
}

tree::TerminalNode* CHTLParser::AttributeContext::SEMICOLON() {
  return getToken(CHTLParser::SEMICOLON, 0);
}


size_t CHTLParser::AttributeContext::getRuleIndex() const {
  return CHTLParser::RuleAttribute;
}

void CHTLParser::AttributeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttribute(this);
}

void CHTLParser::AttributeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttribute(this);
}


std::any CHTLParser::AttributeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitAttribute(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::AttributeContext* CHTLParser::attribute() {
  AttributeContext *_localctx = _tracker.createInstance<AttributeContext>(_ctx, getState());
  enterRule(_localctx, 14, CHTLParser::RuleAttribute);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
    match(CHTLParser::IDENTIFIER);
    setState(104);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::COLON

    || _la == CHTLParser::EQUAL)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(105);
    value();
    setState(107);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLParser::SEMICOLON) {
      setState(106);
      match(CHTLParser::SEMICOLON);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

CHTLParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ValueContext::STRING() {
  return getToken(CHTLParser::STRING, 0);
}

tree::TerminalNode* CHTLParser::ValueContext::UNQUOTED_LITERAL() {
  return getToken(CHTLParser::UNQUOTED_LITERAL, 0);
}

tree::TerminalNode* CHTLParser::ValueContext::NUMBER() {
  return getToken(CHTLParser::NUMBER, 0);
}

CHTLParser::ExpressionContext* CHTLParser::ValueContext::expression() {
  return getRuleContext<CHTLParser::ExpressionContext>(0);
}


size_t CHTLParser::ValueContext::getRuleIndex() const {
  return CHTLParser::RuleValue;
}

void CHTLParser::ValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue(this);
}

void CHTLParser::ValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue(this);
}


std::any CHTLParser::ValueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitValue(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ValueContext* CHTLParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 16, CHTLParser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(113);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(109);
      match(CHTLParser::STRING);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(110);
      match(CHTLParser::UNQUOTED_LITERAL);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(111);
      match(CHTLParser::NUMBER);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(112);
      expression();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

CHTLParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::BlockContext::LEFT_BRACE() {
  return getToken(CHTLParser::LEFT_BRACE, 0);
}

tree::TerminalNode* CHTLParser::BlockContext::RIGHT_BRACE() {
  return getToken(CHTLParser::RIGHT_BRACE, 0);
}

std::vector<CHTLParser::ElementContext *> CHTLParser::BlockContext::element() {
  return getRuleContexts<CHTLParser::ElementContext>();
}

CHTLParser::ElementContext* CHTLParser::BlockContext::element(size_t i) {
  return getRuleContext<CHTLParser::ElementContext>(i);
}

std::vector<CHTLParser::TextContext *> CHTLParser::BlockContext::text() {
  return getRuleContexts<CHTLParser::TextContext>();
}

CHTLParser::TextContext* CHTLParser::BlockContext::text(size_t i) {
  return getRuleContext<CHTLParser::TextContext>(i);
}

std::vector<CHTLParser::CommentContext *> CHTLParser::BlockContext::comment() {
  return getRuleContexts<CHTLParser::CommentContext>();
}

CHTLParser::CommentContext* CHTLParser::BlockContext::comment(size_t i) {
  return getRuleContext<CHTLParser::CommentContext>(i);
}


size_t CHTLParser::BlockContext::getRuleIndex() const {
  return CHTLParser::RuleBlock;
}

void CHTLParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void CHTLParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}


std::any CHTLParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::BlockContext* CHTLParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 18, CHTLParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    match(CHTLParser::LEFT_BRACE);
    setState(121);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 3584) != 0) || ((((_la - 75) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 75)) & 39) != 0)) {
      setState(119);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CHTLParser::TEXT:
        case CHTLParser::STYLE:
        case CHTLParser::SCRIPT:
        case CHTLParser::IDENTIFIER: {
          setState(116);
          element();
          break;
        }

        case CHTLParser::TEXT_CONTENT: {
          setState(117);
          text();
          break;
        }

        case CHTLParser::COMMENT:
        case CHTLParser::GENERATOR_COMMENT: {
          setState(118);
          comment();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(123);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(124);
    match(CHTLParser::RIGHT_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TextContext ------------------------------------------------------------------

CHTLParser::TextContext::TextContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::TextContext::TEXT_CONTENT() {
  return getToken(CHTLParser::TEXT_CONTENT, 0);
}


size_t CHTLParser::TextContext::getRuleIndex() const {
  return CHTLParser::RuleText;
}

void CHTLParser::TextContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterText(this);
}

void CHTLParser::TextContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitText(this);
}


std::any CHTLParser::TextContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitText(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::TextContext* CHTLParser::text() {
  TextContext *_localctx = _tracker.createInstance<TextContext>(_ctx, getState());
  enterRule(_localctx, 20, CHTLParser::RuleText);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(126);
    match(CHTLParser::TEXT_CONTENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TemplateContext ------------------------------------------------------------------

CHTLParser::TemplateContext::TemplateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::TemplateContext::TEMPLATE() {
  return getToken(CHTLParser::TEMPLATE, 0);
}

CHTLParser::TemplateTypeContext* CHTLParser::TemplateContext::templateType() {
  return getRuleContext<CHTLParser::TemplateTypeContext>(0);
}

tree::TerminalNode* CHTLParser::TemplateContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::BlockContext* CHTLParser::TemplateContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::TemplateContext::getRuleIndex() const {
  return CHTLParser::RuleTemplate;
}

void CHTLParser::TemplateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTemplate(this);
}

void CHTLParser::TemplateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTemplate(this);
}


std::any CHTLParser::TemplateContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitTemplate(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::TemplateContext* CHTLParser::template_() {
  TemplateContext *_localctx = _tracker.createInstance<TemplateContext>(_ctx, getState());
  enterRule(_localctx, 22, CHTLParser::RuleTemplate);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(128);
    match(CHTLParser::TEMPLATE);
    setState(129);
    templateType();
    setState(130);
    match(CHTLParser::IDENTIFIER);
    setState(131);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TemplateTypeContext ------------------------------------------------------------------

CHTLParser::TemplateTypeContext::TemplateTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::TemplateTypeContext::STYLE() {
  return getToken(CHTLParser::STYLE, 0);
}

tree::TerminalNode* CHTLParser::TemplateTypeContext::ELEMENT() {
  return getToken(CHTLParser::ELEMENT, 0);
}

tree::TerminalNode* CHTLParser::TemplateTypeContext::VAR() {
  return getToken(CHTLParser::VAR, 0);
}


size_t CHTLParser::TemplateTypeContext::getRuleIndex() const {
  return CHTLParser::RuleTemplateType;
}

void CHTLParser::TemplateTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTemplateType(this);
}

void CHTLParser::TemplateTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTemplateType(this);
}


std::any CHTLParser::TemplateTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitTemplateType(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::TemplateTypeContext* CHTLParser::templateType() {
  TemplateTypeContext *_localctx = _tracker.createInstance<TemplateTypeContext>(_ctx, getState());
  enterRule(_localctx, 24, CHTLParser::RuleTemplateType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(133);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::STYLE || _la == CHTLParser::ELEMENT

    || _la == CHTLParser::VAR)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CustomContext ------------------------------------------------------------------

CHTLParser::CustomContext::CustomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::CustomContext::CUSTOM() {
  return getToken(CHTLParser::CUSTOM, 0);
}

CHTLParser::CustomTypeContext* CHTLParser::CustomContext::customType() {
  return getRuleContext<CHTLParser::CustomTypeContext>(0);
}

tree::TerminalNode* CHTLParser::CustomContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::BlockContext* CHTLParser::CustomContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::CustomContext::getRuleIndex() const {
  return CHTLParser::RuleCustom;
}

void CHTLParser::CustomContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCustom(this);
}

void CHTLParser::CustomContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCustom(this);
}


std::any CHTLParser::CustomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitCustom(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::CustomContext* CHTLParser::custom() {
  CustomContext *_localctx = _tracker.createInstance<CustomContext>(_ctx, getState());
  enterRule(_localctx, 26, CHTLParser::RuleCustom);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(135);
    match(CHTLParser::CUSTOM);
    setState(136);
    customType();
    setState(137);
    match(CHTLParser::IDENTIFIER);
    setState(138);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CustomTypeContext ------------------------------------------------------------------

CHTLParser::CustomTypeContext::CustomTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::CustomTypeContext::STYLE() {
  return getToken(CHTLParser::STYLE, 0);
}

tree::TerminalNode* CHTLParser::CustomTypeContext::ELEMENT() {
  return getToken(CHTLParser::ELEMENT, 0);
}

tree::TerminalNode* CHTLParser::CustomTypeContext::VAR() {
  return getToken(CHTLParser::VAR, 0);
}


size_t CHTLParser::CustomTypeContext::getRuleIndex() const {
  return CHTLParser::RuleCustomType;
}

void CHTLParser::CustomTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCustomType(this);
}

void CHTLParser::CustomTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCustomType(this);
}


std::any CHTLParser::CustomTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitCustomType(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::CustomTypeContext* CHTLParser::customType() {
  CustomTypeContext *_localctx = _tracker.createInstance<CustomTypeContext>(_ctx, getState());
  enterRule(_localctx, 28, CHTLParser::RuleCustomType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(140);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::STYLE || _la == CHTLParser::ELEMENT

    || _la == CHTLParser::VAR)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OriginContext ------------------------------------------------------------------

CHTLParser::OriginContext::OriginContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::OriginContext::ORIGIN() {
  return getToken(CHTLParser::ORIGIN, 0);
}

CHTLParser::OriginTypeContext* CHTLParser::OriginContext::originType() {
  return getRuleContext<CHTLParser::OriginTypeContext>(0);
}

tree::TerminalNode* CHTLParser::OriginContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::BlockContext* CHTLParser::OriginContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::OriginContext::getRuleIndex() const {
  return CHTLParser::RuleOrigin;
}

void CHTLParser::OriginContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOrigin(this);
}

void CHTLParser::OriginContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOrigin(this);
}


std::any CHTLParser::OriginContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitOrigin(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::OriginContext* CHTLParser::origin() {
  OriginContext *_localctx = _tracker.createInstance<OriginContext>(_ctx, getState());
  enterRule(_localctx, 30, CHTLParser::RuleOrigin);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(142);
    match(CHTLParser::ORIGIN);
    setState(143);
    originType();
    setState(144);
    match(CHTLParser::IDENTIFIER);
    setState(145);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OriginTypeContext ------------------------------------------------------------------

CHTLParser::OriginTypeContext::OriginTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::OriginTypeContext::HTML() {
  return getToken(CHTLParser::HTML, 0);
}

tree::TerminalNode* CHTLParser::OriginTypeContext::CSS() {
  return getToken(CHTLParser::CSS, 0);
}

tree::TerminalNode* CHTLParser::OriginTypeContext::JAVASCRIPT() {
  return getToken(CHTLParser::JAVASCRIPT, 0);
}

tree::TerminalNode* CHTLParser::OriginTypeContext::VUE() {
  return getToken(CHTLParser::VUE, 0);
}


size_t CHTLParser::OriginTypeContext::getRuleIndex() const {
  return CHTLParser::RuleOriginType;
}

void CHTLParser::OriginTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOriginType(this);
}

void CHTLParser::OriginTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOriginType(this);
}


std::any CHTLParser::OriginTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitOriginType(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::OriginTypeContext* CHTLParser::originType() {
  OriginTypeContext *_localctx = _tracker.createInstance<OriginTypeContext>(_ctx, getState());
  enterRule(_localctx, 32, CHTLParser::RuleOriginType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(147);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 64424509440) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportStatementContext ------------------------------------------------------------------

CHTLParser::ImportStatementContext::ImportStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ImportStatementContext::IMPORT() {
  return getToken(CHTLParser::IMPORT, 0);
}

std::vector<tree::TerminalNode *> CHTLParser::ImportStatementContext::IDENTIFIER() {
  return getTokens(CHTLParser::IDENTIFIER);
}

tree::TerminalNode* CHTLParser::ImportStatementContext::IDENTIFIER(size_t i) {
  return getToken(CHTLParser::IDENTIFIER, i);
}

tree::TerminalNode* CHTLParser::ImportStatementContext::FROM() {
  return getToken(CHTLParser::FROM, 0);
}

CHTLParser::PathContext* CHTLParser::ImportStatementContext::path() {
  return getRuleContext<CHTLParser::PathContext>(0);
}

CHTLParser::ImportTypeContext* CHTLParser::ImportStatementContext::importType() {
  return getRuleContext<CHTLParser::ImportTypeContext>(0);
}

tree::TerminalNode* CHTLParser::ImportStatementContext::AS() {
  return getToken(CHTLParser::AS, 0);
}


size_t CHTLParser::ImportStatementContext::getRuleIndex() const {
  return CHTLParser::RuleImportStatement;
}

void CHTLParser::ImportStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImportStatement(this);
}

void CHTLParser::ImportStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImportStatement(this);
}


std::any CHTLParser::ImportStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitImportStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ImportStatementContext* CHTLParser::importStatement() {
  ImportStatementContext *_localctx = _tracker.createInstance<ImportStatementContext>(_ctx, getState());
  enterRule(_localctx, 34, CHTLParser::RuleImportStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(149);
    match(CHTLParser::IMPORT);
    setState(151);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 78) != 0)) {
      setState(150);
      importType();
    }
    setState(153);
    match(CHTLParser::IDENTIFIER);
    setState(154);
    match(CHTLParser::FROM);
    setState(155);
    path();
    setState(158);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLParser::AS) {
      setState(156);
      match(CHTLParser::AS);
      setState(157);
      match(CHTLParser::IDENTIFIER);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportTypeContext ------------------------------------------------------------------

CHTLParser::ImportTypeContext::ImportTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ImportTypeContext::TEMPLATE() {
  return getToken(CHTLParser::TEMPLATE, 0);
}

tree::TerminalNode* CHTLParser::ImportTypeContext::CUSTOM() {
  return getToken(CHTLParser::CUSTOM, 0);
}

tree::TerminalNode* CHTLParser::ImportTypeContext::ORIGIN() {
  return getToken(CHTLParser::ORIGIN, 0);
}

tree::TerminalNode* CHTLParser::ImportTypeContext::CONFIGURATION() {
  return getToken(CHTLParser::CONFIGURATION, 0);
}


size_t CHTLParser::ImportTypeContext::getRuleIndex() const {
  return CHTLParser::RuleImportType;
}

void CHTLParser::ImportTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImportType(this);
}

void CHTLParser::ImportTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImportType(this);
}


std::any CHTLParser::ImportTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitImportType(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ImportTypeContext* CHTLParser::importType() {
  ImportTypeContext *_localctx = _tracker.createInstance<ImportTypeContext>(_ctx, getState());
  enterRule(_localctx, 36, CHTLParser::RuleImportType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(160);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 78) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PathContext ------------------------------------------------------------------

CHTLParser::PathContext::PathContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::PathContext::STRING() {
  return getToken(CHTLParser::STRING, 0);
}

tree::TerminalNode* CHTLParser::PathContext::UNQUOTED_LITERAL() {
  return getToken(CHTLParser::UNQUOTED_LITERAL, 0);
}


size_t CHTLParser::PathContext::getRuleIndex() const {
  return CHTLParser::RulePath;
}

void CHTLParser::PathContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPath(this);
}

void CHTLParser::PathContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPath(this);
}


std::any CHTLParser::PathContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitPath(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::PathContext* CHTLParser::path() {
  PathContext *_localctx = _tracker.createInstance<PathContext>(_ctx, getState());
  enterRule(_localctx, 38, CHTLParser::RulePath);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(162);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::STRING

    || _la == CHTLParser::UNQUOTED_LITERAL)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NamespaceContext ------------------------------------------------------------------

CHTLParser::NamespaceContext::NamespaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::NamespaceContext::NAMESPACE() {
  return getToken(CHTLParser::NAMESPACE, 0);
}

tree::TerminalNode* CHTLParser::NamespaceContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

CHTLParser::BlockContext* CHTLParser::NamespaceContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}


size_t CHTLParser::NamespaceContext::getRuleIndex() const {
  return CHTLParser::RuleNamespace;
}

void CHTLParser::NamespaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNamespace(this);
}

void CHTLParser::NamespaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNamespace(this);
}


std::any CHTLParser::NamespaceContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitNamespace(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::NamespaceContext* CHTLParser::namespace_() {
  NamespaceContext *_localctx = _tracker.createInstance<NamespaceContext>(_ctx, getState());
  enterRule(_localctx, 40, CHTLParser::RuleNamespace);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(164);
    match(CHTLParser::NAMESPACE);
    setState(165);
    match(CHTLParser::IDENTIFIER);
    setState(166);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConfigurationContext ------------------------------------------------------------------

CHTLParser::ConfigurationContext::ConfigurationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::ConfigurationContext::CONFIGURATION() {
  return getToken(CHTLParser::CONFIGURATION, 0);
}

CHTLParser::BlockContext* CHTLParser::ConfigurationContext::block() {
  return getRuleContext<CHTLParser::BlockContext>(0);
}

tree::TerminalNode* CHTLParser::ConfigurationContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}


size_t CHTLParser::ConfigurationContext::getRuleIndex() const {
  return CHTLParser::RuleConfiguration;
}

void CHTLParser::ConfigurationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConfiguration(this);
}

void CHTLParser::ConfigurationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConfiguration(this);
}


std::any CHTLParser::ConfigurationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitConfiguration(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ConfigurationContext* CHTLParser::configuration() {
  ConfigurationContext *_localctx = _tracker.createInstance<ConfigurationContext>(_ctx, getState());
  enterRule(_localctx, 42, CHTLParser::RuleConfiguration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(168);
    match(CHTLParser::CONFIGURATION);
    setState(170);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLParser::IDENTIFIER) {
      setState(169);
      match(CHTLParser::IDENTIFIER);
    }
    setState(172);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UseStatementContext ------------------------------------------------------------------

CHTLParser::UseStatementContext::UseStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::UseStatementContext::USE() {
  return getToken(CHTLParser::USE, 0);
}

tree::TerminalNode* CHTLParser::UseStatementContext::HTML5() {
  return getToken(CHTLParser::HTML5, 0);
}

tree::TerminalNode* CHTLParser::UseStatementContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}


size_t CHTLParser::UseStatementContext::getRuleIndex() const {
  return CHTLParser::RuleUseStatement;
}

void CHTLParser::UseStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUseStatement(this);
}

void CHTLParser::UseStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUseStatement(this);
}


std::any CHTLParser::UseStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitUseStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::UseStatementContext* CHTLParser::useStatement() {
  UseStatementContext *_localctx = _tracker.createInstance<UseStatementContext>(_ctx, getState());
  enterRule(_localctx, 44, CHTLParser::RuleUseStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(174);
    match(CHTLParser::USE);
    setState(175);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::HTML5

    || _la == CHTLParser::IDENTIFIER)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CommentContext ------------------------------------------------------------------

CHTLParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::CommentContext::COMMENT() {
  return getToken(CHTLParser::COMMENT, 0);
}

tree::TerminalNode* CHTLParser::CommentContext::GENERATOR_COMMENT() {
  return getToken(CHTLParser::GENERATOR_COMMENT, 0);
}


size_t CHTLParser::CommentContext::getRuleIndex() const {
  return CHTLParser::RuleComment;
}

void CHTLParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void CHTLParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}


std::any CHTLParser::CommentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitComment(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::CommentContext* CHTLParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 46, CHTLParser::RuleComment);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(177);
    _la = _input->LA(1);
    if (!(_la == CHTLParser::COMMENT

    || _la == CHTLParser::GENERATOR_COMMENT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

CHTLParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLParser::TermContext *> CHTLParser::ExpressionContext::term() {
  return getRuleContexts<CHTLParser::TermContext>();
}

CHTLParser::TermContext* CHTLParser::ExpressionContext::term(size_t i) {
  return getRuleContext<CHTLParser::TermContext>(i);
}

std::vector<CHTLParser::OperatorContext *> CHTLParser::ExpressionContext::operator_() {
  return getRuleContexts<CHTLParser::OperatorContext>();
}

CHTLParser::OperatorContext* CHTLParser::ExpressionContext::operator_(size_t i) {
  return getRuleContext<CHTLParser::OperatorContext>(i);
}


size_t CHTLParser::ExpressionContext::getRuleIndex() const {
  return CHTLParser::RuleExpression;
}

void CHTLParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void CHTLParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}


std::any CHTLParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::ExpressionContext* CHTLParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 48, CHTLParser::RuleExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(179);
    term();
    setState(185);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4611404543450677248) != 0)) {
      setState(180);
      operator_();
      setState(181);
      term();
      setState(187);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TermContext ------------------------------------------------------------------

CHTLParser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::TermContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLParser::TermContext::STRING() {
  return getToken(CHTLParser::STRING, 0);
}

tree::TerminalNode* CHTLParser::TermContext::UNQUOTED_LITERAL() {
  return getToken(CHTLParser::UNQUOTED_LITERAL, 0);
}

tree::TerminalNode* CHTLParser::TermContext::NUMBER() {
  return getToken(CHTLParser::NUMBER, 0);
}

CHTLParser::FunctionCallContext* CHTLParser::TermContext::functionCall() {
  return getRuleContext<CHTLParser::FunctionCallContext>(0);
}

CHTLParser::PropertyAccessContext* CHTLParser::TermContext::propertyAccess() {
  return getRuleContext<CHTLParser::PropertyAccessContext>(0);
}

CHTLParser::IndexAccessContext* CHTLParser::TermContext::indexAccess() {
  return getRuleContext<CHTLParser::IndexAccessContext>(0);
}


size_t CHTLParser::TermContext::getRuleIndex() const {
  return CHTLParser::RuleTerm;
}

void CHTLParser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void CHTLParser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}


std::any CHTLParser::TermContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitTerm(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::TermContext* CHTLParser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 50, CHTLParser::RuleTerm);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(195);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(188);
      match(CHTLParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(189);
      match(CHTLParser::STRING);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(190);
      match(CHTLParser::UNQUOTED_LITERAL);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(191);
      match(CHTLParser::NUMBER);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(192);
      functionCall();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(193);
      propertyAccess();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(194);
      indexAccess();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionCallContext ------------------------------------------------------------------

CHTLParser::FunctionCallContext::FunctionCallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::FunctionCallContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLParser::FunctionCallContext::LEFT_PAREN() {
  return getToken(CHTLParser::LEFT_PAREN, 0);
}

tree::TerminalNode* CHTLParser::FunctionCallContext::RIGHT_PAREN() {
  return getToken(CHTLParser::RIGHT_PAREN, 0);
}

std::vector<CHTLParser::ExpressionContext *> CHTLParser::FunctionCallContext::expression() {
  return getRuleContexts<CHTLParser::ExpressionContext>();
}

CHTLParser::ExpressionContext* CHTLParser::FunctionCallContext::expression(size_t i) {
  return getRuleContext<CHTLParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLParser::FunctionCallContext::COMMA() {
  return getTokens(CHTLParser::COMMA);
}

tree::TerminalNode* CHTLParser::FunctionCallContext::COMMA(size_t i) {
  return getToken(CHTLParser::COMMA, i);
}


size_t CHTLParser::FunctionCallContext::getRuleIndex() const {
  return CHTLParser::RuleFunctionCall;
}

void CHTLParser::FunctionCallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionCall(this);
}

void CHTLParser::FunctionCallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionCall(this);
}


std::any CHTLParser::FunctionCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitFunctionCall(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::FunctionCallContext* CHTLParser::functionCall() {
  FunctionCallContext *_localctx = _tracker.createInstance<FunctionCallContext>(_ctx, getState());
  enterRule(_localctx, 52, CHTLParser::RuleFunctionCall);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(197);
    match(CHTLParser::IDENTIFIER);
    setState(198);
    match(CHTLParser::LEFT_PAREN);
    setState(207);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 72) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 72)) & 15) != 0)) {
      setState(199);
      expression();
      setState(204);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == CHTLParser::COMMA) {
        setState(200);
        match(CHTLParser::COMMA);
        setState(201);
        expression();
        setState(206);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(209);
    match(CHTLParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyAccessContext ------------------------------------------------------------------

CHTLParser::PropertyAccessContext::PropertyAccessContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CHTLParser::PropertyAccessContext::IDENTIFIER() {
  return getTokens(CHTLParser::IDENTIFIER);
}

tree::TerminalNode* CHTLParser::PropertyAccessContext::IDENTIFIER(size_t i) {
  return getToken(CHTLParser::IDENTIFIER, i);
}

tree::TerminalNode* CHTLParser::PropertyAccessContext::DOT() {
  return getToken(CHTLParser::DOT, 0);
}


size_t CHTLParser::PropertyAccessContext::getRuleIndex() const {
  return CHTLParser::RulePropertyAccess;
}

void CHTLParser::PropertyAccessContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAccess(this);
}

void CHTLParser::PropertyAccessContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAccess(this);
}


std::any CHTLParser::PropertyAccessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitPropertyAccess(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::PropertyAccessContext* CHTLParser::propertyAccess() {
  PropertyAccessContext *_localctx = _tracker.createInstance<PropertyAccessContext>(_ctx, getState());
  enterRule(_localctx, 54, CHTLParser::RulePropertyAccess);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    match(CHTLParser::IDENTIFIER);
    setState(212);
    match(CHTLParser::DOT);
    setState(213);
    match(CHTLParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IndexAccessContext ------------------------------------------------------------------

CHTLParser::IndexAccessContext::IndexAccessContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::IndexAccessContext::IDENTIFIER() {
  return getToken(CHTLParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLParser::IndexAccessContext::LEFT_BRACKET() {
  return getToken(CHTLParser::LEFT_BRACKET, 0);
}

CHTLParser::ExpressionContext* CHTLParser::IndexAccessContext::expression() {
  return getRuleContext<CHTLParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLParser::IndexAccessContext::RIGHT_BRACKET() {
  return getToken(CHTLParser::RIGHT_BRACKET, 0);
}


size_t CHTLParser::IndexAccessContext::getRuleIndex() const {
  return CHTLParser::RuleIndexAccess;
}

void CHTLParser::IndexAccessContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIndexAccess(this);
}

void CHTLParser::IndexAccessContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIndexAccess(this);
}


std::any CHTLParser::IndexAccessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitIndexAccess(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::IndexAccessContext* CHTLParser::indexAccess() {
  IndexAccessContext *_localctx = _tracker.createInstance<IndexAccessContext>(_ctx, getState());
  enterRule(_localctx, 56, CHTLParser::RuleIndexAccess);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(215);
    match(CHTLParser::IDENTIFIER);
    setState(216);
    match(CHTLParser::LEFT_BRACKET);
    setState(217);
    expression();
    setState(218);
    match(CHTLParser::RIGHT_BRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperatorContext ------------------------------------------------------------------

CHTLParser::OperatorContext::OperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLParser::OperatorContext::PLUS() {
  return getToken(CHTLParser::PLUS, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::MINUS() {
  return getToken(CHTLParser::MINUS, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::MULTIPLY() {
  return getToken(CHTLParser::MULTIPLY, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::DIVIDE() {
  return getToken(CHTLParser::DIVIDE, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::MODULO() {
  return getToken(CHTLParser::MODULO, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::AND() {
  return getToken(CHTLParser::AND, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::OR() {
  return getToken(CHTLParser::OR, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::NOT() {
  return getToken(CHTLParser::NOT, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::GREATER() {
  return getToken(CHTLParser::GREATER, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::LESS() {
  return getToken(CHTLParser::LESS, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::GREATER_EQUAL() {
  return getToken(CHTLParser::GREATER_EQUAL, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::LESS_EQUAL() {
  return getToken(CHTLParser::LESS_EQUAL, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::EQUAL_EQUAL() {
  return getToken(CHTLParser::EQUAL_EQUAL, 0);
}

tree::TerminalNode* CHTLParser::OperatorContext::NOT_EQUAL() {
  return getToken(CHTLParser::NOT_EQUAL, 0);
}


size_t CHTLParser::OperatorContext::getRuleIndex() const {
  return CHTLParser::RuleOperator;
}

void CHTLParser::OperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperator(this);
}

void CHTLParser::OperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperator(this);
}


std::any CHTLParser::OperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLVisitor*>(visitor))
    return parserVisitor->visitOperator(this);
  else
    return visitor->visitChildren(this);
}

CHTLParser::OperatorContext* CHTLParser::operator_() {
  OperatorContext *_localctx = _tracker.createInstance<OperatorContext>(_ctx, getState());
  enterRule(_localctx, 58, CHTLParser::RuleOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(220);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4611404543450677248) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void CHTLParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  chtlParserInitialize();
#else
  ::antlr4::internal::call_once(chtlParserOnceFlag, chtlParserInitialize);
#endif
}
