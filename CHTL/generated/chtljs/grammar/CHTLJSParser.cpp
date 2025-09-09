
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1


#include "CHTLJSListener.h"
#include "CHTLJSVisitor.h"

#include "CHTLJSParser.h"


using namespace antlrcpp;
using namespace chtl.antlr;

using namespace antlr4;

namespace {

struct CHTLJSParserStaticData final {
  CHTLJSParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CHTLJSParserStaticData(const CHTLJSParserStaticData&) = delete;
  CHTLJSParserStaticData(CHTLJSParserStaticData&&) = delete;
  CHTLJSParserStaticData& operator=(const CHTLJSParserStaticData&) = delete;
  CHTLJSParserStaticData& operator=(CHTLJSParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag chtljsParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
CHTLJSParserStaticData *chtljsParserStaticData = nullptr;

void chtljsParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (chtljsParserStaticData != nullptr) {
    return;
  }
#else
  assert(chtljsParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CHTLJSParserStaticData>(
    std::vector<std::string>{
      "program", "statement", "expressionStatement", "declarationStatement", 
      "controlStatement", "chthljsStatement", "expression", "assignmentExpression", 
      "conditionalExpression", "logicalOrExpression", "logicalAndExpression", 
      "equalityExpression", "relationalExpression", "additiveExpression", 
      "multiplicativeExpression", "unaryExpression", "postfixExpression", 
      "primaryExpression", "literal", "identifier", "chthljsSelector", "selectorContent", 
      "selector", "property", "index", "functionExpression", "parameterList", 
      "objectExpression", "propertyAssignment", "propertyName", "arrayExpression", 
      "parenthesizedExpression", "variableDeclaration", "variableDeclarator", 
      "functionDeclaration", "ifStatement", "forStatement", "whileStatement", 
      "switchStatement", "caseClause", "defaultClause", "fileloaderStatement", 
      "listenStatement", "delegateStatement", "animateStatement", "virStatement", 
      "routerStatement", "utilStatement", "block", "argumentList", "assignmentOperator"
    },
    std::vector<std::string>{
      "", "'var'", "'let'", "'const'", "'function'", "'if'", "'else'", "'for'", 
      "'while'", "'switch'", "'case'", "'default'", "'break'", "'continue'", 
      "'return'", "'then'", "'fileloader'", "'listen'", "'delegate'", "'animate'", 
      "'vir'", "'router'", "'util'", "'iNeverAway'", "'{'", "'}'", "'['", 
      "']'", "'('", "')'", "';'", "','", "'.'", "':'", "'\\u003F'", "'+'", 
      "'-'", "", "'/'", "'%'", "'&&'", "'||'", "'!'", "'>'", "'<'", "'>='", 
      "'<='", "'=='", "'!='", "'+='", "'-='", "'*='", "'/='", "'%='", "'#'", 
      "", "", "", "", "", "'null'", "'undefined'"
    },
    std::vector<std::string>{
      "", "VAR", "LET", "CONST", "FUNCTION", "IF", "ELSE", "FOR", "WHILE", 
      "SWITCH", "CASE", "DEFAULT", "BREAK", "CONTINUE", "RETURN", "THEN", 
      "FILELOADER", "LISTEN", "DELEGATE", "ANIMATE", "VIR", "ROUTER", "UTIL", 
      "INEVERAWAY", "LEFT_BRACE", "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET", 
      "LEFT_PAREN", "RIGHT_PAREN", "SEMICOLON", "COMMA", "DOT", "COLON", 
      "QUESTION", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULO", "AND", 
      "OR", "NOT", "GREATER", "LESS", "GREATER_EQUAL", "LESS_EQUAL", "EQUAL_EQUAL", 
      "NOT_EQUAL", "PLUS_EQUAL", "MINUS_EQUAL", "MULTIPLY_EQUAL", "DIVIDE_EQUAL", 
      "MODULO_EQUAL", "HASH", "ASTERISK", "DOUBLE_BRACE", "STRING", "NUMBER", 
      "BOOLEAN", "NULL", "UNDEFINED", "IDENTIFIER", "COMMENT", "MULTILINE_COMMENT", 
      "WHITESPACE", "TILDE", "EQUAL"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,67,489,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,1,0,5,0,104,8,0,10,0,12,0,107,9,0,1,0,1,0,1,1,1,1,1,1,1,
  	1,3,1,115,8,1,1,2,1,2,3,2,119,8,2,1,3,1,3,3,3,123,8,3,1,4,1,4,1,4,1,4,
  	3,4,129,8,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,3,5,138,8,5,1,6,1,6,1,7,1,7,1,
  	7,1,7,3,7,146,8,7,1,8,1,8,1,8,1,8,1,8,1,8,3,8,154,8,8,1,9,1,9,1,9,5,9,
  	159,8,9,10,9,12,9,162,9,9,1,10,1,10,1,10,5,10,167,8,10,10,10,12,10,170,
  	9,10,1,11,1,11,1,11,5,11,175,8,11,10,11,12,11,178,9,11,1,12,1,12,1,12,
  	5,12,183,8,12,10,12,12,12,186,9,12,1,13,1,13,1,13,5,13,191,8,13,10,13,
  	12,13,194,9,13,1,14,1,14,1,14,5,14,199,8,14,10,14,12,14,202,9,14,1,15,
  	1,15,1,15,3,15,207,8,15,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,
  	3,16,218,8,16,1,16,5,16,221,8,16,10,16,12,16,224,9,16,1,17,1,17,1,17,
  	1,17,1,17,1,17,1,17,3,17,233,8,17,1,18,1,18,1,19,1,19,1,20,1,20,1,20,
  	1,20,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,3,21,253,8,21,
  	1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,1,22,3,22,265,8,22,1,23,
  	1,23,1,24,1,24,1,25,1,25,1,25,3,25,274,8,25,1,25,1,25,1,25,1,26,1,26,
  	1,26,5,26,282,8,26,10,26,12,26,285,9,26,1,27,1,27,1,27,1,27,5,27,291,
  	8,27,10,27,12,27,294,9,27,3,27,296,8,27,1,27,1,27,1,28,1,28,1,28,1,28,
  	1,29,1,29,1,29,1,29,1,29,1,29,3,29,310,8,29,1,30,1,30,1,30,1,30,5,30,
  	316,8,30,10,30,12,30,319,9,30,3,30,321,8,30,1,30,1,30,1,31,1,31,1,31,
  	1,31,1,32,1,32,1,32,1,32,5,32,333,8,32,10,32,12,32,336,9,32,1,33,1,33,
  	1,33,3,33,341,8,33,1,34,1,34,1,34,1,34,3,34,347,8,34,1,34,1,34,1,34,1,
  	35,1,35,1,35,1,35,1,35,1,35,1,35,3,35,359,8,35,1,36,1,36,1,36,1,36,1,
  	36,3,36,366,8,36,1,36,3,36,369,8,36,1,36,1,36,3,36,373,8,36,1,36,1,36,
  	1,36,1,37,1,37,1,37,1,37,1,37,1,37,1,38,1,38,1,38,1,38,1,38,1,38,5,38,
  	390,8,38,10,38,12,38,393,9,38,1,38,3,38,396,8,38,1,38,1,38,1,39,1,39,
  	1,39,1,39,5,39,404,8,39,10,39,12,39,407,9,39,1,40,1,40,1,40,5,40,412,
  	8,40,10,40,12,40,415,9,40,1,41,1,41,1,41,1,41,1,41,5,41,422,8,41,10,41,
  	12,41,425,9,41,1,41,1,41,1,41,1,42,1,42,1,42,1,42,1,42,1,42,1,42,1,43,
  	1,43,1,43,1,43,1,43,1,43,1,43,1,43,1,43,1,44,1,44,1,44,1,44,1,44,1,44,
  	1,44,1,45,1,45,1,45,1,45,1,45,1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,47,
  	1,47,1,47,1,47,1,47,1,48,1,48,5,48,472,8,48,10,48,12,48,475,9,48,1,48,
  	1,48,1,49,1,49,1,49,5,49,482,8,49,10,49,12,49,485,9,49,1,50,1,50,1,50,
  	0,0,51,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,
  	46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,
  	92,94,96,98,100,0,9,1,0,47,48,1,0,43,46,1,0,35,36,1,0,37,39,3,0,35,36,
  	42,42,66,66,1,0,57,61,2,0,58,58,62,62,1,0,1,3,2,0,49,53,67,67,500,0,105,
  	1,0,0,0,2,114,1,0,0,0,4,116,1,0,0,0,6,122,1,0,0,0,8,128,1,0,0,0,10,137,
  	1,0,0,0,12,139,1,0,0,0,14,141,1,0,0,0,16,147,1,0,0,0,18,155,1,0,0,0,20,
  	163,1,0,0,0,22,171,1,0,0,0,24,179,1,0,0,0,26,187,1,0,0,0,28,195,1,0,0,
  	0,30,206,1,0,0,0,32,208,1,0,0,0,34,232,1,0,0,0,36,234,1,0,0,0,38,236,
  	1,0,0,0,40,238,1,0,0,0,42,252,1,0,0,0,44,264,1,0,0,0,46,266,1,0,0,0,48,
  	268,1,0,0,0,50,270,1,0,0,0,52,278,1,0,0,0,54,286,1,0,0,0,56,299,1,0,0,
  	0,58,309,1,0,0,0,60,311,1,0,0,0,62,324,1,0,0,0,64,328,1,0,0,0,66,337,
  	1,0,0,0,68,342,1,0,0,0,70,351,1,0,0,0,72,360,1,0,0,0,74,377,1,0,0,0,76,
  	383,1,0,0,0,78,399,1,0,0,0,80,408,1,0,0,0,82,416,1,0,0,0,84,429,1,0,0,
  	0,86,436,1,0,0,0,88,445,1,0,0,0,90,452,1,0,0,0,92,457,1,0,0,0,94,464,
  	1,0,0,0,96,469,1,0,0,0,98,478,1,0,0,0,100,486,1,0,0,0,102,104,3,2,1,0,
  	103,102,1,0,0,0,104,107,1,0,0,0,105,103,1,0,0,0,105,106,1,0,0,0,106,108,
  	1,0,0,0,107,105,1,0,0,0,108,109,5,0,0,1,109,1,1,0,0,0,110,115,3,4,2,0,
  	111,115,3,6,3,0,112,115,3,8,4,0,113,115,3,10,5,0,114,110,1,0,0,0,114,
  	111,1,0,0,0,114,112,1,0,0,0,114,113,1,0,0,0,115,3,1,0,0,0,116,118,3,12,
  	6,0,117,119,5,30,0,0,118,117,1,0,0,0,118,119,1,0,0,0,119,5,1,0,0,0,120,
  	123,3,64,32,0,121,123,3,68,34,0,122,120,1,0,0,0,122,121,1,0,0,0,123,7,
  	1,0,0,0,124,129,3,70,35,0,125,129,3,72,36,0,126,129,3,74,37,0,127,129,
  	3,76,38,0,128,124,1,0,0,0,128,125,1,0,0,0,128,126,1,0,0,0,128,127,1,0,
  	0,0,129,9,1,0,0,0,130,138,3,82,41,0,131,138,3,84,42,0,132,138,3,86,43,
  	0,133,138,3,88,44,0,134,138,3,90,45,0,135,138,3,92,46,0,136,138,3,94,
  	47,0,137,130,1,0,0,0,137,131,1,0,0,0,137,132,1,0,0,0,137,133,1,0,0,0,
  	137,134,1,0,0,0,137,135,1,0,0,0,137,136,1,0,0,0,138,11,1,0,0,0,139,140,
  	3,14,7,0,140,13,1,0,0,0,141,145,3,16,8,0,142,143,3,100,50,0,143,144,3,
  	14,7,0,144,146,1,0,0,0,145,142,1,0,0,0,145,146,1,0,0,0,146,15,1,0,0,0,
  	147,153,3,18,9,0,148,149,5,34,0,0,149,150,3,12,6,0,150,151,5,33,0,0,151,
  	152,3,16,8,0,152,154,1,0,0,0,153,148,1,0,0,0,153,154,1,0,0,0,154,17,1,
  	0,0,0,155,160,3,20,10,0,156,157,5,41,0,0,157,159,3,20,10,0,158,156,1,
  	0,0,0,159,162,1,0,0,0,160,158,1,0,0,0,160,161,1,0,0,0,161,19,1,0,0,0,
  	162,160,1,0,0,0,163,168,3,22,11,0,164,165,5,40,0,0,165,167,3,22,11,0,
  	166,164,1,0,0,0,167,170,1,0,0,0,168,166,1,0,0,0,168,169,1,0,0,0,169,21,
  	1,0,0,0,170,168,1,0,0,0,171,176,3,24,12,0,172,173,7,0,0,0,173,175,3,24,
  	12,0,174,172,1,0,0,0,175,178,1,0,0,0,176,174,1,0,0,0,176,177,1,0,0,0,
  	177,23,1,0,0,0,178,176,1,0,0,0,179,184,3,26,13,0,180,181,7,1,0,0,181,
  	183,3,26,13,0,182,180,1,0,0,0,183,186,1,0,0,0,184,182,1,0,0,0,184,185,
  	1,0,0,0,185,25,1,0,0,0,186,184,1,0,0,0,187,192,3,28,14,0,188,189,7,2,
  	0,0,189,191,3,28,14,0,190,188,1,0,0,0,191,194,1,0,0,0,192,190,1,0,0,0,
  	192,193,1,0,0,0,193,27,1,0,0,0,194,192,1,0,0,0,195,200,3,30,15,0,196,
  	197,7,3,0,0,197,199,3,30,15,0,198,196,1,0,0,0,199,202,1,0,0,0,200,198,
  	1,0,0,0,200,201,1,0,0,0,201,29,1,0,0,0,202,200,1,0,0,0,203,204,7,4,0,
  	0,204,207,3,30,15,0,205,207,3,32,16,0,206,203,1,0,0,0,206,205,1,0,0,0,
  	207,31,1,0,0,0,208,222,3,34,17,0,209,210,5,26,0,0,210,211,3,12,6,0,211,
  	212,5,27,0,0,212,221,1,0,0,0,213,214,5,32,0,0,214,221,5,62,0,0,215,217,
  	5,28,0,0,216,218,3,98,49,0,217,216,1,0,0,0,217,218,1,0,0,0,218,219,1,
  	0,0,0,219,221,5,29,0,0,220,209,1,0,0,0,220,213,1,0,0,0,220,215,1,0,0,
  	0,221,224,1,0,0,0,222,220,1,0,0,0,222,223,1,0,0,0,223,33,1,0,0,0,224,
  	222,1,0,0,0,225,233,3,36,18,0,226,233,3,38,19,0,227,233,3,40,20,0,228,
  	233,3,50,25,0,229,233,3,54,27,0,230,233,3,60,30,0,231,233,3,62,31,0,232,
  	225,1,0,0,0,232,226,1,0,0,0,232,227,1,0,0,0,232,228,1,0,0,0,232,229,1,
  	0,0,0,232,230,1,0,0,0,232,231,1,0,0,0,233,35,1,0,0,0,234,235,7,5,0,0,
  	235,37,1,0,0,0,236,237,5,62,0,0,237,39,1,0,0,0,238,239,5,56,0,0,239,240,
  	3,42,21,0,240,241,5,56,0,0,241,41,1,0,0,0,242,253,3,44,22,0,243,244,3,
  	44,22,0,244,245,5,32,0,0,245,246,3,46,23,0,246,253,1,0,0,0,247,248,3,
  	44,22,0,248,249,5,26,0,0,249,250,3,48,24,0,250,251,5,27,0,0,251,253,1,
  	0,0,0,252,242,1,0,0,0,252,243,1,0,0,0,252,247,1,0,0,0,253,43,1,0,0,0,
  	254,265,5,62,0,0,255,256,5,54,0,0,256,265,5,62,0,0,257,258,5,32,0,0,258,
  	265,5,62,0,0,259,265,5,55,0,0,260,261,5,62,0,0,261,262,5,26,0,0,262,263,
  	5,58,0,0,263,265,5,27,0,0,264,254,1,0,0,0,264,255,1,0,0,0,264,257,1,0,
  	0,0,264,259,1,0,0,0,264,260,1,0,0,0,265,45,1,0,0,0,266,267,5,62,0,0,267,
  	47,1,0,0,0,268,269,7,6,0,0,269,49,1,0,0,0,270,271,5,4,0,0,271,273,5,28,
  	0,0,272,274,3,52,26,0,273,272,1,0,0,0,273,274,1,0,0,0,274,275,1,0,0,0,
  	275,276,5,29,0,0,276,277,3,96,48,0,277,51,1,0,0,0,278,283,5,62,0,0,279,
  	280,5,31,0,0,280,282,5,62,0,0,281,279,1,0,0,0,282,285,1,0,0,0,283,281,
  	1,0,0,0,283,284,1,0,0,0,284,53,1,0,0,0,285,283,1,0,0,0,286,295,5,24,0,
  	0,287,292,3,56,28,0,288,289,5,31,0,0,289,291,3,56,28,0,290,288,1,0,0,
  	0,291,294,1,0,0,0,292,290,1,0,0,0,292,293,1,0,0,0,293,296,1,0,0,0,294,
  	292,1,0,0,0,295,287,1,0,0,0,295,296,1,0,0,0,296,297,1,0,0,0,297,298,5,
  	25,0,0,298,55,1,0,0,0,299,300,3,58,29,0,300,301,5,33,0,0,301,302,3,12,
  	6,0,302,57,1,0,0,0,303,310,5,62,0,0,304,310,5,57,0,0,305,306,5,26,0,0,
  	306,307,3,12,6,0,307,308,5,27,0,0,308,310,1,0,0,0,309,303,1,0,0,0,309,
  	304,1,0,0,0,309,305,1,0,0,0,310,59,1,0,0,0,311,320,5,26,0,0,312,317,3,
  	12,6,0,313,314,5,31,0,0,314,316,3,12,6,0,315,313,1,0,0,0,316,319,1,0,
  	0,0,317,315,1,0,0,0,317,318,1,0,0,0,318,321,1,0,0,0,319,317,1,0,0,0,320,
  	312,1,0,0,0,320,321,1,0,0,0,321,322,1,0,0,0,322,323,5,27,0,0,323,61,1,
  	0,0,0,324,325,5,28,0,0,325,326,3,12,6,0,326,327,5,29,0,0,327,63,1,0,0,
  	0,328,329,7,7,0,0,329,334,3,66,33,0,330,331,5,31,0,0,331,333,3,66,33,
  	0,332,330,1,0,0,0,333,336,1,0,0,0,334,332,1,0,0,0,334,335,1,0,0,0,335,
  	65,1,0,0,0,336,334,1,0,0,0,337,340,5,62,0,0,338,339,5,67,0,0,339,341,
  	3,12,6,0,340,338,1,0,0,0,340,341,1,0,0,0,341,67,1,0,0,0,342,343,5,4,0,
  	0,343,344,5,62,0,0,344,346,5,28,0,0,345,347,3,52,26,0,346,345,1,0,0,0,
  	346,347,1,0,0,0,347,348,1,0,0,0,348,349,5,29,0,0,349,350,3,96,48,0,350,
  	69,1,0,0,0,351,352,5,5,0,0,352,353,5,28,0,0,353,354,3,12,6,0,354,355,
  	5,29,0,0,355,358,3,2,1,0,356,357,5,6,0,0,357,359,3,2,1,0,358,356,1,0,
  	0,0,358,359,1,0,0,0,359,71,1,0,0,0,360,361,5,7,0,0,361,365,5,28,0,0,362,
  	366,3,64,32,0,363,366,3,4,2,0,364,366,5,30,0,0,365,362,1,0,0,0,365,363,
  	1,0,0,0,365,364,1,0,0,0,366,368,1,0,0,0,367,369,3,12,6,0,368,367,1,0,
  	0,0,368,369,1,0,0,0,369,370,1,0,0,0,370,372,5,30,0,0,371,373,3,12,6,0,
  	372,371,1,0,0,0,372,373,1,0,0,0,373,374,1,0,0,0,374,375,5,29,0,0,375,
  	376,3,2,1,0,376,73,1,0,0,0,377,378,5,8,0,0,378,379,5,28,0,0,379,380,3,
  	12,6,0,380,381,5,29,0,0,381,382,3,2,1,0,382,75,1,0,0,0,383,384,5,9,0,
  	0,384,385,5,28,0,0,385,386,3,12,6,0,386,387,5,29,0,0,387,391,5,24,0,0,
  	388,390,3,78,39,0,389,388,1,0,0,0,390,393,1,0,0,0,391,389,1,0,0,0,391,
  	392,1,0,0,0,392,395,1,0,0,0,393,391,1,0,0,0,394,396,3,80,40,0,395,394,
  	1,0,0,0,395,396,1,0,0,0,396,397,1,0,0,0,397,398,5,25,0,0,398,77,1,0,0,
  	0,399,400,5,10,0,0,400,401,3,12,6,0,401,405,5,33,0,0,402,404,3,2,1,0,
  	403,402,1,0,0,0,404,407,1,0,0,0,405,403,1,0,0,0,405,406,1,0,0,0,406,79,
  	1,0,0,0,407,405,1,0,0,0,408,409,5,11,0,0,409,413,5,33,0,0,410,412,3,2,
  	1,0,411,410,1,0,0,0,412,415,1,0,0,0,413,411,1,0,0,0,413,414,1,0,0,0,414,
  	81,1,0,0,0,415,413,1,0,0,0,416,417,5,16,0,0,417,418,5,28,0,0,418,423,
  	5,57,0,0,419,420,5,31,0,0,420,422,5,57,0,0,421,419,1,0,0,0,422,425,1,
  	0,0,0,423,421,1,0,0,0,423,424,1,0,0,0,424,426,1,0,0,0,425,423,1,0,0,0,
  	426,427,5,29,0,0,427,428,3,96,48,0,428,83,1,0,0,0,429,430,5,17,0,0,430,
  	431,5,28,0,0,431,432,3,40,20,0,432,433,5,31,0,0,433,434,3,50,25,0,434,
  	435,5,29,0,0,435,85,1,0,0,0,436,437,5,18,0,0,437,438,5,28,0,0,438,439,
  	3,40,20,0,439,440,5,31,0,0,440,441,5,57,0,0,441,442,5,31,0,0,442,443,
  	3,50,25,0,443,444,5,29,0,0,444,87,1,0,0,0,445,446,5,19,0,0,446,447,5,
  	28,0,0,447,448,3,40,20,0,448,449,5,31,0,0,449,450,3,50,25,0,450,451,5,
  	29,0,0,451,89,1,0,0,0,452,453,5,20,0,0,453,454,5,28,0,0,454,455,3,40,
  	20,0,455,456,5,29,0,0,456,91,1,0,0,0,457,458,5,21,0,0,458,459,5,28,0,
  	0,459,460,5,57,0,0,460,461,5,31,0,0,461,462,3,50,25,0,462,463,5,29,0,
  	0,463,93,1,0,0,0,464,465,5,22,0,0,465,466,3,12,6,0,466,467,5,15,0,0,467,
  	468,3,2,1,0,468,95,1,0,0,0,469,473,5,24,0,0,470,472,3,2,1,0,471,470,1,
  	0,0,0,472,475,1,0,0,0,473,471,1,0,0,0,473,474,1,0,0,0,474,476,1,0,0,0,
  	475,473,1,0,0,0,476,477,5,25,0,0,477,97,1,0,0,0,478,483,3,12,6,0,479,
  	480,5,31,0,0,480,482,3,12,6,0,481,479,1,0,0,0,482,485,1,0,0,0,483,481,
  	1,0,0,0,483,484,1,0,0,0,484,99,1,0,0,0,485,483,1,0,0,0,486,487,7,8,0,
  	0,487,101,1,0,0,0,42,105,114,118,122,128,137,145,153,160,168,176,184,
  	192,200,206,217,220,222,232,252,264,273,283,292,295,309,317,320,334,340,
  	346,358,365,368,372,391,395,405,413,423,473,483
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  chtljsParserStaticData = staticData.release();
}

}

CHTLJSParser::CHTLJSParser(TokenStream *input) : CHTLJSParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CHTLJSParser::CHTLJSParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CHTLJSParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *chtljsParserStaticData->atn, chtljsParserStaticData->decisionToDFA, chtljsParserStaticData->sharedContextCache, options);
}

CHTLJSParser::~CHTLJSParser() {
  delete _interpreter;
}

const atn::ATN& CHTLJSParser::getATN() const {
  return *chtljsParserStaticData->atn;
}

std::string CHTLJSParser::getGrammarFileName() const {
  return "CHTLJS.g4";
}

const std::vector<std::string>& CHTLJSParser::getRuleNames() const {
  return chtljsParserStaticData->ruleNames;
}

const dfa::Vocabulary& CHTLJSParser::getVocabulary() const {
  return chtljsParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CHTLJSParser::getSerializedATN() const {
  return chtljsParserStaticData->serializedATN;
}


//----------------- ProgramContext ------------------------------------------------------------------

CHTLJSParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ProgramContext::EOF() {
  return getToken(CHTLJSParser::EOF, 0);
}

std::vector<CHTLJSParser::StatementContext *> CHTLJSParser::ProgramContext::statement() {
  return getRuleContexts<CHTLJSParser::StatementContext>();
}

CHTLJSParser::StatementContext* CHTLJSParser::ProgramContext::statement(size_t i) {
  return getRuleContext<CHTLJSParser::StatementContext>(i);
}


size_t CHTLJSParser::ProgramContext::getRuleIndex() const {
  return CHTLJSParser::RuleProgram;
}

void CHTLJSParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void CHTLJSParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


std::any CHTLJSParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ProgramContext* CHTLJSParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, CHTLJSParser::RuleProgram);
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
    setState(105);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 9151318944303219646) != 0) || _la == CHTLJSParser::TILDE) {
      setState(102);
      statement();
      setState(107);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(108);
    match(CHTLJSParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

CHTLJSParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::ExpressionStatementContext* CHTLJSParser::StatementContext::expressionStatement() {
  return getRuleContext<CHTLJSParser::ExpressionStatementContext>(0);
}

CHTLJSParser::DeclarationStatementContext* CHTLJSParser::StatementContext::declarationStatement() {
  return getRuleContext<CHTLJSParser::DeclarationStatementContext>(0);
}

CHTLJSParser::ControlStatementContext* CHTLJSParser::StatementContext::controlStatement() {
  return getRuleContext<CHTLJSParser::ControlStatementContext>(0);
}

CHTLJSParser::ChthljsStatementContext* CHTLJSParser::StatementContext::chthljsStatement() {
  return getRuleContext<CHTLJSParser::ChthljsStatementContext>(0);
}


size_t CHTLJSParser::StatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleStatement;
}

void CHTLJSParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void CHTLJSParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}


std::any CHTLJSParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::StatementContext* CHTLJSParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 2, CHTLJSParser::RuleStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(114);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(110);
      expressionStatement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(111);
      declarationStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(112);
      controlStatement();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(113);
      chthljsStatement();
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

//----------------- ExpressionStatementContext ------------------------------------------------------------------

CHTLJSParser::ExpressionStatementContext::ExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ExpressionStatementContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::ExpressionStatementContext::SEMICOLON() {
  return getToken(CHTLJSParser::SEMICOLON, 0);
}


size_t CHTLJSParser::ExpressionStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleExpressionStatement;
}

void CHTLJSParser::ExpressionStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionStatement(this);
}

void CHTLJSParser::ExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionStatement(this);
}


std::any CHTLJSParser::ExpressionStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitExpressionStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ExpressionStatementContext* CHTLJSParser::expressionStatement() {
  ExpressionStatementContext *_localctx = _tracker.createInstance<ExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 4, CHTLJSParser::RuleExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(116);
    expression();
    setState(118);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      setState(117);
      match(CHTLJSParser::SEMICOLON);
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

//----------------- DeclarationStatementContext ------------------------------------------------------------------

CHTLJSParser::DeclarationStatementContext::DeclarationStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::VariableDeclarationContext* CHTLJSParser::DeclarationStatementContext::variableDeclaration() {
  return getRuleContext<CHTLJSParser::VariableDeclarationContext>(0);
}

CHTLJSParser::FunctionDeclarationContext* CHTLJSParser::DeclarationStatementContext::functionDeclaration() {
  return getRuleContext<CHTLJSParser::FunctionDeclarationContext>(0);
}


size_t CHTLJSParser::DeclarationStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleDeclarationStatement;
}

void CHTLJSParser::DeclarationStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclarationStatement(this);
}

void CHTLJSParser::DeclarationStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclarationStatement(this);
}


std::any CHTLJSParser::DeclarationStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitDeclarationStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::DeclarationStatementContext* CHTLJSParser::declarationStatement() {
  DeclarationStatementContext *_localctx = _tracker.createInstance<DeclarationStatementContext>(_ctx, getState());
  enterRule(_localctx, 6, CHTLJSParser::RuleDeclarationStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(122);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::VAR:
      case CHTLJSParser::LET:
      case CHTLJSParser::CONST: {
        enterOuterAlt(_localctx, 1);
        setState(120);
        variableDeclaration();
        break;
      }

      case CHTLJSParser::FUNCTION: {
        enterOuterAlt(_localctx, 2);
        setState(121);
        functionDeclaration();
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

//----------------- ControlStatementContext ------------------------------------------------------------------

CHTLJSParser::ControlStatementContext::ControlStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::IfStatementContext* CHTLJSParser::ControlStatementContext::ifStatement() {
  return getRuleContext<CHTLJSParser::IfStatementContext>(0);
}

CHTLJSParser::ForStatementContext* CHTLJSParser::ControlStatementContext::forStatement() {
  return getRuleContext<CHTLJSParser::ForStatementContext>(0);
}

CHTLJSParser::WhileStatementContext* CHTLJSParser::ControlStatementContext::whileStatement() {
  return getRuleContext<CHTLJSParser::WhileStatementContext>(0);
}

CHTLJSParser::SwitchStatementContext* CHTLJSParser::ControlStatementContext::switchStatement() {
  return getRuleContext<CHTLJSParser::SwitchStatementContext>(0);
}


size_t CHTLJSParser::ControlStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleControlStatement;
}

void CHTLJSParser::ControlStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterControlStatement(this);
}

void CHTLJSParser::ControlStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitControlStatement(this);
}


std::any CHTLJSParser::ControlStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitControlStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ControlStatementContext* CHTLJSParser::controlStatement() {
  ControlStatementContext *_localctx = _tracker.createInstance<ControlStatementContext>(_ctx, getState());
  enterRule(_localctx, 8, CHTLJSParser::RuleControlStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(128);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::IF: {
        enterOuterAlt(_localctx, 1);
        setState(124);
        ifStatement();
        break;
      }

      case CHTLJSParser::FOR: {
        enterOuterAlt(_localctx, 2);
        setState(125);
        forStatement();
        break;
      }

      case CHTLJSParser::WHILE: {
        enterOuterAlt(_localctx, 3);
        setState(126);
        whileStatement();
        break;
      }

      case CHTLJSParser::SWITCH: {
        enterOuterAlt(_localctx, 4);
        setState(127);
        switchStatement();
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

//----------------- ChthljsStatementContext ------------------------------------------------------------------

CHTLJSParser::ChthljsStatementContext::ChthljsStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::FileloaderStatementContext* CHTLJSParser::ChthljsStatementContext::fileloaderStatement() {
  return getRuleContext<CHTLJSParser::FileloaderStatementContext>(0);
}

CHTLJSParser::ListenStatementContext* CHTLJSParser::ChthljsStatementContext::listenStatement() {
  return getRuleContext<CHTLJSParser::ListenStatementContext>(0);
}

CHTLJSParser::DelegateStatementContext* CHTLJSParser::ChthljsStatementContext::delegateStatement() {
  return getRuleContext<CHTLJSParser::DelegateStatementContext>(0);
}

CHTLJSParser::AnimateStatementContext* CHTLJSParser::ChthljsStatementContext::animateStatement() {
  return getRuleContext<CHTLJSParser::AnimateStatementContext>(0);
}

CHTLJSParser::VirStatementContext* CHTLJSParser::ChthljsStatementContext::virStatement() {
  return getRuleContext<CHTLJSParser::VirStatementContext>(0);
}

CHTLJSParser::RouterStatementContext* CHTLJSParser::ChthljsStatementContext::routerStatement() {
  return getRuleContext<CHTLJSParser::RouterStatementContext>(0);
}

CHTLJSParser::UtilStatementContext* CHTLJSParser::ChthljsStatementContext::utilStatement() {
  return getRuleContext<CHTLJSParser::UtilStatementContext>(0);
}


size_t CHTLJSParser::ChthljsStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleChthljsStatement;
}

void CHTLJSParser::ChthljsStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterChthljsStatement(this);
}

void CHTLJSParser::ChthljsStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitChthljsStatement(this);
}


std::any CHTLJSParser::ChthljsStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitChthljsStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ChthljsStatementContext* CHTLJSParser::chthljsStatement() {
  ChthljsStatementContext *_localctx = _tracker.createInstance<ChthljsStatementContext>(_ctx, getState());
  enterRule(_localctx, 10, CHTLJSParser::RuleChthljsStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(137);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::FILELOADER: {
        enterOuterAlt(_localctx, 1);
        setState(130);
        fileloaderStatement();
        break;
      }

      case CHTLJSParser::LISTEN: {
        enterOuterAlt(_localctx, 2);
        setState(131);
        listenStatement();
        break;
      }

      case CHTLJSParser::DELEGATE: {
        enterOuterAlt(_localctx, 3);
        setState(132);
        delegateStatement();
        break;
      }

      case CHTLJSParser::ANIMATE: {
        enterOuterAlt(_localctx, 4);
        setState(133);
        animateStatement();
        break;
      }

      case CHTLJSParser::VIR: {
        enterOuterAlt(_localctx, 5);
        setState(134);
        virStatement();
        break;
      }

      case CHTLJSParser::ROUTER: {
        enterOuterAlt(_localctx, 6);
        setState(135);
        routerStatement();
        break;
      }

      case CHTLJSParser::UTIL: {
        enterOuterAlt(_localctx, 7);
        setState(136);
        utilStatement();
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

//----------------- ExpressionContext ------------------------------------------------------------------

CHTLJSParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::AssignmentExpressionContext* CHTLJSParser::ExpressionContext::assignmentExpression() {
  return getRuleContext<CHTLJSParser::AssignmentExpressionContext>(0);
}


size_t CHTLJSParser::ExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleExpression;
}

void CHTLJSParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void CHTLJSParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}


std::any CHTLJSParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 12, CHTLJSParser::RuleExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(139);
    assignmentExpression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignmentExpressionContext ------------------------------------------------------------------

CHTLJSParser::AssignmentExpressionContext::AssignmentExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::ConditionalExpressionContext* CHTLJSParser::AssignmentExpressionContext::conditionalExpression() {
  return getRuleContext<CHTLJSParser::ConditionalExpressionContext>(0);
}

CHTLJSParser::AssignmentOperatorContext* CHTLJSParser::AssignmentExpressionContext::assignmentOperator() {
  return getRuleContext<CHTLJSParser::AssignmentOperatorContext>(0);
}

CHTLJSParser::AssignmentExpressionContext* CHTLJSParser::AssignmentExpressionContext::assignmentExpression() {
  return getRuleContext<CHTLJSParser::AssignmentExpressionContext>(0);
}


size_t CHTLJSParser::AssignmentExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleAssignmentExpression;
}

void CHTLJSParser::AssignmentExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentExpression(this);
}

void CHTLJSParser::AssignmentExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentExpression(this);
}


std::any CHTLJSParser::AssignmentExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitAssignmentExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::AssignmentExpressionContext* CHTLJSParser::assignmentExpression() {
  AssignmentExpressionContext *_localctx = _tracker.createInstance<AssignmentExpressionContext>(_ctx, getState());
  enterRule(_localctx, 14, CHTLJSParser::RuleAssignmentExpression);
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
    setState(141);
    conditionalExpression();
    setState(145);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 49) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 49)) & 262175) != 0)) {
      setState(142);
      assignmentOperator();
      setState(143);
      assignmentExpression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConditionalExpressionContext ------------------------------------------------------------------

CHTLJSParser::ConditionalExpressionContext::ConditionalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::LogicalOrExpressionContext* CHTLJSParser::ConditionalExpressionContext::logicalOrExpression() {
  return getRuleContext<CHTLJSParser::LogicalOrExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::ConditionalExpressionContext::QUESTION() {
  return getToken(CHTLJSParser::QUESTION, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ConditionalExpressionContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::ConditionalExpressionContext::COLON() {
  return getToken(CHTLJSParser::COLON, 0);
}

CHTLJSParser::ConditionalExpressionContext* CHTLJSParser::ConditionalExpressionContext::conditionalExpression() {
  return getRuleContext<CHTLJSParser::ConditionalExpressionContext>(0);
}


size_t CHTLJSParser::ConditionalExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleConditionalExpression;
}

void CHTLJSParser::ConditionalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConditionalExpression(this);
}

void CHTLJSParser::ConditionalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConditionalExpression(this);
}


std::any CHTLJSParser::ConditionalExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitConditionalExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ConditionalExpressionContext* CHTLJSParser::conditionalExpression() {
  ConditionalExpressionContext *_localctx = _tracker.createInstance<ConditionalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 16, CHTLJSParser::RuleConditionalExpression);
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
    logicalOrExpression();
    setState(153);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLJSParser::QUESTION) {
      setState(148);
      match(CHTLJSParser::QUESTION);
      setState(149);
      expression();
      setState(150);
      match(CHTLJSParser::COLON);
      setState(151);
      conditionalExpression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LogicalOrExpressionContext ------------------------------------------------------------------

CHTLJSParser::LogicalOrExpressionContext::LogicalOrExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::LogicalAndExpressionContext *> CHTLJSParser::LogicalOrExpressionContext::logicalAndExpression() {
  return getRuleContexts<CHTLJSParser::LogicalAndExpressionContext>();
}

CHTLJSParser::LogicalAndExpressionContext* CHTLJSParser::LogicalOrExpressionContext::logicalAndExpression(size_t i) {
  return getRuleContext<CHTLJSParser::LogicalAndExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::LogicalOrExpressionContext::OR() {
  return getTokens(CHTLJSParser::OR);
}

tree::TerminalNode* CHTLJSParser::LogicalOrExpressionContext::OR(size_t i) {
  return getToken(CHTLJSParser::OR, i);
}


size_t CHTLJSParser::LogicalOrExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleLogicalOrExpression;
}

void CHTLJSParser::LogicalOrExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOrExpression(this);
}

void CHTLJSParser::LogicalOrExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalOrExpression(this);
}


std::any CHTLJSParser::LogicalOrExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitLogicalOrExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::LogicalOrExpressionContext* CHTLJSParser::logicalOrExpression() {
  LogicalOrExpressionContext *_localctx = _tracker.createInstance<LogicalOrExpressionContext>(_ctx, getState());
  enterRule(_localctx, 18, CHTLJSParser::RuleLogicalOrExpression);
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
    setState(155);
    logicalAndExpression();
    setState(160);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::OR) {
      setState(156);
      match(CHTLJSParser::OR);
      setState(157);
      logicalAndExpression();
      setState(162);
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

//----------------- LogicalAndExpressionContext ------------------------------------------------------------------

CHTLJSParser::LogicalAndExpressionContext::LogicalAndExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::EqualityExpressionContext *> CHTLJSParser::LogicalAndExpressionContext::equalityExpression() {
  return getRuleContexts<CHTLJSParser::EqualityExpressionContext>();
}

CHTLJSParser::EqualityExpressionContext* CHTLJSParser::LogicalAndExpressionContext::equalityExpression(size_t i) {
  return getRuleContext<CHTLJSParser::EqualityExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::LogicalAndExpressionContext::AND() {
  return getTokens(CHTLJSParser::AND);
}

tree::TerminalNode* CHTLJSParser::LogicalAndExpressionContext::AND(size_t i) {
  return getToken(CHTLJSParser::AND, i);
}


size_t CHTLJSParser::LogicalAndExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleLogicalAndExpression;
}

void CHTLJSParser::LogicalAndExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalAndExpression(this);
}

void CHTLJSParser::LogicalAndExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalAndExpression(this);
}


std::any CHTLJSParser::LogicalAndExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitLogicalAndExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::LogicalAndExpressionContext* CHTLJSParser::logicalAndExpression() {
  LogicalAndExpressionContext *_localctx = _tracker.createInstance<LogicalAndExpressionContext>(_ctx, getState());
  enterRule(_localctx, 20, CHTLJSParser::RuleLogicalAndExpression);
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
    setState(163);
    equalityExpression();
    setState(168);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::AND) {
      setState(164);
      match(CHTLJSParser::AND);
      setState(165);
      equalityExpression();
      setState(170);
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

//----------------- EqualityExpressionContext ------------------------------------------------------------------

CHTLJSParser::EqualityExpressionContext::EqualityExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::RelationalExpressionContext *> CHTLJSParser::EqualityExpressionContext::relationalExpression() {
  return getRuleContexts<CHTLJSParser::RelationalExpressionContext>();
}

CHTLJSParser::RelationalExpressionContext* CHTLJSParser::EqualityExpressionContext::relationalExpression(size_t i) {
  return getRuleContext<CHTLJSParser::RelationalExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::EqualityExpressionContext::EQUAL_EQUAL() {
  return getTokens(CHTLJSParser::EQUAL_EQUAL);
}

tree::TerminalNode* CHTLJSParser::EqualityExpressionContext::EQUAL_EQUAL(size_t i) {
  return getToken(CHTLJSParser::EQUAL_EQUAL, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::EqualityExpressionContext::NOT_EQUAL() {
  return getTokens(CHTLJSParser::NOT_EQUAL);
}

tree::TerminalNode* CHTLJSParser::EqualityExpressionContext::NOT_EQUAL(size_t i) {
  return getToken(CHTLJSParser::NOT_EQUAL, i);
}


size_t CHTLJSParser::EqualityExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleEqualityExpression;
}

void CHTLJSParser::EqualityExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpression(this);
}

void CHTLJSParser::EqualityExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqualityExpression(this);
}


std::any CHTLJSParser::EqualityExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitEqualityExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::EqualityExpressionContext* CHTLJSParser::equalityExpression() {
  EqualityExpressionContext *_localctx = _tracker.createInstance<EqualityExpressionContext>(_ctx, getState());
  enterRule(_localctx, 22, CHTLJSParser::RuleEqualityExpression);
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
    setState(171);
    relationalExpression();
    setState(176);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::EQUAL_EQUAL

    || _la == CHTLJSParser::NOT_EQUAL) {
      setState(172);
      _la = _input->LA(1);
      if (!(_la == CHTLJSParser::EQUAL_EQUAL

      || _la == CHTLJSParser::NOT_EQUAL)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(173);
      relationalExpression();
      setState(178);
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

//----------------- RelationalExpressionContext ------------------------------------------------------------------

CHTLJSParser::RelationalExpressionContext::RelationalExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::AdditiveExpressionContext *> CHTLJSParser::RelationalExpressionContext::additiveExpression() {
  return getRuleContexts<CHTLJSParser::AdditiveExpressionContext>();
}

CHTLJSParser::AdditiveExpressionContext* CHTLJSParser::RelationalExpressionContext::additiveExpression(size_t i) {
  return getRuleContext<CHTLJSParser::AdditiveExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::RelationalExpressionContext::LESS() {
  return getTokens(CHTLJSParser::LESS);
}

tree::TerminalNode* CHTLJSParser::RelationalExpressionContext::LESS(size_t i) {
  return getToken(CHTLJSParser::LESS, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::RelationalExpressionContext::GREATER() {
  return getTokens(CHTLJSParser::GREATER);
}

tree::TerminalNode* CHTLJSParser::RelationalExpressionContext::GREATER(size_t i) {
  return getToken(CHTLJSParser::GREATER, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::RelationalExpressionContext::LESS_EQUAL() {
  return getTokens(CHTLJSParser::LESS_EQUAL);
}

tree::TerminalNode* CHTLJSParser::RelationalExpressionContext::LESS_EQUAL(size_t i) {
  return getToken(CHTLJSParser::LESS_EQUAL, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::RelationalExpressionContext::GREATER_EQUAL() {
  return getTokens(CHTLJSParser::GREATER_EQUAL);
}

tree::TerminalNode* CHTLJSParser::RelationalExpressionContext::GREATER_EQUAL(size_t i) {
  return getToken(CHTLJSParser::GREATER_EQUAL, i);
}


size_t CHTLJSParser::RelationalExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleRelationalExpression;
}

void CHTLJSParser::RelationalExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelationalExpression(this);
}

void CHTLJSParser::RelationalExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelationalExpression(this);
}


std::any CHTLJSParser::RelationalExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitRelationalExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::RelationalExpressionContext* CHTLJSParser::relationalExpression() {
  RelationalExpressionContext *_localctx = _tracker.createInstance<RelationalExpressionContext>(_ctx, getState());
  enterRule(_localctx, 24, CHTLJSParser::RuleRelationalExpression);
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
    additiveExpression();
    setState(184);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 131941395333120) != 0)) {
      setState(180);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 131941395333120) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(181);
      additiveExpression();
      setState(186);
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

//----------------- AdditiveExpressionContext ------------------------------------------------------------------

CHTLJSParser::AdditiveExpressionContext::AdditiveExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::MultiplicativeExpressionContext *> CHTLJSParser::AdditiveExpressionContext::multiplicativeExpression() {
  return getRuleContexts<CHTLJSParser::MultiplicativeExpressionContext>();
}

CHTLJSParser::MultiplicativeExpressionContext* CHTLJSParser::AdditiveExpressionContext::multiplicativeExpression(size_t i) {
  return getRuleContext<CHTLJSParser::MultiplicativeExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::AdditiveExpressionContext::PLUS() {
  return getTokens(CHTLJSParser::PLUS);
}

tree::TerminalNode* CHTLJSParser::AdditiveExpressionContext::PLUS(size_t i) {
  return getToken(CHTLJSParser::PLUS, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::AdditiveExpressionContext::MINUS() {
  return getTokens(CHTLJSParser::MINUS);
}

tree::TerminalNode* CHTLJSParser::AdditiveExpressionContext::MINUS(size_t i) {
  return getToken(CHTLJSParser::MINUS, i);
}


size_t CHTLJSParser::AdditiveExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleAdditiveExpression;
}

void CHTLJSParser::AdditiveExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAdditiveExpression(this);
}

void CHTLJSParser::AdditiveExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAdditiveExpression(this);
}


std::any CHTLJSParser::AdditiveExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitAdditiveExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::AdditiveExpressionContext* CHTLJSParser::additiveExpression() {
  AdditiveExpressionContext *_localctx = _tracker.createInstance<AdditiveExpressionContext>(_ctx, getState());
  enterRule(_localctx, 26, CHTLJSParser::RuleAdditiveExpression);
  size_t _la = 0;

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
    setState(187);
    multiplicativeExpression();
    setState(192);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(188);
        _la = _input->LA(1);
        if (!(_la == CHTLJSParser::PLUS

        || _la == CHTLJSParser::MINUS)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(189);
        multiplicativeExpression(); 
      }
      setState(194);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MultiplicativeExpressionContext ------------------------------------------------------------------

CHTLJSParser::MultiplicativeExpressionContext::MultiplicativeExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::UnaryExpressionContext *> CHTLJSParser::MultiplicativeExpressionContext::unaryExpression() {
  return getRuleContexts<CHTLJSParser::UnaryExpressionContext>();
}

CHTLJSParser::UnaryExpressionContext* CHTLJSParser::MultiplicativeExpressionContext::unaryExpression(size_t i) {
  return getRuleContext<CHTLJSParser::UnaryExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::MultiplicativeExpressionContext::MULTIPLY() {
  return getTokens(CHTLJSParser::MULTIPLY);
}

tree::TerminalNode* CHTLJSParser::MultiplicativeExpressionContext::MULTIPLY(size_t i) {
  return getToken(CHTLJSParser::MULTIPLY, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::MultiplicativeExpressionContext::DIVIDE() {
  return getTokens(CHTLJSParser::DIVIDE);
}

tree::TerminalNode* CHTLJSParser::MultiplicativeExpressionContext::DIVIDE(size_t i) {
  return getToken(CHTLJSParser::DIVIDE, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::MultiplicativeExpressionContext::MODULO() {
  return getTokens(CHTLJSParser::MODULO);
}

tree::TerminalNode* CHTLJSParser::MultiplicativeExpressionContext::MODULO(size_t i) {
  return getToken(CHTLJSParser::MODULO, i);
}


size_t CHTLJSParser::MultiplicativeExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleMultiplicativeExpression;
}

void CHTLJSParser::MultiplicativeExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiplicativeExpression(this);
}

void CHTLJSParser::MultiplicativeExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMultiplicativeExpression(this);
}


std::any CHTLJSParser::MultiplicativeExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitMultiplicativeExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::MultiplicativeExpressionContext* CHTLJSParser::multiplicativeExpression() {
  MultiplicativeExpressionContext *_localctx = _tracker.createInstance<MultiplicativeExpressionContext>(_ctx, getState());
  enterRule(_localctx, 28, CHTLJSParser::RuleMultiplicativeExpression);
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
    setState(195);
    unaryExpression();
    setState(200);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 962072674304) != 0)) {
      setState(196);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 962072674304) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(197);
      unaryExpression();
      setState(202);
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

//----------------- UnaryExpressionContext ------------------------------------------------------------------

CHTLJSParser::UnaryExpressionContext::UnaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::UnaryExpressionContext* CHTLJSParser::UnaryExpressionContext::unaryExpression() {
  return getRuleContext<CHTLJSParser::UnaryExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::UnaryExpressionContext::PLUS() {
  return getToken(CHTLJSParser::PLUS, 0);
}

tree::TerminalNode* CHTLJSParser::UnaryExpressionContext::MINUS() {
  return getToken(CHTLJSParser::MINUS, 0);
}

tree::TerminalNode* CHTLJSParser::UnaryExpressionContext::NOT() {
  return getToken(CHTLJSParser::NOT, 0);
}

tree::TerminalNode* CHTLJSParser::UnaryExpressionContext::TILDE() {
  return getToken(CHTLJSParser::TILDE, 0);
}

CHTLJSParser::PostfixExpressionContext* CHTLJSParser::UnaryExpressionContext::postfixExpression() {
  return getRuleContext<CHTLJSParser::PostfixExpressionContext>(0);
}


size_t CHTLJSParser::UnaryExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleUnaryExpression;
}

void CHTLJSParser::UnaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpression(this);
}

void CHTLJSParser::UnaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpression(this);
}


std::any CHTLJSParser::UnaryExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitUnaryExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::UnaryExpressionContext* CHTLJSParser::unaryExpression() {
  UnaryExpressionContext *_localctx = _tracker.createInstance<UnaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 30, CHTLJSParser::RuleUnaryExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(206);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::PLUS:
      case CHTLJSParser::MINUS:
      case CHTLJSParser::NOT:
      case CHTLJSParser::TILDE: {
        enterOuterAlt(_localctx, 1);
        setState(203);
        _la = _input->LA(1);
        if (!(((((_la - 35) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 35)) & 2147483779) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(204);
        unaryExpression();
        break;
      }

      case CHTLJSParser::FUNCTION:
      case CHTLJSParser::LEFT_BRACE:
      case CHTLJSParser::LEFT_BRACKET:
      case CHTLJSParser::LEFT_PAREN:
      case CHTLJSParser::DOUBLE_BRACE:
      case CHTLJSParser::STRING:
      case CHTLJSParser::NUMBER:
      case CHTLJSParser::BOOLEAN:
      case CHTLJSParser::NULL_:
      case CHTLJSParser::UNDEFINED:
      case CHTLJSParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(205);
        postfixExpression();
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

//----------------- PostfixExpressionContext ------------------------------------------------------------------

CHTLJSParser::PostfixExpressionContext::PostfixExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::PrimaryExpressionContext* CHTLJSParser::PostfixExpressionContext::primaryExpression() {
  return getRuleContext<CHTLJSParser::PrimaryExpressionContext>(0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::LEFT_BRACKET() {
  return getTokens(CHTLJSParser::LEFT_BRACKET);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::LEFT_BRACKET(size_t i) {
  return getToken(CHTLJSParser::LEFT_BRACKET, i);
}

std::vector<CHTLJSParser::ExpressionContext *> CHTLJSParser::PostfixExpressionContext::expression() {
  return getRuleContexts<CHTLJSParser::ExpressionContext>();
}

CHTLJSParser::ExpressionContext* CHTLJSParser::PostfixExpressionContext::expression(size_t i) {
  return getRuleContext<CHTLJSParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::RIGHT_BRACKET() {
  return getTokens(CHTLJSParser::RIGHT_BRACKET);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::RIGHT_BRACKET(size_t i) {
  return getToken(CHTLJSParser::RIGHT_BRACKET, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::DOT() {
  return getTokens(CHTLJSParser::DOT);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::DOT(size_t i) {
  return getToken(CHTLJSParser::DOT, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::IDENTIFIER() {
  return getTokens(CHTLJSParser::IDENTIFIER);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::IDENTIFIER(size_t i) {
  return getToken(CHTLJSParser::IDENTIFIER, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::LEFT_PAREN() {
  return getTokens(CHTLJSParser::LEFT_PAREN);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::LEFT_PAREN(size_t i) {
  return getToken(CHTLJSParser::LEFT_PAREN, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::PostfixExpressionContext::RIGHT_PAREN() {
  return getTokens(CHTLJSParser::RIGHT_PAREN);
}

tree::TerminalNode* CHTLJSParser::PostfixExpressionContext::RIGHT_PAREN(size_t i) {
  return getToken(CHTLJSParser::RIGHT_PAREN, i);
}

std::vector<CHTLJSParser::ArgumentListContext *> CHTLJSParser::PostfixExpressionContext::argumentList() {
  return getRuleContexts<CHTLJSParser::ArgumentListContext>();
}

CHTLJSParser::ArgumentListContext* CHTLJSParser::PostfixExpressionContext::argumentList(size_t i) {
  return getRuleContext<CHTLJSParser::ArgumentListContext>(i);
}


size_t CHTLJSParser::PostfixExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RulePostfixExpression;
}

void CHTLJSParser::PostfixExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPostfixExpression(this);
}

void CHTLJSParser::PostfixExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPostfixExpression(this);
}


std::any CHTLJSParser::PostfixExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitPostfixExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::PostfixExpressionContext* CHTLJSParser::postfixExpression() {
  PostfixExpressionContext *_localctx = _tracker.createInstance<PostfixExpressionContext>(_ctx, getState());
  enterRule(_localctx, 32, CHTLJSParser::RulePostfixExpression);
  size_t _la = 0;

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
    setState(208);
    primaryExpression();
    setState(222);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(220);
        _errHandler->sync(this);
        switch (_input->LA(1)) {
          case CHTLJSParser::LEFT_BRACKET: {
            setState(209);
            match(CHTLJSParser::LEFT_BRACKET);
            setState(210);
            expression();
            setState(211);
            match(CHTLJSParser::RIGHT_BRACKET);
            break;
          }

          case CHTLJSParser::DOT: {
            setState(213);
            match(CHTLJSParser::DOT);
            setState(214);
            match(CHTLJSParser::IDENTIFIER);
            break;
          }

          case CHTLJSParser::LEFT_PAREN: {
            setState(215);
            match(CHTLJSParser::LEFT_PAREN);
            setState(217);
            _errHandler->sync(this);

            _la = _input->LA(1);
            if (((((_la - 4) & ~ 0x3fULL) == 0) &&
              ((1ULL << (_la - 4)) & 5183643452445818881) != 0)) {
              setState(216);
              argumentList();
            }
            setState(219);
            match(CHTLJSParser::RIGHT_PAREN);
            break;
          }

        default:
          throw NoViableAltException(this);
        } 
      }
      setState(224);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExpressionContext ------------------------------------------------------------------

CHTLJSParser::PrimaryExpressionContext::PrimaryExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::LiteralContext* CHTLJSParser::PrimaryExpressionContext::literal() {
  return getRuleContext<CHTLJSParser::LiteralContext>(0);
}

CHTLJSParser::IdentifierContext* CHTLJSParser::PrimaryExpressionContext::identifier() {
  return getRuleContext<CHTLJSParser::IdentifierContext>(0);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::PrimaryExpressionContext::chthljsSelector() {
  return getRuleContext<CHTLJSParser::ChthljsSelectorContext>(0);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::PrimaryExpressionContext::functionExpression() {
  return getRuleContext<CHTLJSParser::FunctionExpressionContext>(0);
}

CHTLJSParser::ObjectExpressionContext* CHTLJSParser::PrimaryExpressionContext::objectExpression() {
  return getRuleContext<CHTLJSParser::ObjectExpressionContext>(0);
}

CHTLJSParser::ArrayExpressionContext* CHTLJSParser::PrimaryExpressionContext::arrayExpression() {
  return getRuleContext<CHTLJSParser::ArrayExpressionContext>(0);
}

CHTLJSParser::ParenthesizedExpressionContext* CHTLJSParser::PrimaryExpressionContext::parenthesizedExpression() {
  return getRuleContext<CHTLJSParser::ParenthesizedExpressionContext>(0);
}


size_t CHTLJSParser::PrimaryExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RulePrimaryExpression;
}

void CHTLJSParser::PrimaryExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpression(this);
}

void CHTLJSParser::PrimaryExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpression(this);
}


std::any CHTLJSParser::PrimaryExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitPrimaryExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::PrimaryExpressionContext* CHTLJSParser::primaryExpression() {
  PrimaryExpressionContext *_localctx = _tracker.createInstance<PrimaryExpressionContext>(_ctx, getState());
  enterRule(_localctx, 34, CHTLJSParser::RulePrimaryExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(232);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::STRING:
      case CHTLJSParser::NUMBER:
      case CHTLJSParser::BOOLEAN:
      case CHTLJSParser::NULL_:
      case CHTLJSParser::UNDEFINED: {
        enterOuterAlt(_localctx, 1);
        setState(225);
        literal();
        break;
      }

      case CHTLJSParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(226);
        identifier();
        break;
      }

      case CHTLJSParser::DOUBLE_BRACE: {
        enterOuterAlt(_localctx, 3);
        setState(227);
        chthljsSelector();
        break;
      }

      case CHTLJSParser::FUNCTION: {
        enterOuterAlt(_localctx, 4);
        setState(228);
        functionExpression();
        break;
      }

      case CHTLJSParser::LEFT_BRACE: {
        enterOuterAlt(_localctx, 5);
        setState(229);
        objectExpression();
        break;
      }

      case CHTLJSParser::LEFT_BRACKET: {
        enterOuterAlt(_localctx, 6);
        setState(230);
        arrayExpression();
        break;
      }

      case CHTLJSParser::LEFT_PAREN: {
        enterOuterAlt(_localctx, 7);
        setState(231);
        parenthesizedExpression();
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

//----------------- LiteralContext ------------------------------------------------------------------

CHTLJSParser::LiteralContext::LiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::LiteralContext::STRING() {
  return getToken(CHTLJSParser::STRING, 0);
}

tree::TerminalNode* CHTLJSParser::LiteralContext::NUMBER() {
  return getToken(CHTLJSParser::NUMBER, 0);
}

tree::TerminalNode* CHTLJSParser::LiteralContext::BOOLEAN() {
  return getToken(CHTLJSParser::BOOLEAN, 0);
}

tree::TerminalNode* CHTLJSParser::LiteralContext::NULL_() {
  return getToken(CHTLJSParser::NULL_, 0);
}

tree::TerminalNode* CHTLJSParser::LiteralContext::UNDEFINED() {
  return getToken(CHTLJSParser::UNDEFINED, 0);
}


size_t CHTLJSParser::LiteralContext::getRuleIndex() const {
  return CHTLJSParser::RuleLiteral;
}

void CHTLJSParser::LiteralContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteral(this);
}

void CHTLJSParser::LiteralContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteral(this);
}


std::any CHTLJSParser::LiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitLiteral(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::LiteralContext* CHTLJSParser::literal() {
  LiteralContext *_localctx = _tracker.createInstance<LiteralContext>(_ctx, getState());
  enterRule(_localctx, 36, CHTLJSParser::RuleLiteral);
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
    setState(234);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4467570830351532032) != 0))) {
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

//----------------- IdentifierContext ------------------------------------------------------------------

CHTLJSParser::IdentifierContext::IdentifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::IdentifierContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}


size_t CHTLJSParser::IdentifierContext::getRuleIndex() const {
  return CHTLJSParser::RuleIdentifier;
}

void CHTLJSParser::IdentifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifier(this);
}

void CHTLJSParser::IdentifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifier(this);
}


std::any CHTLJSParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::IdentifierContext* CHTLJSParser::identifier() {
  IdentifierContext *_localctx = _tracker.createInstance<IdentifierContext>(_ctx, getState());
  enterRule(_localctx, 38, CHTLJSParser::RuleIdentifier);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    match(CHTLJSParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ChthljsSelectorContext ------------------------------------------------------------------

CHTLJSParser::ChthljsSelectorContext::ChthljsSelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CHTLJSParser::ChthljsSelectorContext::DOUBLE_BRACE() {
  return getTokens(CHTLJSParser::DOUBLE_BRACE);
}

tree::TerminalNode* CHTLJSParser::ChthljsSelectorContext::DOUBLE_BRACE(size_t i) {
  return getToken(CHTLJSParser::DOUBLE_BRACE, i);
}

CHTLJSParser::SelectorContentContext* CHTLJSParser::ChthljsSelectorContext::selectorContent() {
  return getRuleContext<CHTLJSParser::SelectorContentContext>(0);
}


size_t CHTLJSParser::ChthljsSelectorContext::getRuleIndex() const {
  return CHTLJSParser::RuleChthljsSelector;
}

void CHTLJSParser::ChthljsSelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterChthljsSelector(this);
}

void CHTLJSParser::ChthljsSelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitChthljsSelector(this);
}


std::any CHTLJSParser::ChthljsSelectorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitChthljsSelector(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::chthljsSelector() {
  ChthljsSelectorContext *_localctx = _tracker.createInstance<ChthljsSelectorContext>(_ctx, getState());
  enterRule(_localctx, 40, CHTLJSParser::RuleChthljsSelector);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(238);
    match(CHTLJSParser::DOUBLE_BRACE);
    setState(239);
    selectorContent();
    setState(240);
    match(CHTLJSParser::DOUBLE_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectorContentContext ------------------------------------------------------------------

CHTLJSParser::SelectorContentContext::SelectorContentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::SelectorContext* CHTLJSParser::SelectorContentContext::selector() {
  return getRuleContext<CHTLJSParser::SelectorContext>(0);
}

tree::TerminalNode* CHTLJSParser::SelectorContentContext::DOT() {
  return getToken(CHTLJSParser::DOT, 0);
}

CHTLJSParser::PropertyContext* CHTLJSParser::SelectorContentContext::property() {
  return getRuleContext<CHTLJSParser::PropertyContext>(0);
}

tree::TerminalNode* CHTLJSParser::SelectorContentContext::LEFT_BRACKET() {
  return getToken(CHTLJSParser::LEFT_BRACKET, 0);
}

CHTLJSParser::IndexContext* CHTLJSParser::SelectorContentContext::index() {
  return getRuleContext<CHTLJSParser::IndexContext>(0);
}

tree::TerminalNode* CHTLJSParser::SelectorContentContext::RIGHT_BRACKET() {
  return getToken(CHTLJSParser::RIGHT_BRACKET, 0);
}


size_t CHTLJSParser::SelectorContentContext::getRuleIndex() const {
  return CHTLJSParser::RuleSelectorContent;
}

void CHTLJSParser::SelectorContentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelectorContent(this);
}

void CHTLJSParser::SelectorContentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelectorContent(this);
}


std::any CHTLJSParser::SelectorContentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitSelectorContent(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::SelectorContentContext* CHTLJSParser::selectorContent() {
  SelectorContentContext *_localctx = _tracker.createInstance<SelectorContentContext>(_ctx, getState());
  enterRule(_localctx, 42, CHTLJSParser::RuleSelectorContent);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(252);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(242);
      selector();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(243);
      selector();
      setState(244);
      match(CHTLJSParser::DOT);
      setState(245);
      property();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(247);
      selector();
      setState(248);
      match(CHTLJSParser::LEFT_BRACKET);
      setState(249);
      index();
      setState(250);
      match(CHTLJSParser::RIGHT_BRACKET);
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

//----------------- SelectorContext ------------------------------------------------------------------

CHTLJSParser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::SelectorContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::HASH() {
  return getToken(CHTLJSParser::HASH, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::DOT() {
  return getToken(CHTLJSParser::DOT, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::ASTERISK() {
  return getToken(CHTLJSParser::ASTERISK, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::LEFT_BRACKET() {
  return getToken(CHTLJSParser::LEFT_BRACKET, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::NUMBER() {
  return getToken(CHTLJSParser::NUMBER, 0);
}

tree::TerminalNode* CHTLJSParser::SelectorContext::RIGHT_BRACKET() {
  return getToken(CHTLJSParser::RIGHT_BRACKET, 0);
}


size_t CHTLJSParser::SelectorContext::getRuleIndex() const {
  return CHTLJSParser::RuleSelector;
}

void CHTLJSParser::SelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector(this);
}

void CHTLJSParser::SelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector(this);
}


std::any CHTLJSParser::SelectorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitSelector(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::SelectorContext* CHTLJSParser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 44, CHTLJSParser::RuleSelector);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(264);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(254);
      match(CHTLJSParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(255);
      match(CHTLJSParser::HASH);
      setState(256);
      match(CHTLJSParser::IDENTIFIER);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(257);
      match(CHTLJSParser::DOT);
      setState(258);
      match(CHTLJSParser::IDENTIFIER);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(259);
      match(CHTLJSParser::ASTERISK);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(260);
      match(CHTLJSParser::IDENTIFIER);
      setState(261);
      match(CHTLJSParser::LEFT_BRACKET);
      setState(262);
      match(CHTLJSParser::NUMBER);
      setState(263);
      match(CHTLJSParser::RIGHT_BRACKET);
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

//----------------- PropertyContext ------------------------------------------------------------------

CHTLJSParser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::PropertyContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}


size_t CHTLJSParser::PropertyContext::getRuleIndex() const {
  return CHTLJSParser::RuleProperty;
}

void CHTLJSParser::PropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProperty(this);
}

void CHTLJSParser::PropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProperty(this);
}


std::any CHTLJSParser::PropertyContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitProperty(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::PropertyContext* CHTLJSParser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 46, CHTLJSParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(266);
    match(CHTLJSParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IndexContext ------------------------------------------------------------------

CHTLJSParser::IndexContext::IndexContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::IndexContext::NUMBER() {
  return getToken(CHTLJSParser::NUMBER, 0);
}

tree::TerminalNode* CHTLJSParser::IndexContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}


size_t CHTLJSParser::IndexContext::getRuleIndex() const {
  return CHTLJSParser::RuleIndex;
}

void CHTLJSParser::IndexContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIndex(this);
}

void CHTLJSParser::IndexContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIndex(this);
}


std::any CHTLJSParser::IndexContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitIndex(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::IndexContext* CHTLJSParser::index() {
  IndexContext *_localctx = _tracker.createInstance<IndexContext>(_ctx, getState());
  enterRule(_localctx, 48, CHTLJSParser::RuleIndex);
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
    setState(268);
    _la = _input->LA(1);
    if (!(_la == CHTLJSParser::NUMBER

    || _la == CHTLJSParser::IDENTIFIER)) {
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

//----------------- FunctionExpressionContext ------------------------------------------------------------------

CHTLJSParser::FunctionExpressionContext::FunctionExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::FunctionExpressionContext::FUNCTION() {
  return getToken(CHTLJSParser::FUNCTION, 0);
}

tree::TerminalNode* CHTLJSParser::FunctionExpressionContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

tree::TerminalNode* CHTLJSParser::FunctionExpressionContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

CHTLJSParser::BlockContext* CHTLJSParser::FunctionExpressionContext::block() {
  return getRuleContext<CHTLJSParser::BlockContext>(0);
}

CHTLJSParser::ParameterListContext* CHTLJSParser::FunctionExpressionContext::parameterList() {
  return getRuleContext<CHTLJSParser::ParameterListContext>(0);
}


size_t CHTLJSParser::FunctionExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleFunctionExpression;
}

void CHTLJSParser::FunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExpression(this);
}

void CHTLJSParser::FunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExpression(this);
}


std::any CHTLJSParser::FunctionExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitFunctionExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::functionExpression() {
  FunctionExpressionContext *_localctx = _tracker.createInstance<FunctionExpressionContext>(_ctx, getState());
  enterRule(_localctx, 50, CHTLJSParser::RuleFunctionExpression);
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
    setState(270);
    match(CHTLJSParser::FUNCTION);
    setState(271);
    match(CHTLJSParser::LEFT_PAREN);
    setState(273);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLJSParser::IDENTIFIER) {
      setState(272);
      parameterList();
    }
    setState(275);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(276);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterListContext ------------------------------------------------------------------

CHTLJSParser::ParameterListContext::ParameterListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CHTLJSParser::ParameterListContext::IDENTIFIER() {
  return getTokens(CHTLJSParser::IDENTIFIER);
}

tree::TerminalNode* CHTLJSParser::ParameterListContext::IDENTIFIER(size_t i) {
  return getToken(CHTLJSParser::IDENTIFIER, i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::ParameterListContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::ParameterListContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::ParameterListContext::getRuleIndex() const {
  return CHTLJSParser::RuleParameterList;
}

void CHTLJSParser::ParameterListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParameterList(this);
}

void CHTLJSParser::ParameterListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParameterList(this);
}


std::any CHTLJSParser::ParameterListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitParameterList(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ParameterListContext* CHTLJSParser::parameterList() {
  ParameterListContext *_localctx = _tracker.createInstance<ParameterListContext>(_ctx, getState());
  enterRule(_localctx, 52, CHTLJSParser::RuleParameterList);
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
    setState(278);
    match(CHTLJSParser::IDENTIFIER);
    setState(283);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::COMMA) {
      setState(279);
      match(CHTLJSParser::COMMA);
      setState(280);
      match(CHTLJSParser::IDENTIFIER);
      setState(285);
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

//----------------- ObjectExpressionContext ------------------------------------------------------------------

CHTLJSParser::ObjectExpressionContext::ObjectExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ObjectExpressionContext::LEFT_BRACE() {
  return getToken(CHTLJSParser::LEFT_BRACE, 0);
}

tree::TerminalNode* CHTLJSParser::ObjectExpressionContext::RIGHT_BRACE() {
  return getToken(CHTLJSParser::RIGHT_BRACE, 0);
}

std::vector<CHTLJSParser::PropertyAssignmentContext *> CHTLJSParser::ObjectExpressionContext::propertyAssignment() {
  return getRuleContexts<CHTLJSParser::PropertyAssignmentContext>();
}

CHTLJSParser::PropertyAssignmentContext* CHTLJSParser::ObjectExpressionContext::propertyAssignment(size_t i) {
  return getRuleContext<CHTLJSParser::PropertyAssignmentContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::ObjectExpressionContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::ObjectExpressionContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::ObjectExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleObjectExpression;
}

void CHTLJSParser::ObjectExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterObjectExpression(this);
}

void CHTLJSParser::ObjectExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitObjectExpression(this);
}


std::any CHTLJSParser::ObjectExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitObjectExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ObjectExpressionContext* CHTLJSParser::objectExpression() {
  ObjectExpressionContext *_localctx = _tracker.createInstance<ObjectExpressionContext>(_ctx, getState());
  enterRule(_localctx, 54, CHTLJSParser::RuleObjectExpression);
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
    setState(286);
    match(CHTLJSParser::LEFT_BRACE);
    setState(295);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4755801206570352640) != 0)) {
      setState(287);
      propertyAssignment();
      setState(292);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == CHTLJSParser::COMMA) {
        setState(288);
        match(CHTLJSParser::COMMA);
        setState(289);
        propertyAssignment();
        setState(294);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(297);
    match(CHTLJSParser::RIGHT_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyAssignmentContext ------------------------------------------------------------------

CHTLJSParser::PropertyAssignmentContext::PropertyAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CHTLJSParser::PropertyNameContext* CHTLJSParser::PropertyAssignmentContext::propertyName() {
  return getRuleContext<CHTLJSParser::PropertyNameContext>(0);
}

tree::TerminalNode* CHTLJSParser::PropertyAssignmentContext::COLON() {
  return getToken(CHTLJSParser::COLON, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::PropertyAssignmentContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}


size_t CHTLJSParser::PropertyAssignmentContext::getRuleIndex() const {
  return CHTLJSParser::RulePropertyAssignment;
}

void CHTLJSParser::PropertyAssignmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyAssignment(this);
}

void CHTLJSParser::PropertyAssignmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyAssignment(this);
}


std::any CHTLJSParser::PropertyAssignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitPropertyAssignment(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::PropertyAssignmentContext* CHTLJSParser::propertyAssignment() {
  PropertyAssignmentContext *_localctx = _tracker.createInstance<PropertyAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 56, CHTLJSParser::RulePropertyAssignment);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(299);
    propertyName();
    setState(300);
    match(CHTLJSParser::COLON);
    setState(301);
    expression();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyNameContext ------------------------------------------------------------------

CHTLJSParser::PropertyNameContext::PropertyNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::PropertyNameContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLJSParser::PropertyNameContext::STRING() {
  return getToken(CHTLJSParser::STRING, 0);
}

tree::TerminalNode* CHTLJSParser::PropertyNameContext::LEFT_BRACKET() {
  return getToken(CHTLJSParser::LEFT_BRACKET, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::PropertyNameContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::PropertyNameContext::RIGHT_BRACKET() {
  return getToken(CHTLJSParser::RIGHT_BRACKET, 0);
}


size_t CHTLJSParser::PropertyNameContext::getRuleIndex() const {
  return CHTLJSParser::RulePropertyName;
}

void CHTLJSParser::PropertyNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPropertyName(this);
}

void CHTLJSParser::PropertyNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPropertyName(this);
}


std::any CHTLJSParser::PropertyNameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitPropertyName(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::PropertyNameContext* CHTLJSParser::propertyName() {
  PropertyNameContext *_localctx = _tracker.createInstance<PropertyNameContext>(_ctx, getState());
  enterRule(_localctx, 58, CHTLJSParser::RulePropertyName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(309);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(303);
        match(CHTLJSParser::IDENTIFIER);
        break;
      }

      case CHTLJSParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(304);
        match(CHTLJSParser::STRING);
        break;
      }

      case CHTLJSParser::LEFT_BRACKET: {
        enterOuterAlt(_localctx, 3);
        setState(305);
        match(CHTLJSParser::LEFT_BRACKET);
        setState(306);
        expression();
        setState(307);
        match(CHTLJSParser::RIGHT_BRACKET);
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

//----------------- ArrayExpressionContext ------------------------------------------------------------------

CHTLJSParser::ArrayExpressionContext::ArrayExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ArrayExpressionContext::LEFT_BRACKET() {
  return getToken(CHTLJSParser::LEFT_BRACKET, 0);
}

tree::TerminalNode* CHTLJSParser::ArrayExpressionContext::RIGHT_BRACKET() {
  return getToken(CHTLJSParser::RIGHT_BRACKET, 0);
}

std::vector<CHTLJSParser::ExpressionContext *> CHTLJSParser::ArrayExpressionContext::expression() {
  return getRuleContexts<CHTLJSParser::ExpressionContext>();
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ArrayExpressionContext::expression(size_t i) {
  return getRuleContext<CHTLJSParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::ArrayExpressionContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::ArrayExpressionContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::ArrayExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleArrayExpression;
}

void CHTLJSParser::ArrayExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayExpression(this);
}

void CHTLJSParser::ArrayExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayExpression(this);
}


std::any CHTLJSParser::ArrayExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitArrayExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ArrayExpressionContext* CHTLJSParser::arrayExpression() {
  ArrayExpressionContext *_localctx = _tracker.createInstance<ArrayExpressionContext>(_ctx, getState());
  enterRule(_localctx, 60, CHTLJSParser::RuleArrayExpression);
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
    setState(311);
    match(CHTLJSParser::LEFT_BRACKET);
    setState(320);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 5183643452445818881) != 0)) {
      setState(312);
      expression();
      setState(317);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == CHTLJSParser::COMMA) {
        setState(313);
        match(CHTLJSParser::COMMA);
        setState(314);
        expression();
        setState(319);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(322);
    match(CHTLJSParser::RIGHT_BRACKET);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParenthesizedExpressionContext ------------------------------------------------------------------

CHTLJSParser::ParenthesizedExpressionContext::ParenthesizedExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ParenthesizedExpressionContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ParenthesizedExpressionContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::ParenthesizedExpressionContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::ParenthesizedExpressionContext::getRuleIndex() const {
  return CHTLJSParser::RuleParenthesizedExpression;
}

void CHTLJSParser::ParenthesizedExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenthesizedExpression(this);
}

void CHTLJSParser::ParenthesizedExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParenthesizedExpression(this);
}


std::any CHTLJSParser::ParenthesizedExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitParenthesizedExpression(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ParenthesizedExpressionContext* CHTLJSParser::parenthesizedExpression() {
  ParenthesizedExpressionContext *_localctx = _tracker.createInstance<ParenthesizedExpressionContext>(_ctx, getState());
  enterRule(_localctx, 62, CHTLJSParser::RuleParenthesizedExpression);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(324);
    match(CHTLJSParser::LEFT_PAREN);
    setState(325);
    expression();
    setState(326);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableDeclarationContext ------------------------------------------------------------------

CHTLJSParser::VariableDeclarationContext::VariableDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::VariableDeclaratorContext *> CHTLJSParser::VariableDeclarationContext::variableDeclarator() {
  return getRuleContexts<CHTLJSParser::VariableDeclaratorContext>();
}

CHTLJSParser::VariableDeclaratorContext* CHTLJSParser::VariableDeclarationContext::variableDeclarator(size_t i) {
  return getRuleContext<CHTLJSParser::VariableDeclaratorContext>(i);
}

tree::TerminalNode* CHTLJSParser::VariableDeclarationContext::VAR() {
  return getToken(CHTLJSParser::VAR, 0);
}

tree::TerminalNode* CHTLJSParser::VariableDeclarationContext::LET() {
  return getToken(CHTLJSParser::LET, 0);
}

tree::TerminalNode* CHTLJSParser::VariableDeclarationContext::CONST() {
  return getToken(CHTLJSParser::CONST, 0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::VariableDeclarationContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::VariableDeclarationContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::VariableDeclarationContext::getRuleIndex() const {
  return CHTLJSParser::RuleVariableDeclaration;
}

void CHTLJSParser::VariableDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclaration(this);
}

void CHTLJSParser::VariableDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclaration(this);
}


std::any CHTLJSParser::VariableDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitVariableDeclaration(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::VariableDeclarationContext* CHTLJSParser::variableDeclaration() {
  VariableDeclarationContext *_localctx = _tracker.createInstance<VariableDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 64, CHTLJSParser::RuleVariableDeclaration);
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
    setState(328);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 14) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(329);
    variableDeclarator();
    setState(334);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::COMMA) {
      setState(330);
      match(CHTLJSParser::COMMA);
      setState(331);
      variableDeclarator();
      setState(336);
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

//----------------- VariableDeclaratorContext ------------------------------------------------------------------

CHTLJSParser::VariableDeclaratorContext::VariableDeclaratorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::VariableDeclaratorContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLJSParser::VariableDeclaratorContext::EQUAL() {
  return getToken(CHTLJSParser::EQUAL, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::VariableDeclaratorContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}


size_t CHTLJSParser::VariableDeclaratorContext::getRuleIndex() const {
  return CHTLJSParser::RuleVariableDeclarator;
}

void CHTLJSParser::VariableDeclaratorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableDeclarator(this);
}

void CHTLJSParser::VariableDeclaratorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableDeclarator(this);
}


std::any CHTLJSParser::VariableDeclaratorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitVariableDeclarator(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::VariableDeclaratorContext* CHTLJSParser::variableDeclarator() {
  VariableDeclaratorContext *_localctx = _tracker.createInstance<VariableDeclaratorContext>(_ctx, getState());
  enterRule(_localctx, 66, CHTLJSParser::RuleVariableDeclarator);
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
    setState(337);
    match(CHTLJSParser::IDENTIFIER);
    setState(340);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLJSParser::EQUAL) {
      setState(338);
      match(CHTLJSParser::EQUAL);
      setState(339);
      expression();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionDeclarationContext ------------------------------------------------------------------

CHTLJSParser::FunctionDeclarationContext::FunctionDeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::FunctionDeclarationContext::FUNCTION() {
  return getToken(CHTLJSParser::FUNCTION, 0);
}

tree::TerminalNode* CHTLJSParser::FunctionDeclarationContext::IDENTIFIER() {
  return getToken(CHTLJSParser::IDENTIFIER, 0);
}

tree::TerminalNode* CHTLJSParser::FunctionDeclarationContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

tree::TerminalNode* CHTLJSParser::FunctionDeclarationContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

CHTLJSParser::BlockContext* CHTLJSParser::FunctionDeclarationContext::block() {
  return getRuleContext<CHTLJSParser::BlockContext>(0);
}

CHTLJSParser::ParameterListContext* CHTLJSParser::FunctionDeclarationContext::parameterList() {
  return getRuleContext<CHTLJSParser::ParameterListContext>(0);
}


size_t CHTLJSParser::FunctionDeclarationContext::getRuleIndex() const {
  return CHTLJSParser::RuleFunctionDeclaration;
}

void CHTLJSParser::FunctionDeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionDeclaration(this);
}

void CHTLJSParser::FunctionDeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionDeclaration(this);
}


std::any CHTLJSParser::FunctionDeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitFunctionDeclaration(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::FunctionDeclarationContext* CHTLJSParser::functionDeclaration() {
  FunctionDeclarationContext *_localctx = _tracker.createInstance<FunctionDeclarationContext>(_ctx, getState());
  enterRule(_localctx, 68, CHTLJSParser::RuleFunctionDeclaration);
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
    setState(342);
    match(CHTLJSParser::FUNCTION);
    setState(343);
    match(CHTLJSParser::IDENTIFIER);
    setState(344);
    match(CHTLJSParser::LEFT_PAREN);
    setState(346);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLJSParser::IDENTIFIER) {
      setState(345);
      parameterList();
    }
    setState(348);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(349);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfStatementContext ------------------------------------------------------------------

CHTLJSParser::IfStatementContext::IfStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::IfStatementContext::IF() {
  return getToken(CHTLJSParser::IF, 0);
}

tree::TerminalNode* CHTLJSParser::IfStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::IfStatementContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::IfStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

std::vector<CHTLJSParser::StatementContext *> CHTLJSParser::IfStatementContext::statement() {
  return getRuleContexts<CHTLJSParser::StatementContext>();
}

CHTLJSParser::StatementContext* CHTLJSParser::IfStatementContext::statement(size_t i) {
  return getRuleContext<CHTLJSParser::StatementContext>(i);
}

tree::TerminalNode* CHTLJSParser::IfStatementContext::ELSE() {
  return getToken(CHTLJSParser::ELSE, 0);
}


size_t CHTLJSParser::IfStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleIfStatement;
}

void CHTLJSParser::IfStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfStatement(this);
}

void CHTLJSParser::IfStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfStatement(this);
}


std::any CHTLJSParser::IfStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitIfStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::IfStatementContext* CHTLJSParser::ifStatement() {
  IfStatementContext *_localctx = _tracker.createInstance<IfStatementContext>(_ctx, getState());
  enterRule(_localctx, 70, CHTLJSParser::RuleIfStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(351);
    match(CHTLJSParser::IF);
    setState(352);
    match(CHTLJSParser::LEFT_PAREN);
    setState(353);
    expression();
    setState(354);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(355);
    statement();
    setState(358);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx)) {
    case 1: {
      setState(356);
      match(CHTLJSParser::ELSE);
      setState(357);
      statement();
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

//----------------- ForStatementContext ------------------------------------------------------------------

CHTLJSParser::ForStatementContext::ForStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ForStatementContext::FOR() {
  return getToken(CHTLJSParser::FOR, 0);
}

tree::TerminalNode* CHTLJSParser::ForStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::ForStatementContext::SEMICOLON() {
  return getTokens(CHTLJSParser::SEMICOLON);
}

tree::TerminalNode* CHTLJSParser::ForStatementContext::SEMICOLON(size_t i) {
  return getToken(CHTLJSParser::SEMICOLON, i);
}

tree::TerminalNode* CHTLJSParser::ForStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

CHTLJSParser::StatementContext* CHTLJSParser::ForStatementContext::statement() {
  return getRuleContext<CHTLJSParser::StatementContext>(0);
}

CHTLJSParser::VariableDeclarationContext* CHTLJSParser::ForStatementContext::variableDeclaration() {
  return getRuleContext<CHTLJSParser::VariableDeclarationContext>(0);
}

CHTLJSParser::ExpressionStatementContext* CHTLJSParser::ForStatementContext::expressionStatement() {
  return getRuleContext<CHTLJSParser::ExpressionStatementContext>(0);
}

std::vector<CHTLJSParser::ExpressionContext *> CHTLJSParser::ForStatementContext::expression() {
  return getRuleContexts<CHTLJSParser::ExpressionContext>();
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ForStatementContext::expression(size_t i) {
  return getRuleContext<CHTLJSParser::ExpressionContext>(i);
}


size_t CHTLJSParser::ForStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleForStatement;
}

void CHTLJSParser::ForStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForStatement(this);
}

void CHTLJSParser::ForStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForStatement(this);
}


std::any CHTLJSParser::ForStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitForStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ForStatementContext* CHTLJSParser::forStatement() {
  ForStatementContext *_localctx = _tracker.createInstance<ForStatementContext>(_ctx, getState());
  enterRule(_localctx, 72, CHTLJSParser::RuleForStatement);
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
    setState(360);
    match(CHTLJSParser::FOR);
    setState(361);
    match(CHTLJSParser::LEFT_PAREN);
    setState(365);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CHTLJSParser::VAR:
      case CHTLJSParser::LET:
      case CHTLJSParser::CONST: {
        setState(362);
        variableDeclaration();
        break;
      }

      case CHTLJSParser::FUNCTION:
      case CHTLJSParser::LEFT_BRACE:
      case CHTLJSParser::LEFT_BRACKET:
      case CHTLJSParser::LEFT_PAREN:
      case CHTLJSParser::PLUS:
      case CHTLJSParser::MINUS:
      case CHTLJSParser::NOT:
      case CHTLJSParser::DOUBLE_BRACE:
      case CHTLJSParser::STRING:
      case CHTLJSParser::NUMBER:
      case CHTLJSParser::BOOLEAN:
      case CHTLJSParser::NULL_:
      case CHTLJSParser::UNDEFINED:
      case CHTLJSParser::IDENTIFIER:
      case CHTLJSParser::TILDE: {
        setState(363);
        expressionStatement();
        break;
      }

      case CHTLJSParser::SEMICOLON: {
        setState(364);
        match(CHTLJSParser::SEMICOLON);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(368);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 5183643452445818881) != 0)) {
      setState(367);
      expression();
    }
    setState(370);
    match(CHTLJSParser::SEMICOLON);
    setState(372);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 4) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 4)) & 5183643452445818881) != 0)) {
      setState(371);
      expression();
    }
    setState(374);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(375);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhileStatementContext ------------------------------------------------------------------

CHTLJSParser::WhileStatementContext::WhileStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::WhileStatementContext::WHILE() {
  return getToken(CHTLJSParser::WHILE, 0);
}

tree::TerminalNode* CHTLJSParser::WhileStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::WhileStatementContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::WhileStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

CHTLJSParser::StatementContext* CHTLJSParser::WhileStatementContext::statement() {
  return getRuleContext<CHTLJSParser::StatementContext>(0);
}


size_t CHTLJSParser::WhileStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleWhileStatement;
}

void CHTLJSParser::WhileStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWhileStatement(this);
}

void CHTLJSParser::WhileStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWhileStatement(this);
}


std::any CHTLJSParser::WhileStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitWhileStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::WhileStatementContext* CHTLJSParser::whileStatement() {
  WhileStatementContext *_localctx = _tracker.createInstance<WhileStatementContext>(_ctx, getState());
  enterRule(_localctx, 74, CHTLJSParser::RuleWhileStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(377);
    match(CHTLJSParser::WHILE);
    setState(378);
    match(CHTLJSParser::LEFT_PAREN);
    setState(379);
    expression();
    setState(380);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(381);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SwitchStatementContext ------------------------------------------------------------------

CHTLJSParser::SwitchStatementContext::SwitchStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::SwitchStatementContext::SWITCH() {
  return getToken(CHTLJSParser::SWITCH, 0);
}

tree::TerminalNode* CHTLJSParser::SwitchStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::SwitchStatementContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::SwitchStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

tree::TerminalNode* CHTLJSParser::SwitchStatementContext::LEFT_BRACE() {
  return getToken(CHTLJSParser::LEFT_BRACE, 0);
}

tree::TerminalNode* CHTLJSParser::SwitchStatementContext::RIGHT_BRACE() {
  return getToken(CHTLJSParser::RIGHT_BRACE, 0);
}

std::vector<CHTLJSParser::CaseClauseContext *> CHTLJSParser::SwitchStatementContext::caseClause() {
  return getRuleContexts<CHTLJSParser::CaseClauseContext>();
}

CHTLJSParser::CaseClauseContext* CHTLJSParser::SwitchStatementContext::caseClause(size_t i) {
  return getRuleContext<CHTLJSParser::CaseClauseContext>(i);
}

CHTLJSParser::DefaultClauseContext* CHTLJSParser::SwitchStatementContext::defaultClause() {
  return getRuleContext<CHTLJSParser::DefaultClauseContext>(0);
}


size_t CHTLJSParser::SwitchStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleSwitchStatement;
}

void CHTLJSParser::SwitchStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSwitchStatement(this);
}

void CHTLJSParser::SwitchStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSwitchStatement(this);
}


std::any CHTLJSParser::SwitchStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitSwitchStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::SwitchStatementContext* CHTLJSParser::switchStatement() {
  SwitchStatementContext *_localctx = _tracker.createInstance<SwitchStatementContext>(_ctx, getState());
  enterRule(_localctx, 76, CHTLJSParser::RuleSwitchStatement);
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
    setState(383);
    match(CHTLJSParser::SWITCH);
    setState(384);
    match(CHTLJSParser::LEFT_PAREN);
    setState(385);
    expression();
    setState(386);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(387);
    match(CHTLJSParser::LEFT_BRACE);
    setState(391);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::CASE) {
      setState(388);
      caseClause();
      setState(393);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(395);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CHTLJSParser::DEFAULT) {
      setState(394);
      defaultClause();
    }
    setState(397);
    match(CHTLJSParser::RIGHT_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaseClauseContext ------------------------------------------------------------------

CHTLJSParser::CaseClauseContext::CaseClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::CaseClauseContext::CASE() {
  return getToken(CHTLJSParser::CASE, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::CaseClauseContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::CaseClauseContext::COLON() {
  return getToken(CHTLJSParser::COLON, 0);
}

std::vector<CHTLJSParser::StatementContext *> CHTLJSParser::CaseClauseContext::statement() {
  return getRuleContexts<CHTLJSParser::StatementContext>();
}

CHTLJSParser::StatementContext* CHTLJSParser::CaseClauseContext::statement(size_t i) {
  return getRuleContext<CHTLJSParser::StatementContext>(i);
}


size_t CHTLJSParser::CaseClauseContext::getRuleIndex() const {
  return CHTLJSParser::RuleCaseClause;
}

void CHTLJSParser::CaseClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCaseClause(this);
}

void CHTLJSParser::CaseClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCaseClause(this);
}


std::any CHTLJSParser::CaseClauseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitCaseClause(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::CaseClauseContext* CHTLJSParser::caseClause() {
  CaseClauseContext *_localctx = _tracker.createInstance<CaseClauseContext>(_ctx, getState());
  enterRule(_localctx, 78, CHTLJSParser::RuleCaseClause);
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
    setState(399);
    match(CHTLJSParser::CASE);
    setState(400);
    expression();
    setState(401);
    match(CHTLJSParser::COLON);
    setState(405);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 9151318944303219646) != 0) || _la == CHTLJSParser::TILDE) {
      setState(402);
      statement();
      setState(407);
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

//----------------- DefaultClauseContext ------------------------------------------------------------------

CHTLJSParser::DefaultClauseContext::DefaultClauseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::DefaultClauseContext::DEFAULT() {
  return getToken(CHTLJSParser::DEFAULT, 0);
}

tree::TerminalNode* CHTLJSParser::DefaultClauseContext::COLON() {
  return getToken(CHTLJSParser::COLON, 0);
}

std::vector<CHTLJSParser::StatementContext *> CHTLJSParser::DefaultClauseContext::statement() {
  return getRuleContexts<CHTLJSParser::StatementContext>();
}

CHTLJSParser::StatementContext* CHTLJSParser::DefaultClauseContext::statement(size_t i) {
  return getRuleContext<CHTLJSParser::StatementContext>(i);
}


size_t CHTLJSParser::DefaultClauseContext::getRuleIndex() const {
  return CHTLJSParser::RuleDefaultClause;
}

void CHTLJSParser::DefaultClauseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefaultClause(this);
}

void CHTLJSParser::DefaultClauseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefaultClause(this);
}


std::any CHTLJSParser::DefaultClauseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitDefaultClause(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::DefaultClauseContext* CHTLJSParser::defaultClause() {
  DefaultClauseContext *_localctx = _tracker.createInstance<DefaultClauseContext>(_ctx, getState());
  enterRule(_localctx, 80, CHTLJSParser::RuleDefaultClause);
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
    setState(408);
    match(CHTLJSParser::DEFAULT);
    setState(409);
    match(CHTLJSParser::COLON);
    setState(413);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 9151318944303219646) != 0) || _la == CHTLJSParser::TILDE) {
      setState(410);
      statement();
      setState(415);
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

//----------------- FileloaderStatementContext ------------------------------------------------------------------

CHTLJSParser::FileloaderStatementContext::FileloaderStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::FileloaderStatementContext::FILELOADER() {
  return getToken(CHTLJSParser::FILELOADER, 0);
}

tree::TerminalNode* CHTLJSParser::FileloaderStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::FileloaderStatementContext::STRING() {
  return getTokens(CHTLJSParser::STRING);
}

tree::TerminalNode* CHTLJSParser::FileloaderStatementContext::STRING(size_t i) {
  return getToken(CHTLJSParser::STRING, i);
}

tree::TerminalNode* CHTLJSParser::FileloaderStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}

CHTLJSParser::BlockContext* CHTLJSParser::FileloaderStatementContext::block() {
  return getRuleContext<CHTLJSParser::BlockContext>(0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::FileloaderStatementContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::FileloaderStatementContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::FileloaderStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleFileloaderStatement;
}

void CHTLJSParser::FileloaderStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFileloaderStatement(this);
}

void CHTLJSParser::FileloaderStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFileloaderStatement(this);
}


std::any CHTLJSParser::FileloaderStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitFileloaderStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::FileloaderStatementContext* CHTLJSParser::fileloaderStatement() {
  FileloaderStatementContext *_localctx = _tracker.createInstance<FileloaderStatementContext>(_ctx, getState());
  enterRule(_localctx, 82, CHTLJSParser::RuleFileloaderStatement);
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
    setState(416);
    match(CHTLJSParser::FILELOADER);
    setState(417);
    match(CHTLJSParser::LEFT_PAREN);
    setState(418);
    match(CHTLJSParser::STRING);
    setState(423);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::COMMA) {
      setState(419);
      match(CHTLJSParser::COMMA);
      setState(420);
      match(CHTLJSParser::STRING);
      setState(425);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(426);
    match(CHTLJSParser::RIGHT_PAREN);
    setState(427);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ListenStatementContext ------------------------------------------------------------------

CHTLJSParser::ListenStatementContext::ListenStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::ListenStatementContext::LISTEN() {
  return getToken(CHTLJSParser::LISTEN, 0);
}

tree::TerminalNode* CHTLJSParser::ListenStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::ListenStatementContext::chthljsSelector() {
  return getRuleContext<CHTLJSParser::ChthljsSelectorContext>(0);
}

tree::TerminalNode* CHTLJSParser::ListenStatementContext::COMMA() {
  return getToken(CHTLJSParser::COMMA, 0);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::ListenStatementContext::functionExpression() {
  return getRuleContext<CHTLJSParser::FunctionExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::ListenStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::ListenStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleListenStatement;
}

void CHTLJSParser::ListenStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterListenStatement(this);
}

void CHTLJSParser::ListenStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitListenStatement(this);
}


std::any CHTLJSParser::ListenStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitListenStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ListenStatementContext* CHTLJSParser::listenStatement() {
  ListenStatementContext *_localctx = _tracker.createInstance<ListenStatementContext>(_ctx, getState());
  enterRule(_localctx, 84, CHTLJSParser::RuleListenStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(429);
    match(CHTLJSParser::LISTEN);
    setState(430);
    match(CHTLJSParser::LEFT_PAREN);
    setState(431);
    chthljsSelector();
    setState(432);
    match(CHTLJSParser::COMMA);
    setState(433);
    functionExpression();
    setState(434);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DelegateStatementContext ------------------------------------------------------------------

CHTLJSParser::DelegateStatementContext::DelegateStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::DelegateStatementContext::DELEGATE() {
  return getToken(CHTLJSParser::DELEGATE, 0);
}

tree::TerminalNode* CHTLJSParser::DelegateStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::DelegateStatementContext::chthljsSelector() {
  return getRuleContext<CHTLJSParser::ChthljsSelectorContext>(0);
}

std::vector<tree::TerminalNode *> CHTLJSParser::DelegateStatementContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::DelegateStatementContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}

tree::TerminalNode* CHTLJSParser::DelegateStatementContext::STRING() {
  return getToken(CHTLJSParser::STRING, 0);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::DelegateStatementContext::functionExpression() {
  return getRuleContext<CHTLJSParser::FunctionExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::DelegateStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::DelegateStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleDelegateStatement;
}

void CHTLJSParser::DelegateStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDelegateStatement(this);
}

void CHTLJSParser::DelegateStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDelegateStatement(this);
}


std::any CHTLJSParser::DelegateStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitDelegateStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::DelegateStatementContext* CHTLJSParser::delegateStatement() {
  DelegateStatementContext *_localctx = _tracker.createInstance<DelegateStatementContext>(_ctx, getState());
  enterRule(_localctx, 86, CHTLJSParser::RuleDelegateStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(436);
    match(CHTLJSParser::DELEGATE);
    setState(437);
    match(CHTLJSParser::LEFT_PAREN);
    setState(438);
    chthljsSelector();
    setState(439);
    match(CHTLJSParser::COMMA);
    setState(440);
    match(CHTLJSParser::STRING);
    setState(441);
    match(CHTLJSParser::COMMA);
    setState(442);
    functionExpression();
    setState(443);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AnimateStatementContext ------------------------------------------------------------------

CHTLJSParser::AnimateStatementContext::AnimateStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::AnimateStatementContext::ANIMATE() {
  return getToken(CHTLJSParser::ANIMATE, 0);
}

tree::TerminalNode* CHTLJSParser::AnimateStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::AnimateStatementContext::chthljsSelector() {
  return getRuleContext<CHTLJSParser::ChthljsSelectorContext>(0);
}

tree::TerminalNode* CHTLJSParser::AnimateStatementContext::COMMA() {
  return getToken(CHTLJSParser::COMMA, 0);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::AnimateStatementContext::functionExpression() {
  return getRuleContext<CHTLJSParser::FunctionExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::AnimateStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::AnimateStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleAnimateStatement;
}

void CHTLJSParser::AnimateStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnimateStatement(this);
}

void CHTLJSParser::AnimateStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnimateStatement(this);
}


std::any CHTLJSParser::AnimateStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitAnimateStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::AnimateStatementContext* CHTLJSParser::animateStatement() {
  AnimateStatementContext *_localctx = _tracker.createInstance<AnimateStatementContext>(_ctx, getState());
  enterRule(_localctx, 88, CHTLJSParser::RuleAnimateStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(445);
    match(CHTLJSParser::ANIMATE);
    setState(446);
    match(CHTLJSParser::LEFT_PAREN);
    setState(447);
    chthljsSelector();
    setState(448);
    match(CHTLJSParser::COMMA);
    setState(449);
    functionExpression();
    setState(450);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VirStatementContext ------------------------------------------------------------------

CHTLJSParser::VirStatementContext::VirStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::VirStatementContext::VIR() {
  return getToken(CHTLJSParser::VIR, 0);
}

tree::TerminalNode* CHTLJSParser::VirStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

CHTLJSParser::ChthljsSelectorContext* CHTLJSParser::VirStatementContext::chthljsSelector() {
  return getRuleContext<CHTLJSParser::ChthljsSelectorContext>(0);
}

tree::TerminalNode* CHTLJSParser::VirStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::VirStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleVirStatement;
}

void CHTLJSParser::VirStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVirStatement(this);
}

void CHTLJSParser::VirStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVirStatement(this);
}


std::any CHTLJSParser::VirStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitVirStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::VirStatementContext* CHTLJSParser::virStatement() {
  VirStatementContext *_localctx = _tracker.createInstance<VirStatementContext>(_ctx, getState());
  enterRule(_localctx, 90, CHTLJSParser::RuleVirStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(452);
    match(CHTLJSParser::VIR);
    setState(453);
    match(CHTLJSParser::LEFT_PAREN);
    setState(454);
    chthljsSelector();
    setState(455);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RouterStatementContext ------------------------------------------------------------------

CHTLJSParser::RouterStatementContext::RouterStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::RouterStatementContext::ROUTER() {
  return getToken(CHTLJSParser::ROUTER, 0);
}

tree::TerminalNode* CHTLJSParser::RouterStatementContext::LEFT_PAREN() {
  return getToken(CHTLJSParser::LEFT_PAREN, 0);
}

tree::TerminalNode* CHTLJSParser::RouterStatementContext::STRING() {
  return getToken(CHTLJSParser::STRING, 0);
}

tree::TerminalNode* CHTLJSParser::RouterStatementContext::COMMA() {
  return getToken(CHTLJSParser::COMMA, 0);
}

CHTLJSParser::FunctionExpressionContext* CHTLJSParser::RouterStatementContext::functionExpression() {
  return getRuleContext<CHTLJSParser::FunctionExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::RouterStatementContext::RIGHT_PAREN() {
  return getToken(CHTLJSParser::RIGHT_PAREN, 0);
}


size_t CHTLJSParser::RouterStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleRouterStatement;
}

void CHTLJSParser::RouterStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRouterStatement(this);
}

void CHTLJSParser::RouterStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRouterStatement(this);
}


std::any CHTLJSParser::RouterStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitRouterStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::RouterStatementContext* CHTLJSParser::routerStatement() {
  RouterStatementContext *_localctx = _tracker.createInstance<RouterStatementContext>(_ctx, getState());
  enterRule(_localctx, 92, CHTLJSParser::RuleRouterStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(457);
    match(CHTLJSParser::ROUTER);
    setState(458);
    match(CHTLJSParser::LEFT_PAREN);
    setState(459);
    match(CHTLJSParser::STRING);
    setState(460);
    match(CHTLJSParser::COMMA);
    setState(461);
    functionExpression();
    setState(462);
    match(CHTLJSParser::RIGHT_PAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UtilStatementContext ------------------------------------------------------------------

CHTLJSParser::UtilStatementContext::UtilStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::UtilStatementContext::UTIL() {
  return getToken(CHTLJSParser::UTIL, 0);
}

CHTLJSParser::ExpressionContext* CHTLJSParser::UtilStatementContext::expression() {
  return getRuleContext<CHTLJSParser::ExpressionContext>(0);
}

tree::TerminalNode* CHTLJSParser::UtilStatementContext::THEN() {
  return getToken(CHTLJSParser::THEN, 0);
}

CHTLJSParser::StatementContext* CHTLJSParser::UtilStatementContext::statement() {
  return getRuleContext<CHTLJSParser::StatementContext>(0);
}


size_t CHTLJSParser::UtilStatementContext::getRuleIndex() const {
  return CHTLJSParser::RuleUtilStatement;
}

void CHTLJSParser::UtilStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUtilStatement(this);
}

void CHTLJSParser::UtilStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUtilStatement(this);
}


std::any CHTLJSParser::UtilStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitUtilStatement(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::UtilStatementContext* CHTLJSParser::utilStatement() {
  UtilStatementContext *_localctx = _tracker.createInstance<UtilStatementContext>(_ctx, getState());
  enterRule(_localctx, 94, CHTLJSParser::RuleUtilStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(464);
    match(CHTLJSParser::UTIL);
    setState(465);
    expression();
    setState(466);
    match(CHTLJSParser::THEN);
    setState(467);
    statement();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

CHTLJSParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::BlockContext::LEFT_BRACE() {
  return getToken(CHTLJSParser::LEFT_BRACE, 0);
}

tree::TerminalNode* CHTLJSParser::BlockContext::RIGHT_BRACE() {
  return getToken(CHTLJSParser::RIGHT_BRACE, 0);
}

std::vector<CHTLJSParser::StatementContext *> CHTLJSParser::BlockContext::statement() {
  return getRuleContexts<CHTLJSParser::StatementContext>();
}

CHTLJSParser::StatementContext* CHTLJSParser::BlockContext::statement(size_t i) {
  return getRuleContext<CHTLJSParser::StatementContext>(i);
}


size_t CHTLJSParser::BlockContext::getRuleIndex() const {
  return CHTLJSParser::RuleBlock;
}

void CHTLJSParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void CHTLJSParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}


std::any CHTLJSParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::BlockContext* CHTLJSParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 96, CHTLJSParser::RuleBlock);
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
    setState(469);
    match(CHTLJSParser::LEFT_BRACE);
    setState(473);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 9151318944303219646) != 0) || _la == CHTLJSParser::TILDE) {
      setState(470);
      statement();
      setState(475);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(476);
    match(CHTLJSParser::RIGHT_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentListContext ------------------------------------------------------------------

CHTLJSParser::ArgumentListContext::ArgumentListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CHTLJSParser::ExpressionContext *> CHTLJSParser::ArgumentListContext::expression() {
  return getRuleContexts<CHTLJSParser::ExpressionContext>();
}

CHTLJSParser::ExpressionContext* CHTLJSParser::ArgumentListContext::expression(size_t i) {
  return getRuleContext<CHTLJSParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> CHTLJSParser::ArgumentListContext::COMMA() {
  return getTokens(CHTLJSParser::COMMA);
}

tree::TerminalNode* CHTLJSParser::ArgumentListContext::COMMA(size_t i) {
  return getToken(CHTLJSParser::COMMA, i);
}


size_t CHTLJSParser::ArgumentListContext::getRuleIndex() const {
  return CHTLJSParser::RuleArgumentList;
}

void CHTLJSParser::ArgumentListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgumentList(this);
}

void CHTLJSParser::ArgumentListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgumentList(this);
}


std::any CHTLJSParser::ArgumentListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitArgumentList(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::ArgumentListContext* CHTLJSParser::argumentList() {
  ArgumentListContext *_localctx = _tracker.createInstance<ArgumentListContext>(_ctx, getState());
  enterRule(_localctx, 98, CHTLJSParser::RuleArgumentList);
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
    setState(478);
    expression();
    setState(483);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CHTLJSParser::COMMA) {
      setState(479);
      match(CHTLJSParser::COMMA);
      setState(480);
      expression();
      setState(485);
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

//----------------- AssignmentOperatorContext ------------------------------------------------------------------

CHTLJSParser::AssignmentOperatorContext::AssignmentOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::EQUAL() {
  return getToken(CHTLJSParser::EQUAL, 0);
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::PLUS_EQUAL() {
  return getToken(CHTLJSParser::PLUS_EQUAL, 0);
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::MINUS_EQUAL() {
  return getToken(CHTLJSParser::MINUS_EQUAL, 0);
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::MULTIPLY_EQUAL() {
  return getToken(CHTLJSParser::MULTIPLY_EQUAL, 0);
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::DIVIDE_EQUAL() {
  return getToken(CHTLJSParser::DIVIDE_EQUAL, 0);
}

tree::TerminalNode* CHTLJSParser::AssignmentOperatorContext::MODULO_EQUAL() {
  return getToken(CHTLJSParser::MODULO_EQUAL, 0);
}


size_t CHTLJSParser::AssignmentOperatorContext::getRuleIndex() const {
  return CHTLJSParser::RuleAssignmentOperator;
}

void CHTLJSParser::AssignmentOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignmentOperator(this);
}

void CHTLJSParser::AssignmentOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CHTLJSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignmentOperator(this);
}


std::any CHTLJSParser::AssignmentOperatorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<CHTLJSVisitor*>(visitor))
    return parserVisitor->visitAssignmentOperator(this);
  else
    return visitor->visitChildren(this);
}

CHTLJSParser::AssignmentOperatorContext* CHTLJSParser::assignmentOperator() {
  AssignmentOperatorContext *_localctx = _tracker.createInstance<AssignmentOperatorContext>(_ctx, getState());
  enterRule(_localctx, 100, CHTLJSParser::RuleAssignmentOperator);
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
    setState(486);
    _la = _input->LA(1);
    if (!(((((_la - 49) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 49)) & 262175) != 0))) {
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

void CHTLJSParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  chtljsParserInitialize();
#else
  ::antlr4::internal::call_once(chtljsParserOnceFlag, chtljsParserInitialize);
#endif
}
