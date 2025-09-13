#include <gtest/gtest.h>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLNode/RootNode.h"
#include "CHTL/CHTLNode/ElementNode.h"

// A helper function to parse a string of CHTL source
std::unique_ptr<CHTL::RootNode> ParseString(const std::string& source) {
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.scanTokens();
    CHTL::CHTLLoader loader;
    auto context = std::make_shared<CHTL::ParserContext>();
    CHTL::CHTLParser parser(source, tokens, loader, "./test.chtl", context);
    return parser.parse();
}

TEST(ParserTest, ParsesSimpleElement) {
    std::string source = "div {}";
    auto ast = ParseString(source);

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->children_.size(), 1);

    auto* element = dynamic_cast<CHTL::ElementNode*>(ast->children_[0].get());
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->tagName_, "div");
}

TEST(ParserTest, ParsesArithmeticExpressionInStyle) {
    std::string source = R"(
        div {
            style {
                width: 10px + 5px;
            }
        }
    )";
    auto ast = ParseString(source);

    ASSERT_NE(ast, nullptr);
    auto* div = dynamic_cast<CHTL::ElementNode*>(ast->children_[0].get());
    ASSERT_NE(div, nullptr);
    ASSERT_EQ(div->children_.size(), 1);

    auto* style = dynamic_cast<CHTL::StyleBlockNode*>(div->children_[0].get());
    ASSERT_NE(style, nullptr);
    ASSERT_EQ(style->inline_properties_.size(), 1);

    const auto& prop = style->inline_properties_[0];
    EXPECT_EQ(prop.first, "width");

    ASSERT_EQ(prop.second.size(), 1);
    const auto& prop_value = prop.second[0];
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<CHTL::ArithmeticNode>>(prop_value));

    const auto* arithmetic_node = std::get<std::unique_ptr<CHTL::ArithmeticNode>>(prop_value).get();
    ASSERT_NE(arithmetic_node, nullptr);
    EXPECT_EQ(arithmetic_node->op.type, CHTL::TokenType::Plus);
}

TEST(ParserTest, ParsesOperatorPrecedence) {
    std::string source = R"(
        div {
            style {
                width: 10px + 5px * 2;
            }
        }
    )";
    auto ast = ParseString(source);
    auto* div = dynamic_cast<CHTL::ElementNode*>(ast->children_[0].get());
    auto* style = dynamic_cast<CHTL::StyleBlockNode*>(div->children_[0].get());
    const auto& prop_value = style->inline_properties_[0].second[0];
    const auto* root_op = std::get<std::unique_ptr<CHTL::ArithmeticNode>>(prop_value).get();

    // Root operation should be PLUS because of precedence
    EXPECT_EQ(root_op->op.type, CHTL::TokenType::Plus);

    // The right-hand side of the PLUS should be another arithmetic node for TIMES
    const auto& right_hand_side = root_op->right[0];
    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<CHTL::ArithmeticNode>>(right_hand_side));
    const auto* nested_op = std::get<std::unique_ptr<CHTL::ArithmeticNode>>(right_hand_side).get();
    EXPECT_EQ(nested_op->op.type, CHTL::TokenType::Asterisk);
}

TEST(ParserTest, ParsesConditionalExpression) {
    std::string source = R"(
        div {
            style {
                color: 1 > 0 ? "red" : "blue";
            }
        }
    )";
    auto ast = ParseString(source);
    auto* div = dynamic_cast<CHTL::ElementNode*>(ast->children_[0].get());
    auto* style = dynamic_cast<CHTL::StyleBlockNode*>(div->children_[0].get());
    const auto& prop_value = style->inline_properties_[0].second[0];

    ASSERT_TRUE(std::holds_alternative<std::unique_ptr<CHTL::ConditionalNode>>(prop_value));
    const auto* cond_node = std::get<std::unique_ptr<CHTL::ConditionalNode>>(prop_value).get();
    ASSERT_NE(cond_node, nullptr);
    // A simple check to see if the node was created. More detailed checks could be added.
    ASSERT_EQ(cond_node->true_branch.size(), 1);
}

TEST(ParserTest, ParsesDeleteSpecialization) {
    std::string source = R"(
        [Custom] @Element Box {
            span {}
            p {}
        }

        body {
            @Element Box {
                delete span;
            }
        }
    )";
    auto ast = ParseString(source);
    ASSERT_NE(ast, nullptr);
    // Find the body element
    CHTL::ElementNode* body = nullptr;
    for(const auto& node : ast->children_) {
        if(auto* el = dynamic_cast<CHTL::ElementNode*>(node.get())) {
            if (el->tagName_ == "body") {
                body = el;
                break;
            }
        }
    }
    ASSERT_NE(body, nullptr);
    // After specialization, the body should only contain the 'p' element.
    ASSERT_EQ(body->children_.size(), 1);
    auto* child = dynamic_cast<CHTL::ElementNode*>(body->children_[0].get());
    ASSERT_NE(child, nullptr);
    EXPECT_EQ(child->tagName_, "p");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
