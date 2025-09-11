#include "CHTL/CHTLExpressionEvaluator/ExpressionEvaluator.h"
#include "CHTL/CHTLExpressionParser/ExpressionParser.h"
#include "CHTL/CHTLLexer/Token.h"
#include <iostream>
#include <cassert>
#include <string_view>

// Simple test runner
void runTest(const std::string& testName, void (*testFunc)()) {
    std::cout << "Running test: " << testName << "..." << std::endl;
    testFunc();
    std::cout << "  [PASS]" << std::endl;
}

// --- Test Cases ---

void testSimpleArithmetic() {
    // 10 + 20
    auto ten = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "10", 1});
    auto twenty = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "20", 1});
    auto expr = std::make_unique<BinaryOpExprNode>(std::move(ten), Token{TokenType::PLUS, "+", 1}, std::move(twenty));

    ExpressionEvaluator evaluator;
    PropertyMap context;
    std::string result = evaluator.evaluate(expr.get(), context);
    assert(result == "30");
}

void testComparison() {
    // 100 > 50
    auto हंड्रेड = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "100", 1});
    auto fifty = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "50", 1});
    auto expr = std::make_unique<BinaryOpExprNode>(std::move(हंड्रेड), Token{TokenType::GREATER, ">", 1}, std::move(fifty));

    ExpressionEvaluator evaluator;
    PropertyMap context;
    std::string result = evaluator.evaluate(expr.get(), context);
    assert(result == "true");
}

void testConditionalTrue() {
    // true ? "red" : "blue"
    auto cond = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "true", 1}); // Not a real CHTL literal, but evaluator should handle bool
    auto thenBranch = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "red", 1});
    auto elseBranch = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "blue", 1});

    // This test is flawed because we can't create a bool literal directly.
    // Let's test the full expression instead.
    // 10 > 5 ? "red" : "blue"
    auto ten = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "10", 1});
    auto five = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "5", 1});
    auto condition = std::make_unique<BinaryOpExprNode>(std::move(ten), Token{TokenType::GREATER, ">", 1}, std::move(five));

    auto expr = std::make_unique<ConditionalExprNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));

    ExpressionEvaluator evaluator;
    PropertyMap context;
    std::string result = evaluator.evaluate(expr.get(), context);
    assert(result == "red");
}

void testSelfReference() {
    // width > 100 ? "big" : "small"
    // context: width = 150px
    PropertyMap context;
    context["width"] = CssValue{ 150.0, "px" };

    auto width = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "width", 1});
    auto hundred = std::make_unique<LiteralExprNode>(Token{TokenType::NUMBER, "100", 1});
    auto condition = std::make_unique<BinaryOpExprNode>(std::move(width), Token{TokenType::GREATER, ">", 1}, std::move(hundred));

    auto thenBranch = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "big", 1});
    auto elseBranch = std::make_unique<LiteralExprNode>(Token{TokenType::IDENTIFIER, "small", 1});

    auto expr = std::make_unique<ConditionalExprNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));

    ExpressionEvaluator evaluator;
    std::string result = evaluator.evaluate(expr.get(), context);
    assert(result == "big");
}


void testParserPrecedence() {
    // 100 + 50 * 2
    std::vector<Token> tokens = {
        {TokenType::NUMBER, "100", 1},
        {TokenType::PLUS, "+", 1},
        {TokenType::NUMBER, "50", 1},
        {TokenType::STAR, "*", 1},
        {TokenType::NUMBER, "2", 1}
    };
    ExpressionParser parser(tokens);
    auto expr = parser.parse();

    // Expected tree: BinaryOp(+, Literal(100), BinaryOp(*, Literal(50), Literal(2)))
    auto* root = dynamic_cast<BinaryOpExprNode*>(expr.get());
    assert(root != nullptr);
    assert(root->op.type == TokenType::PLUS);

    auto* left = dynamic_cast<LiteralExprNode*>(root->left.get());
    assert(left != nullptr);
    assert(left->value.lexeme == "100");

    auto* right = dynamic_cast<BinaryOpExprNode*>(root->right.get());
    assert(right != nullptr);
    assert(right->op.type == TokenType::STAR);

    auto* right_left = dynamic_cast<LiteralExprNode*>(right->left.get());
    assert(right_left->value.lexeme == "50");
    auto* right_right = dynamic_cast<LiteralExprNode*>(right->right.get());
    assert(right_right->value.lexeme == "2");
}


int main() {
    try {
        runTest("Simple Arithmetic", testSimpleArithmetic);
        runTest("Comparison", testComparison);
        runTest("Conditional (True case)", testConditionalTrue);
        runTest("Self-Reference", testSelfReference);
        runTest("Parser Precedence", testParserPrecedence);
    } catch (const std::exception& e) {
        std::cerr << "  [FAIL] Exception thrown: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
