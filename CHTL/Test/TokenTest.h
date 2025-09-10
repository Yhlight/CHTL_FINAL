#ifndef CHTL_TOKEN_TEST_H
#define CHTL_TOKEN_TEST_H

#include "TestFramework.h"
#include "../CHTL/CHTLLexer/Token.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"

namespace CHTL {
namespace Test {

class TokenTest {
public:
    static std::unique_ptr<TestSuite> createTestSuite();
    
private:
    static void testTokenCreation();
    static void testTokenComparison();
    static void testTokenToString();
    static void testTokenTypeName();
    static void testTokenStream();
    static void testTokenStreamOperations();
    static void testTokenStreamFind();
    static void testTokenStreamSkip();
    static void testTokenStreamBacktrack();
    static void testLexerBasic();
    static void testLexerComments();
    static void testLexerStrings();
    static void testLexerNumbers();
    static void testLexerOperators();
    static void testLexerSymbols();
    static void testLexerKeywords();
    static void testLexerTemplates();
    static void testLexerColonEqual();
    static void testLexerElementNames();
    static void testLexerAttributeNames();
    static void testLexerTextContent();
    static void testLexerLiterals();
};

} // namespace Test
} // namespace CHTL

#endif // CHTL_TOKEN_TEST_H