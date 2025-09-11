#include "gtest/gtest.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/ElementNode.h"

TEST(ParserTest, ParsesTopLevelTemplates) {
    std::string source = R"(
        [Template] @Style MyStyles {}
        [Template] @Element MyElement {}
        div {}
    )";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.Tokenize();
    CHTL::CHTLParser parser(tokens);
    CHTL::NodeList ast = parser.Parse();

    ASSERT_EQ(ast.size(), 3);

    // Check first template definition
    EXPECT_EQ(ast[0]->GetType(), CHTL::NodeType::TemplateDefinition);
    const auto* template1 = static_cast<const CHTL::TemplateDefinitionNode*>(ast[0].get());
    EXPECT_EQ(template1->GetName(), "MyStyles");
    EXPECT_EQ(template1->GetTemplateType(), CHTL::TemplateType::Style);

    // Check second template definition
    EXPECT_EQ(ast[1]->GetType(), CHTL::NodeType::TemplateDefinition);
    const auto* template2 = static_cast<const CHTL::TemplateDefinitionNode*>(ast[1].get());
    EXPECT_EQ(template2->GetName(), "MyElement");
    EXPECT_EQ(template2->GetTemplateType(), CHTL::TemplateType::Element);

    // Check the element node
    EXPECT_EQ(ast[2]->GetType(), CHTL::NodeType::Element);
    const auto* element = static_cast<const CHTL::ElementNode*>(ast[2].get());
    EXPECT_EQ(element->GetTagName(), "div");
}

TEST(ParserTest, DistinguishesCustomFromTemplate) {
    std::string source = R"(
        [Custom] @Style MyCustomStyle {}
        [Template] @Element MyTemplateElement {}
    )";
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.Tokenize();
    CHTL::CHTLParser parser(tokens);
    CHTL::NodeList ast = parser.Parse();

    ASSERT_EQ(ast.size(), 2);

    // Check the [Custom] definition
    ASSERT_EQ(ast[0]->GetType(), CHTL::NodeType::TemplateDefinition);
    const auto* customNode = static_cast<const CHTL::TemplateDefinitionNode*>(ast[0].get());
    EXPECT_EQ(customNode->GetName(), "MyCustomStyle");
    EXPECT_EQ(customNode->GetTemplateType(), CHTL::TemplateType::Style);
    EXPECT_TRUE(customNode->IsCustom());

    // Check the [Template] definition
    ASSERT_EQ(ast[1]->GetType(), CHTL::NodeType::TemplateDefinition);
    const auto* templateNode = static_cast<const CHTL::TemplateDefinitionNode*>(ast[1].get());
    EXPECT_EQ(templateNode->GetName(), "MyTemplateElement");
    EXPECT_EQ(templateNode->GetTemplateType(), CHTL::TemplateType::Element);
    EXPECT_FALSE(templateNode->IsCustom());
}
