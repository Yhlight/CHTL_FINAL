#include "test_main.h"
#include "../src/CHTL/CHTLLexer.h"
#include "../src/CHTL/CHTLParser.h"
#include <iostream>

TEST(ParserBasic) {
    std::string source = "div { text: \"Hello World\"; }";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType());
}

TEST(ParserElement) {
    std::string source = R"(
div
{
    id: box;
    class: welcome;
    
    text
    {
        Hello World
    }
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType());
}

TEST(ParserText) {
    std::string source = "\"Hello World\"";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserTemplate) {
    std::string source = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserCustom) {
    std::string source = R"(
[Custom] @Element Box
{
    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserStyle) {
    std::string source = R"(
style
{
    .box
    {
        width: 100px;
        height: 100px;
        background-color: red;
    }
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserScript) {
    std::string source = R"(
script
{
    console.log("Hello World");
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserOrigin) {
    std::string source = R"(
[Origin] @Html
{
    <div>Hello World</div>
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserImport) {
    std::string source = R"(
[Import] @Chtl from "module.chtl"
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserConfiguration) {
    std::string source = R"(
[Configuration]
{
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}

TEST(ParserNamespace) {
    std::string source = R"(
[Namespace] space
{
    [Custom] @Element Box
    {
        div
        {
            style
            {
                width: 200px;
                height: 200px;
            }
        }
    }
}
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(NodeType::ELEMENT, root->getType()); // 文档根节点
}