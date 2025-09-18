#include "test_main.h"
#include "../src/Scanner/UnifiedScanner.h"
#include <iostream>

TEST(ScannerBasic) {
    std::string source = R"(
div
{
    text
    {
        Hello World
    }
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerStyleBlock) {
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
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerScriptBlock) {
    std::string source = R"(
script
{
    console.log("Hello World");
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerTemplate) {
    std::string source = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerCustom) {
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
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerOrigin) {
    std::string source = R"(
[Origin] @Html
{
    <div>Hello World</div>
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerImport) {
    std::string source = R"(
[Import] @Chtl from "module.chtl"
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerConfiguration) {
    std::string source = R"(
[Configuration]
{
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerNamespace) {
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
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}

TEST(ScannerPlaceholder) {
    std::string source = R"(
div
{
    text
    {
        Hello World
    }
}
)";
    UnifiedScanner scanner;
    scanner.setEnablePlaceholderMechanism(true);
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
    ASSERT_TRUE(result.placeholders.size() > 0);
}

TEST(ScannerMixed) {
    std::string source = R"(
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }
    
    script
    {
        console.log("Hello World");
    }
}
)";
    UnifiedScanner scanner;
    ScanResult result = scanner.scan(source);
    
    ASSERT_TRUE(result.fragments.size() > 0);
    ASSERT_FALSE(result.processedCode.empty());
}