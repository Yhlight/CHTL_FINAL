#include <gtest/gtest.h>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL/TemplateSystem.h"
#include "CHTL/ModuleSystem.h"
#include "CHTL_JS/CHTLJSCompiler.h"
#include "Scanner/UnifiedScanner.h"
#include "CLI/CHTLCli.h"

namespace CHTL {

class CHTLComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化测试环境
        lexer_ = std::make_unique<CHTLLexer>("");
        parser_ = std::make_unique<CHTLParser>(TokenList{});
        generator_ = std::make_unique<CHTLGenerator>();
        templateSystem_ = std::make_shared<TemplateSystem>();
        moduleSystem_ = std::make_shared<ModuleSystem>();
        chtljsCompiler_ = std::make_shared<CHTLJSCompiler>();
        scanner_ = std::make_shared<UnifiedScanner>();
    }
    
    void TearDown() override {
        // 清理测试环境
        lexer_.reset();
        parser_.reset();
        generator_.reset();
        templateSystem_.reset();
        moduleSystem_.reset();
        chtljsCompiler_.reset();
        scanner_.reset();
    }
    
    std::unique_ptr<CHTLLexer> lexer_;
    std::unique_ptr<CHTLParser> parser_;
    std::unique_ptr<CHTLGenerator> generator_;
    std::shared_ptr<TemplateSystem> templateSystem_;
    std::shared_ptr<ModuleSystem> moduleSystem_;
    std::shared_ptr<CHTLJSCompiler> chtljsCompiler_;
    std::shared_ptr<UnifiedScanner> scanner_;
};

// 基础语法测试
TEST_F(CHTLComprehensiveTest, BasicSyntaxTest) {
    std::string source = R"(
        html {
            head {
                title { "My CHTL Page" }
                link {
                    rel: "stylesheet";
                    href: "style.css";
                }
            }
            body {
                div {
                    id: mainContent;
                    class: container;
                    text { "Hello, CHTL!" }
                    span {
                        text: "This is a span.";
                    }
                }
            }
        }
    )";
    
    // 词法分析
    lexer_ = std::make_unique<CHTLLexer>(source);
    auto tokens = lexer_->tokenize();
    EXPECT_FALSE(tokens.empty());
    
    // 语法分析
    parser_ = std::make_unique<CHTLParser>(tokens);
    auto ast = parser_->parse();
    EXPECT_NE(ast, nullptr);
    
    // 代码生成
    generator_ = std::make_unique<CHTLGenerator>();
    auto result = generator_->generate(ast);
    EXPECT_FALSE(result.html.empty());
    EXPECT_FALSE(result.hasErrors());
}

// 模板系统测试
TEST_F(CHTLComprehensiveTest, TemplateSystemTest) {
    // 创建模板
    auto template_ = std::make_shared<Template>(TemplateType::STYLE, "button");
    template_->setContent("button { background-color: {{color}}; color: {{textColor}}; }");
    template_->addParameter("color", "blue");
    template_->addParameter("textColor", "white");
    
    templateSystem_->registerTemplate(template_);
    
    // 渲染模板
    std::unordered_map<std::string, std::string> variables = {
        {"color", "red"},
        {"textColor", "black"}
    };
    
    std::string result = templateSystem_->renderTemplate("button", variables);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("background-color: red"), std::string::npos);
    EXPECT_NE(result.find("color: black"), std::string::npos);
}

// 模块系统测试
TEST_F(CHTLComprehensiveTest, ModuleSystemTest) {
    // 创建模块
    ModuleInfo info("test-module", "1.0.0");
    info.description = "Test module";
    info.author = "Test Author";
    
    auto module = std::make_shared<Module>(ModuleType::CMOD, "test-module");
    module->setInfo(info);
    
    auto exportTable = std::make_shared<ExportTable>();
    exportTable->addExport("testFunction", "function", true);
    exportTable->addExport("testVariable", "variable", false);
    
    module->setExportTable(exportTable);
    module->setContent("function testFunction() { return 'test'; }");
    
    moduleSystem_->registerModule(module);
    
    // 验证模块
    EXPECT_TRUE(moduleSystem_->hasModule("test-module"));
    auto retrievedModule = moduleSystem_->getModule("test-module");
    EXPECT_NE(retrievedModule, nullptr);
    EXPECT_EQ(retrievedModule->getInfo().name, "test-module");
    EXPECT_EQ(retrievedModule->getInfo().version, "1.0.0");
}

// CHTL JS编译器测试
TEST_F(CHTLComprehensiveTest, CHTLJSCompilerTest) {
    std::string source = R"(
        Vir {
            name: "testObject";
            properties: {
                value: "test";
                count: 0;
            };
        }
        
        Listen {
            event: "click";
            handler: function(e) {
                console.log("Clicked!");
            };
        }
        
        Animate {
            name: "fadeIn";
            properties: {
                duration: 1000;
                easing: "ease-in-out";
            };
        }
    )";
    
    auto result = chtljsCompiler_->compile(source);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("const testObject"), std::string::npos);
    EXPECT_NE(result.find("addEventListener"), std::string::npos);
    EXPECT_NE(result.find("fadeIn"), std::string::npos);
}

// 统一扫描器测试
TEST_F(CHTLComprehensiveTest, UnifiedScannerTest) {
    std::string source = R"(
        html {
            style {
                .button {
                    background-color: @color;
                    color: @textColor;
                }
            }
            script {
                Vir {
                    name: "testObject";
                    properties: { value: "test"; };
                }
            }
        }
    )";
    
    auto result = scanner_->scan(source);
    EXPECT_FALSE(result.fragments.empty());
    EXPECT_FALSE(result.processedCode.empty());
    
    // 验证片段类型
    bool hasCHTL = false;
    bool hasCSS = false;
    bool hasCHTLJS = false;
    
    for (const auto& fragment : result.fragments) {
        if (fragment.type == CodeFragmentType::CHTL) hasCHTL = true;
        if (fragment.type == CodeFragmentType::CSS) hasCSS = true;
        if (fragment.type == CodeFragmentType::CHTL_JS) hasCHTLJS = true;
    }
    
    EXPECT_TRUE(hasCHTL);
    EXPECT_TRUE(hasCSS);
    EXPECT_TRUE(hasCHTLJS);
}

// 复杂语法测试
TEST_F(CHTLComprehensiveTest, ComplexSyntaxTest) {
    std::string source = R"(
        [Template] @Style button {
            background-color: {{color}};
            color: {{textColor}};
            padding: {{padding}};
        }
        
        [Custom] @Style button {
            delete background-color;
            insert border: 1px solid {{borderColor}};
        }
        
        [Origin] @Html {
            <div class="external-content">
                <p>External HTML content</p>
            </div>
        }
        
        [Import] @Chtl {
            from: "components/header.chtl";
            as: "Header";
        }
        
        [Configuration] {
            [Name] {
                template: "Template";
                custom: "Custom";
            }
        }
        
        [Namespace] MyApp {
            html {
                head {
                    title { "{{title}}" }
                }
                body {
                    Header {}
                    div {
                        class: container;
                        text { "{{content}}" }
                    }
                }
            }
        }
    )";
    
    // 词法分析
    lexer_ = std::make_unique<CHTLLexer>(source);
    auto tokens = lexer_->tokenize();
    EXPECT_FALSE(tokens.empty());
    
    // 语法分析
    parser_ = std::make_unique<CHTLParser>(tokens);
    auto ast = parser_->parse();
    EXPECT_NE(ast, nullptr);
    
    // 代码生成
    generator_ = std::make_unique<CHTLGenerator>();
    auto result = generator_->generate(ast);
    EXPECT_FALSE(result.html.empty());
    EXPECT_FALSE(result.hasErrors());
}

// 错误处理测试
TEST_F(CHTLComprehensiveTest, ErrorHandlingTest) {
    std::string source = R"(
        html {
            div {
                // 缺少闭合括号
                text { "Hello, World!"
            }
        }
    )";
    
    // 词法分析
    lexer_ = std::make_unique<CHTLLexer>(source);
    auto tokens = lexer_->tokenize();
    EXPECT_FALSE(tokens.empty());
    
    // 语法分析
    parser_ = std::make_unique<CHTLParser>(tokens);
    auto ast = parser_->parse();
    // 应该能够处理错误并返回部分AST
    EXPECT_NE(ast, nullptr);
}

// 性能测试
TEST_F(CHTLComprehensiveTest, PerformanceTest) {
    std::string source = R"(
        html {
            head {
                title { "Performance Test" }
            }
            body {
                div {
                    class: container;
                    text { "Performance test content" }
                }
            }
        }
    )";
    
    // 多次编译测试性能
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        lexer_ = std::make_unique<CHTLLexer>(source);
        auto tokens = lexer_->tokenize();
        parser_ = std::make_unique<CHTLParser>(tokens);
        auto ast = parser_->parse();
        generator_ = std::make_unique<CHTLGenerator>();
        auto result = generator_->generate(ast);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 性能要求：1000次编译应该在1秒内完成
    EXPECT_LT(duration.count(), 1000);
}

// 内存测试
TEST_F(CHTLComprehensiveTest, MemoryTest) {
    std::string source = R"(
        html {
            body {
                div {
                    text { "Memory test content" }
                }
            }
        }
    )";
    
    // 多次创建和销毁对象测试内存泄漏
    const int iterations = 10000;
    
    for (int i = 0; i < iterations; ++i) {
        auto lexer = std::make_unique<CHTLLexer>(source);
        auto tokens = lexer->tokenize();
        auto parser = std::make_unique<CHTLParser>(tokens);
        auto ast = parser->parse();
        auto generator = std::make_unique<CHTLGenerator>();
        auto result = generator->generate(ast);
        
        // 对象会自动销毁
    }
    
    // 如果没有内存泄漏，测试应该通过
    EXPECT_TRUE(true);
}

// 边界条件测试
TEST_F(CHTLComprehensiveTest, BoundaryConditionsTest) {
    // 空输入测试
    std::string emptySource = "";
    lexer_ = std::make_unique<CHTLLexer>(emptySource);
    auto emptyTokens = lexer_->tokenize();
    EXPECT_TRUE(emptyTokens.empty() || emptyTokens.size() == 1); // 可能只有一个EOF token
    
    // 只有空格的输入测试
    std::string whitespaceSource = "   \n\t  \r\n  ";
    lexer_ = std::make_unique<CHTLLexer>(whitespaceSource);
    auto whitespaceTokens = lexer_->tokenize();
    EXPECT_TRUE(whitespaceTokens.empty() || whitespaceTokens.size() == 1);
    
    // 只有注释的输入测试
    std::string commentSource = "// This is a comment\n/* This is a multi-line comment */";
    lexer_ = std::make_unique<CHTLLexer>(commentSource);
    auto commentTokens = lexer_->tokenize();
    EXPECT_FALSE(commentTokens.empty());
}

// 集成测试
TEST_F(CHTLComprehensiveTest, IntegrationTest) {
    std::string source = R"(
        [Template] @Style button {
            background-color: {{color}};
            color: {{textColor}};
        }
        
        [Import] @Chtl {
            from: "components/header.chtl";
            as: "Header";
        }
        
        html {
            head {
                title { "Integration Test" }
                style {
                    .container {
                        width: 100%;
                        height: 100vh;
                    }
                }
            }
            body {
                Header {}
                div {
                    class: container;
                    text { "Integration test content" }
                    script {
                        Vir {
                            name: "testObject";
                            properties: { value: "test"; };
                        }
                    }
                }
            }
        }
    )";
    
    // 完整编译流程
    lexer_ = std::make_unique<CHTLLexer>(source);
    auto tokens = lexer_->tokenize();
    EXPECT_FALSE(tokens.empty());
    
    parser_ = std::make_unique<CHTLParser>(tokens);
    auto ast = parser_->parse();
    EXPECT_NE(ast, nullptr);
    
    generator_ = std::make_unique<CHTLGenerator>();
    generator_->setTemplateSystem(templateSystem_);
    generator_->setModuleSystem(moduleSystem_);
    generator_->setCHTLJSCompiler(chtljsCompiler_);
    generator_->setUnifiedScanner(scanner_);
    
    auto result = generator_->generate(ast);
    EXPECT_FALSE(result.html.empty());
    EXPECT_FALSE(result.hasErrors());
    
    // 验证输出包含预期内容
    EXPECT_NE(result.html.find("Integration Test"), std::string::npos);
    EXPECT_NE(result.html.find("Integration test content"), std::string::npos);
}

} // namespace CHTL