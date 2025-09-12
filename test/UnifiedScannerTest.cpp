#include <gtest/gtest.h>
#include "Scanner/UnifiedScanner.h"

class UnifiedScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        scanner = std::make_unique<CHTL::UnifiedScanner>();
    }

    void TearDown() override {
        scanner.reset();
    }

    std::unique_ptr<CHTL::UnifiedScanner> scanner;
};

TEST_F(UnifiedScannerTest, BasicCHTLCode) {
    std::string source_code = R"(
        div
        {
            text
            {
                Hello World
            }
        }
    )";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_EQ(fragments.size(), 1);
    EXPECT_EQ(fragments[0].type, CHTL::UnifiedScanner::CodeType::CHTL);
}

TEST_F(UnifiedScannerTest, CHTLWithTemplate) {
    std::string source_code = R"(
        [Template] @Style DefaultText
        {
            color: black;
            font-size: 16px;
        }
        
        div
        {
            @Style DefaultText;
        }
    )";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_EQ(fragments.size(), 1);
    EXPECT_EQ(fragments[0].type, CHTL::UnifiedScanner::CodeType::CHTL);
}

TEST_F(UnifiedScannerTest, MixedCodeTypes) {
    std::string source_code = R"(
        div
        {
            style
            {
                .box
                {
                    width: 100px;
                }
            }
            
            script
            {
                console.log("Hello");
            }
        }
    )";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_GT(fragments.size(), 0);
    
    bool hasCHTL = false;
    bool hasJS = false;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == CHTL::UnifiedScanner::CodeType::CHTL) {
            hasCHTL = true;
        } else if (fragment.type == CHTL::UnifiedScanner::CodeType::JS) {
            hasJS = true;
        }
    }
    
    EXPECT_TRUE(hasCHTL);
    EXPECT_TRUE(hasJS);
}

TEST_F(UnifiedScannerTest, PlaceholderGeneration) {
    std::string source_code = R"(
        function test() {
            return "Hello";
        }
        
        const a = 5;
    )";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_GT(fragments.size(), 0);
    
    // 检查占位符映射
    const auto& placeholder_map = scanner->getPlaceholderMap();
    EXPECT_GT(placeholder_map.size(), 0);
}

TEST_F(UnifiedScannerTest, EmptyInput) {
    std::string source_code = "";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_EQ(fragments.size(), 0);
}

TEST_F(UnifiedScannerTest, WhitespaceOnly) {
    std::string source_code = "   \n\t  \r\n  ";
    
    auto fragments = scanner->scan(source_code);
    
    EXPECT_EQ(fragments.size(), 0);
}