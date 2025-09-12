#include <gtest/gtest.h>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

class CHTLParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        lexer = std::make_unique<CHTL::CHTLLexer>();
        parser = std::make_unique<CHTL::CHTLParser>();
    }

    void TearDown() override {
        lexer.reset();
        parser.reset();
    }

    std::unique_ptr<CHTL::CHTLLexer> lexer;
    std::unique_ptr<CHTL::CHTLParser> parser;
};

TEST_F(CHTLParserTest, BasicElementParsing) {
    std::string source_code = R"(
        div
        {
            text
            {
                Hello World
            }
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    EXPECT_EQ(ast->getType(), CHTL::NodeType::ROOT);
    EXPECT_GT(ast->getChildren().size(), 0);
}

TEST_F(CHTLParserTest, ElementWithAttributes) {
    std::string source_code = R"(
        div
        {
            id: "box";
            class: "container";
            
            text
            {
                Content
            }
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有元素节点
    bool hasElement = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::ELEMENT) {
            hasElement = true;
            EXPECT_TRUE(child->hasAttribute("id"));
            EXPECT_TRUE(child->hasAttribute("class"));
            break;
        }
    }
    
    EXPECT_TRUE(hasElement);
}

TEST_F(CHTLParserTest, TemplateDeclaration) {
    std::string source_code = R"(
        [Template] @Style DefaultText
        {
            color: black;
            font-size: 16px;
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有模板节点
    bool hasTemplate = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::TEMPLATE) {
            hasTemplate = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasTemplate);
}

TEST_F(CHTLParserTest, CustomDeclaration) {
    std::string source_code = R"(
        [Custom] @Element Box
        {
            div
            {
                text
                {
                    Box Content
                }
            }
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有自定义节点
    bool hasCustom = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::CUSTOM) {
            hasCustom = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasCustom);
}

TEST_F(CHTLParserTest, StyleBlock) {
    std::string source_code = R"(
        div
        {
            style
            {
                .box
                {
                    width: 100px;
                    height: 100px;
                }
            }
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有样式节点
    bool hasStyle = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::ELEMENT) {
            for (const auto& grandchild : child->getChildren()) {
                if (grandchild->getType() == CHTL::NodeType::STYLE) {
                    hasStyle = true;
                    break;
                }
            }
        }
    }
    
    EXPECT_TRUE(hasStyle);
}

TEST_F(CHTLParserTest, ScriptBlock) {
    std::string source_code = R"(
        div
        {
            script
            {
                console.log("Hello World");
            }
        }
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有脚本节点
    bool hasScript = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::ELEMENT) {
            for (const auto& grandchild : child->getChildren()) {
                if (grandchild->getType() == CHTL::NodeType::SCRIPT) {
                    hasScript = true;
                    break;
                }
            }
        }
    }
    
    EXPECT_TRUE(hasScript);
}

TEST_F(CHTLParserTest, ImportStatement) {
    std::string source_code = R"(
        [Import] @Chtl from "module.chtl"
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    
    // 检查是否有导入节点
    bool hasImport = false;
    for (const auto& child : ast->getChildren()) {
        if (child->getType() == CHTL::NodeType::IMPORT) {
            hasImport = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasImport);
}

TEST_F(CHTLParserTest, EmptyInput) {
    std::string source_code = "";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    EXPECT_NE(ast, nullptr);
    EXPECT_EQ(ast->getType(), CHTL::NodeType::ROOT);
    EXPECT_EQ(ast->getChildren().size(), 0);
}

TEST_F(CHTLParserTest, ErrorHandling) {
    std::string source_code = R"(
        div
        {
            // 缺少闭合括号
    )";
    
    auto tokens = lexer->tokenize(source_code);
    auto ast = parser->parse(tokens);
    
    // 应该处理错误并返回部分解析结果或nullptr
    // 具体行为取决于错误处理策略
    const auto& errors = parser->getErrors();
    EXPECT_GT(errors.size(), 0);
}