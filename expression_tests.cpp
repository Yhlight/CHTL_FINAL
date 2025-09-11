#include "CHTL/CHTLExpressionEvaluator/ExpressionEvaluator.h"
#include "CHTL/CHTLExpressionParser/ExpressionParser.h"
#include "CHTL/CHTLLexer/Token.h"
#include <iostream>
#include <cassert>
#include <string_view>

// Simple test runner
void runTest(const std::string& testName, void (*testFunc)()) {
    std::cout << "Running test: " << testName << "..." << std::endl;
    try {
        testFunc();
        std::cout << "  [PASS]" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  [FAIL] Exception thrown: " << e.what() << std::endl;
        exit(1);
    }
}

// --- Test Case ---

void testConditionalWithContext() {
    // This test mimics the Generator's two-pass logic.

    // 1. Define the properties we are working with.
    // `width: 150px;`
    std::vector<Token> widthTokens = { {TokenType::NUMBER, "150", "", 1}, {TokenType::IDENTIFIER, "px", "", 1} };
    // `background-color: width > 100px ? red : blue;`
    std::vector<Token> bgTokens = {
        {TokenType::IDENTIFIER, "width", "", 1},
        {TokenType::GREATER, ">", "", 1},
        {TokenType::NUMBER, "100", "", 1},
        {TokenType::IDENTIFIER, "px", "", 1},
        {TokenType::QUESTION, "?", "", 1},
        {TokenType::IDENTIFIER, "red", "", 1},
        {TokenType::COLON, ":", "", 1},
        {TokenType::IDENTIFIER, "blue", "", 1}
    };

    // 2. First Pass: Evaluate simple properties and populate the context map.
    PropertyMap localProps;
    ExpressionEvaluator evaluator;

    ExpressionParser widthParser(widthTokens);
    auto widthExpr = widthParser.parse();
    // We call visit() to get the raw CssValue, not the final string.
    localProps["width"] = evaluator.visit(widthExpr.get(), {}); // Pass empty context for simple values

    // Assert that the context was populated correctly
    assert(localProps.count("width") == 1);
    assert(std::get<double>(localProps["width"].value) == 150.0);
    assert(localProps["width"].unit == "px");

    // 3. Second Pass: Evaluate the complex property using the populated context.
    ExpressionParser bgParser(bgTokens);
    auto bgExpr = bgParser.parse();
    std::string result = evaluator.evaluate(bgExpr.get(), localProps);

    // 4. Assert the final result is correct.
    std::cout << "  DEBUG: Final result is '" << result << "'" << std::endl;
    assert(result == "red");
}


int main() {
    runTest("Generator-like Conditional Evaluation", testConditionalWithContext);

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
