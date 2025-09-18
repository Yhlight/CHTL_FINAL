#include "test_main.h"
#include "../src/CHTL/CHTLLexer.h"
#include "../src/CHTL/CHTLParser.h"
#include "../src/CHTL/CHTLGenerator.h"
#include <iostream>

TEST(GeneratorBasic) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_FALSE(result.html.empty());
}

TEST(GeneratorElement) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_TRUE(result.html.find("<div") != std::string::npos);
    ASSERT_TRUE(result.html.find("id=\"box\"") != std::string::npos);
    ASSERT_TRUE(result.html.find("class=\"welcome\"") != std::string::npos);
    ASSERT_TRUE(result.html.find("Hello World") != std::string::npos);
}

TEST(GeneratorStyle) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_TRUE(result.html.find("<style") != std::string::npos);
    ASSERT_TRUE(result.html.find("</style>") != std::string::npos);
}

TEST(GeneratorScript) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_TRUE(result.html.find("<script") != std::string::npos);
    ASSERT_TRUE(result.html.find("</script>") != std::string::npos);
    ASSERT_TRUE(result.html.find("console.log") != std::string::npos);
}

TEST(GeneratorTemplate) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    // 模板节点在HTML生成时通常被展开
}

TEST(GeneratorCustom) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    // 自定义节点在HTML生成时通常被展开
}

TEST(GeneratorOrigin) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_TRUE(result.html.find("<div>Hello World</div>") != std::string::npos);
}

TEST(GeneratorImport) {
    std::string source = R"(
[Import] @Chtl from "module.chtl"
)";
    CHTLLexer lexer(source);
    TokenList tokens = lexer.tokenize();
    
    CHTLParser parser(tokens);
    NodePtr root = parser.parse();
    
    ASSERT_TRUE(root != nullptr);
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    // 导入节点在HTML生成时通常被处理
}

TEST(GeneratorConfiguration) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    // 配置节点在HTML生成时通常被忽略
}

TEST(GeneratorNamespace) {
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
    
    CHTLGenerator generator;
    GenerationResult result = generator.generate(root);
    
    ASSERT_FALSE(result.hasErrors());
    // 命名空间节点在HTML生成时通常被展开
}

TEST(GeneratorOptions) {
    std::string source = R"(
div
{
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
    
    CHTLGenerator generator;
    GenerationOptions options;
    options.includeDefaultStructure = true;
    options.minify = true;
    
    GenerationResult result = generator.generate(root, options);
    
    ASSERT_FALSE(result.hasErrors());
    ASSERT_TRUE(result.html.find("<!DOCTYPE html>") != std::string::npos);
    ASSERT_TRUE(result.html.find("<html") != std::string::npos);
    ASSERT_TRUE(result.html.find("<head>") != std::string::npos);
    ASSERT_TRUE(result.html.find("<body>") != std::string::npos);
}