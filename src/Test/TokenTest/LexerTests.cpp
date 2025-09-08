#include "CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

struct LexerTestCase {
    std::string name;
    std::string input;
    std::vector<TokenType> expectedTypes;
};

void runLexerTests() {
    std::cout << "--- Running Lexer Tests ---" << std::endl;

    std::vector<LexerTestCase> testCases = {
        {
            "Basic Elements",
            "html { body {} }",
            {TokenType::IDENTIFIER, TokenType::L_BRACE, TokenType::IDENTIFIER, TokenType::L_BRACE, TokenType::R_BRACE, TokenType::R_BRACE, TokenType::END_OF_FILE}
        },
        {
            "Attributes",
            "div { id: main; class=\"container\"; }",
            {TokenType::IDENTIFIER, TokenType::L_BRACE, TokenType::IDENTIFIER, TokenType::COLON, TokenType::IDENTIFIER, TokenType::SEMICOLON, TokenType::IDENTIFIER, TokenType::EQUALS, TokenType::STRING, TokenType::SEMICOLON, TokenType::R_BRACE, TokenType::END_OF_FILE}
        },
        {
            "Text block",
            "text { \"Hello World\" }",
            {TokenType::KEYWORD_TEXT, TokenType::L_BRACE, TokenType::STRING, TokenType::R_BRACE, TokenType::END_OF_FILE}
        },
        {
            "Comments",
            "// single line \n /* multi line */ -- generator \n html{}",
            {TokenType::IDENTIFIER, TokenType::L_BRACE, TokenType::R_BRACE, TokenType::END_OF_FILE}
        }
    };

    int passed = 0;
    for (const auto& testCase : testCases) {
        Lexer lexer(testCase.input);
        bool testPassed = true;
        for (size_t i = 0; i < testCase.expectedTypes.size(); ++i) {
            Token t = lexer.getNextToken();
            if (t.type != testCase.expectedTypes[i]) {
                std::cout << "[FAIL] " << testCase.name << " - Mismatch at token " << i << ". Expected " << (int)testCase.expectedTypes[i] << ", got " << (int)t.type << std::endl;
                testPassed = false;
                break;
            }
        }
        if (testPassed) {
            std::cout << "[PASS] " << testCase.name << std::endl;
            passed++;
        }
    }

    std::cout << "Lexer Tests Passed: " << passed << "/" << testCases.size() << std::endl;
    assert(passed == testCases.size());
}
