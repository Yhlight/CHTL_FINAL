#include <iostream>
#include <fstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/ConstraintNode.h"
#include "CHTL/CHTLConstraint/CHTLConstraintProcessor.h"
#include "CHTL/CHTLNode/ConfigurationNode.h"
#include "CHTL/CHTLConfiguration/CHTLConfigurationProcessor.h"
#include "CHTL/CHTLNode/UseNode.h"
#include "CHTL/CHTLUse/CHTLUseProcessor.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSToken.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSBaseNode.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

int main() {
    std::cout << "CHTL基础功能测试" << std::endl;
    std::cout << "=================" << std::endl;
    
    try {
        // 读取测试文件
        std::ifstream file("test.chtl");
        if (!file.is_open()) {
            std::cerr << "无法打开测试文件" << std::endl;
            return 1;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();
        
        std::cout << "源代码:" << std::endl;
        std::cout << source << std::endl;
        std::cout << std::endl;
        
        // 测试词法分析器
        std::cout << "词法分析结果:" << std::endl;
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        for (size_t i = 0; i < tokens.size() && i < 20; ++i) {
            auto token = tokens.peek(i);
            if (token.getTypeName() != "END_OF_FILE") {
                std::cout << token.toString() << std::endl;
            }
        }
        std::cout << std::endl;
        
        // 测试语法分析器
        std::cout << "语法分析结果:" << std::endl;
        CHTL::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "AST创建成功，节点数: " << ast->getChildCount() << std::endl;
            
            // 显示解析错误
            auto errors = parser.getErrors();
            if (!errors.empty()) {
                std::cout << "解析错误:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << error << std::endl;
                }
            }
        } else {
            std::cout << "AST创建失败" << std::endl;
        }
        
        // 测试代码生成器
        if (ast) {
            std::cout << "代码生成结果:" << std::endl;
            CHTL::CHTLGenerator generator;
            generator.setDebugMode(true);
            std::string html = generator.generate(ast);
            
            std::cout << "生成的HTML长度: " << html.length() << std::endl;
            std::cout << "HTML内容:" << std::endl;
            std::cout << html << std::endl;
            
            // 显示生成器错误
            auto errors = generator.getErrors();
            if (!errors.empty()) {
                std::cout << "生成错误:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  " << error << std::endl;
                }
            }
        }
        
        // 测试完整编译流程
        std::cout << "完整编译流程测试:" << std::endl;
        CHTL::CompilerDispatcher dispatcher;
        dispatcher.setDebugMode(true);
        auto result = dispatcher.compileFile("test.chtl");
        
        std::cout << "编译结果HTML长度: " << result.html.length() << std::endl;
        std::cout << "编译结果CSS长度: " << result.css.length() << std::endl;
        std::cout << "编译结果JavaScript长度: " << result.javascript.length() << std::endl;
        
        if (!result.errors.empty()) {
            std::cout << "编译错误:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  " << error << std::endl;
            }
        }
        
        if (!result.warnings.empty()) {
            std::cout << "编译警告:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "  " << warning << std::endl;
            }
        }
        
        // 测试约束系统
        std::cout << "\n约束系统测试:" << std::endl;
        CHTL::CHTLConstraintProcessor constraintProcessor;
        constraintProcessor.setDebugMode(true);
        
        // 创建约束节点
        auto constraint1 = std::make_shared<CHTL::ConstraintNode>("test_constraint", CHTL::ConstraintNode::ConstraintType::EXACT);
        constraint1->setConstraintExpression("test_value");
        constraint1->addTargetElement("div");
        constraint1->addTargetProperty("class");
        constraint1->setOperation(CHTL::ConstraintNode::ConstraintOperation::REQUIRE);
        constraint1->setOperationMessage("测试约束消息");
        
        // 添加约束到处理器
        constraintProcessor.addConstraint(constraint1);
        
        // 处理约束
        bool constraintResult = constraintProcessor.processConstraints();
        std::cout << "约束处理结果: " << (constraintResult ? "成功" : "失败") << std::endl;
        
        // 检查约束
        bool checkResult = constraintProcessor.checkConstraints("test_value");
        std::cout << "约束检查结果: " << (checkResult ? "通过" : "失败") << std::endl;
        
        // 输出约束调试信息
        std::cout << "约束处理器调试信息:" << std::endl;
        std::cout << constraintProcessor.getDebugInfo() << std::endl;
        
        // 测试配置系统
        std::cout << "\n配置系统测试:" << std::endl;
        CHTL::CHTLConfigurationProcessor configurationProcessor;
        configurationProcessor.setDebugMode(true);
        
        // 创建配置节点
        auto config1 = std::make_shared<CHTL::ConfigurationNode>("test_config", CHTL::ConfigurationNode::ConfigurationType::GLOBAL);
        config1->setConfigurationGroup("test_group");
        config1->setConfigurationParameter("debug", "true");
        config1->setConfigurationParameter("strict", "false");
        config1->addConfigurationRule("is_valid");
        config1->setConfigurationOption("auto_generate", true);
        config1->setConfigurationLimit("max_depth", 10);
        config1->setConfigurationDefault("output_format", "html5");
        
        // 添加配置到处理器
        configurationProcessor.addConfiguration(config1);
        
        // 处理配置
        bool configResult = configurationProcessor.processConfigurations();
        std::cout << "配置处理结果: " << (configResult ? "成功" : "失败") << std::endl;
        
        // 验证配置
        bool validateResult = configurationProcessor.validateConfigurations();
        std::cout << "配置验证结果: " << (validateResult ? "通过" : "失败") << std::endl;
        
        // 应用配置
        bool applyResult = configurationProcessor.applyConfigurations();
        std::cout << "配置应用结果: " << (applyResult ? "成功" : "失败") << std::endl;
        
        // 输出配置调试信息
        std::cout << "配置处理器调试信息:" << std::endl;
        std::cout << configurationProcessor.getDebugInfo() << std::endl;
        
        // 测试use语法系统
        std::cout << "\nuse语法系统测试:" << std::endl;
        CHTL::CHTLUseProcessor useProcessor;
        useProcessor.setDebugMode(true);
        
        // 创建use节点
        auto use1 = std::make_shared<CHTL::UseNode>("test_use", CHTL::UseNode::UseType::HTML5);
        use1->setUseGroup("test_group");
        use1->setUseVersion("1.0.0");
        use1->setUseParameter("doctype", "html5");
        use1->setUseParameter("charset", "utf-8");
        use1->addUseRule("is_valid");
        use1->setUseOption("auto_generate", true);
        use1->setUseLimit("max_depth", 10);
        use1->setUseDefault("output_format", "html5");
        
        // 添加use到处理器
        useProcessor.addUse(use1);
        
        // 处理use
        bool useResult = useProcessor.processUses();
        std::cout << "use处理结果: " << (useResult ? "成功" : "失败") << std::endl;
        
        // 验证use
        bool useValidateResult = useProcessor.validateUses();
        std::cout << "use验证结果: " << (useValidateResult ? "通过" : "失败") << std::endl;
        
        // 应用use
        bool useApplyResult = useProcessor.applyUses();
        std::cout << "use应用结果: " << (useApplyResult ? "成功" : "失败") << std::endl;
        
        // 输出use调试信息
        std::cout << "use处理器调试信息:" << std::endl;
        std::cout << useProcessor.getDebugInfo() << std::endl;
        
        // 测试CHTL JS词法分析器
        std::cout << "\nCHTL JS词法分析器测试:" << std::endl;
        CHTL_JS::CHTLJSLexer jsLexer;
        jsLexer.setDebugMode(true);
        jsLexer.setStrictMode(true);
        
        // 测试CHTL JS代码
        std::string chtljsCode = R"(
            vir test = listen {
                click: () => {
                    console.log('Button clicked!');
                },
                mouseenter: () => {
                    console.log('Mouse entered!');
                }
            };
            
            {{button}}->addEventListener('click', () => {
                console.log('Enhanced selector clicked!');
            });
            
            const anim = animate {
                target: {{box}},
                duration: 1000,
                easing: ease-in-out,
                begin: {
                    opacity: 0,
                    transform: 'scale(0.5)'
                },
                end: {
                    opacity: 1,
                    transform: 'scale(1.0)'
                }
            };
            
            router {
                url: "/home",
                page: {{homePage}}
            };
        )";
        
        jsLexer.setSource(chtljsCode);
        auto jsTokens = jsLexer.tokenize();
        
        std::cout << "CHTL JS词法分析结果:" << std::endl;
        for (size_t i = 0; i < jsTokens.size() && i < 30; ++i) {
            const auto& token = jsTokens[i];
            if (token.getType() != CHTL_JS::CHTLJSTokenType::END_OF_FILE) {
                std::cout << "[" << i << "] " << token.toDebugString() << std::endl;
            }
        }
        
        // 测试统一扫描器功能
        std::cout << "\n统一扫描器测试:" << std::endl;
        jsLexer.setUnifiedMode(true);
        auto fragments = jsLexer.separateCodeFragments(chtljsCode);
        std::cout << "分离的代码片段数量: " << fragments.size() << std::endl;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            std::cout << "片段 " << i << ": " << fragments[i].substr(0, 50) << "..." << std::endl;
        }
        
        // 测试语法边界识别
        std::cout << "\n语法边界识别测试:" << std::endl;
        auto boundaries = jsLexer.detectBoundaries(chtljsCode);
        std::cout << "检测到的语法边界数量: " << boundaries.size() << std::endl;
        
        // 测试占位符机制
        std::cout << "\n占位符机制测试:" << std::endl;
        std::string placeholder = jsLexer.createPlaceholder("function test() {}", "function");
        std::cout << "创建的占位符: " << placeholder << std::endl;
        
        std::string restored = jsLexer.restoreFromPlaceholder(placeholder);
        std::cout << "恢复的内容: " << restored << std::endl;
        
        // 输出CHTL JS词法分析器调试信息
        std::cout << "\nCHTL JS词法分析器调试信息:" << std::endl;
        std::cout << jsLexer.getDebugInfo() << std::endl;
        
        // 测试CHTL JS全局映射
        std::cout << "\nCHTL JS全局映射测试:" << std::endl;
        CHTL_JS::CHTLJSGlobalMap globalMap;
        globalMap.setDebugMode(true);
        
        // 测试关键字查找
        auto keywordType = globalMap.findTokenType("vir");
        std::cout << "关键字 'vir' 的类型: " << static_cast<int>(keywordType) << std::endl;
        
        // 测试运算符查找
        auto operatorType = globalMap.findTokenType("->");
        std::cout << "运算符 '->' 的类型: " << static_cast<int>(operatorType) << std::endl;
        
        // 测试CHTL JS函数
        bool hasFunction = globalMap.hasCHTLJSFunction("printMylove");
        std::cout << "是否有printMylove函数: " << (hasFunction ? "是" : "否") << std::endl;
        
        if (hasFunction) {
            std::string signature = globalMap.getCHTLJSFunctionSignature("printMylove");
            std::cout << "printMylove函数签名: " << signature << std::endl;
        }
        
        // 输出全局映射调试信息
        std::cout << "\nCHTL JS全局映射调试信息:" << std::endl;
        std::cout << globalMap.getDebugInfo() << std::endl;
        
        // 测试CHTL JS语法分析器
        std::cout << "\nCHTL JS语法分析器测试:" << std::endl;
        CHTL_JS::CHTLJSParser jsParser;
        jsParser.setDebugMode(true);
        jsParser.setStrictMode(true);
        jsParser.setAllowUnquotedLiterals(true);
        jsParser.setAllowUnorderedKeyValuePairs(true);
        jsParser.setAllowOptionalKeyValuePairs(true);
        jsParser.setAllowChainSyntax(true);
        jsParser.setAllowDeclarationSyntax(true);
        
        // 设置词法分析结果
        jsParser.setTokens(jsTokens);
        
        // 解析AST
        auto ast = jsParser.parse();
        if (ast) {
            std::cout << "AST解析成功!" << std::endl;
            std::cout << "AST类型: " << ast->getNodeTypeName() << std::endl;
            std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
            
            // 遍历AST
            std::cout << "\nAST遍历结果:" << std::endl;
            ast->traverse([](std::shared_ptr<CHTL_JS::CHTLJSBaseNode> node) {
                std::cout << "  " << node->toDebugString() << std::endl;
            });
            
            // 查找特定类型的节点
            auto enhancedSelectors = ast->findNodes(CHTL_JS::CHTLJSNodeType::ENHANCED_SELECTOR);
            std::cout << "\n找到的增强选择器数量: " << enhancedSelectors.size() << std::endl;
            
            auto virtualObjects = ast->findNodes(CHTL_JS::CHTLJSNodeType::VIRTUAL_OBJECT);
            std::cout << "找到的虚对象数量: " << virtualObjects.size() << std::endl;
            
            auto listenExpressions = ast->findNodes(CHTL_JS::CHTLJSNodeType::LISTEN_EXPRESSION);
            std::cout << "找到的监听表达式数量: " << listenExpressions.size() << std::endl;
            
            // 验证AST
            bool isValid = jsParser.validateAST(ast);
            std::cout << "AST验证结果: " << (isValid ? "通过" : "失败") << std::endl;
            
            if (!isValid) {
                auto validationErrors = jsParser.validateNode(ast);
                std::cout << "验证错误:" << std::endl;
                for (const auto& error : validationErrors) {
                    std::cout << "  " << error << std::endl;
                }
            }
            
            // 转换为JavaScript
            std::cout << "\n转换为JavaScript:" << std::endl;
            std::string javascript = ast->toJavaScript();
            std::cout << javascript << std::endl;
            
            // 转换为CHTL JS
            std::cout << "\n转换为CHTL JS:" << std::endl;
            std::string chtljs = ast->toCHTLJS();
            std::cout << chtljs << std::endl;
            
            // 转换为JSON
            std::cout << "\n转换为JSON:" << std::endl;
            std::string json = ast->toJSON();
            std::cout << json << std::endl;
            
        } else {
            std::cout << "AST解析失败!" << std::endl;
        }
        
        // 输出语法分析器调试信息
        std::cout << "\nCHTL JS语法分析器调试信息:" << std::endl;
        std::cout << jsParser.getDebugInfo() << std::endl;
        
        // 测试错误处理
        std::cout << "\n错误处理测试:" << std::endl;
        if (jsParser.hasErrors()) {
            std::cout << "错误数量: " << jsParser.getErrors().size() << std::endl;
            for (const auto& error : jsParser.getErrors()) {
                std::cout << "  错误: " << error << std::endl;
            }
        }
        
        if (jsParser.hasWarnings()) {
            std::cout << "警告数量: " << jsParser.getWarnings().size() << std::endl;
            for (const auto& warning : jsParser.getWarnings()) {
                std::cout << "  警告: " << warning << std::endl;
            }
        }
        
        // 测试CHTL JS代码生成器
        std::cout << "\nCHTL JS代码生成器测试:" << std::endl;
        CHTL_JS::CHTLJSGenerator jsGenerator;
        jsGenerator.setDebugMode(true);
        jsGenerator.setStrictMode(true);
        jsGenerator.setMinifyOutput(false);
        jsGenerator.setBeautifyOutput(true);
        jsGenerator.setIncludeComments(true);
        jsGenerator.setIncludeSourceMaps(false);
        jsGenerator.setOutputFormat("javascript");
        jsGenerator.setLanguageVersion("1.0.0");
        
        // 生成JavaScript代码
        if (ast) {
            std::string generatedCode = jsGenerator.generate(ast);
            std::cout << "生成的JavaScript代码:" << std::endl;
            std::cout << generatedCode << std::endl;
            
            // 验证生成的代码
            bool isValid = jsGenerator.validateOutput(generatedCode);
            std::cout << "\n代码验证结果: " << (isValid ? "通过" : "失败") << std::endl;
            
            if (!isValid) {
                auto validationErrors = jsGenerator.validateGeneratedCode(generatedCode);
                std::cout << "验证错误:" << std::endl;
                for (const auto& error : validationErrors) {
                    std::cout << "  " << error << std::endl;
                }
            }
            
            // 测试不同的输出格式
            std::cout << "\n测试不同输出格式:" << std::endl;
            
            // JavaScript格式
            jsGenerator.setOutputFormat("javascript");
            std::string jsCode = jsGenerator.generate(ast);
            std::cout << "JavaScript格式: " << jsCode.substr(0, 100) << "..." << std::endl;
            
            // CHTL JS格式
            jsGenerator.setOutputFormat("chtljs");
            std::string chtljsCode = jsGenerator.generate(ast);
            std::cout << "CHTL JS格式: " << chtljsCode.substr(0, 100) << "..." << std::endl;
            
            // HTML格式
            jsGenerator.setOutputFormat("html");
            std::string htmlCode = jsGenerator.generate(ast);
            std::cout << "HTML格式: " << htmlCode.substr(0, 100) << "..." << std::endl;
            
            // CSS格式
            jsGenerator.setOutputFormat("css");
            std::string cssCode = jsGenerator.generate(ast);
            std::cout << "CSS格式: " << cssCode.substr(0, 100) << "..." << std::endl;
            
            // 测试压缩和美化
            std::cout << "\n测试代码压缩和美化:" << std::endl;
            
            // 压缩模式
            jsGenerator.setMinifyOutput(true);
            jsGenerator.setBeautifyOutput(false);
            std::string minifiedCode = jsGenerator.generate(ast);
            std::cout << "压缩后代码长度: " << minifiedCode.length() << " 字符" << std::endl;
            
            // 美化模式
            jsGenerator.setMinifyOutput(false);
            jsGenerator.setBeautifyOutput(true);
            std::string beautifiedCode = jsGenerator.generate(ast);
            std::cout << "美化后代码长度: " << beautifiedCode.length() << " 字符" << std::endl;
            
            // 测试统计信息
            std::cout << "\n代码生成统计信息:" << std::endl;
            std::cout << "生成行数: " << jsGenerator.getGeneratedLines() << std::endl;
            std::cout << "生成字符数: " << jsGenerator.getGeneratedCharacters() << std::endl;
            
            auto featureUsage = jsGenerator.getFeatureUsage();
            if (!featureUsage.empty()) {
                std::cout << "功能使用统计:" << std::endl;
                for (const auto& pair : featureUsage) {
                    std::cout << "  " << pair.first << ": " << pair.second << " 次" << std::endl;
                }
            }
            
        } else {
            std::cout << "无法生成代码，AST为空!" << std::endl;
        }
        
        // 输出代码生成器调试信息
        std::cout << "\nCHTL JS代码生成器调试信息:" << std::endl;
        std::cout << jsGenerator.getDebugInfo() << std::endl;
        
        // 测试代码生成器错误处理
        std::cout << "\n代码生成器错误处理测试:" << std::endl;
        if (jsGenerator.hasErrors()) {
            std::cout << "错误数量: " << jsGenerator.getErrors().size() << std::endl;
            for (const auto& error : jsGenerator.getErrors()) {
                std::cout << "  错误: " << error << std::endl;
            }
        }
        
        if (jsGenerator.hasWarnings()) {
            std::cout << "警告数量: " << jsGenerator.getWarnings().size() << std::endl;
            for (const auto& warning : jsGenerator.getWarnings()) {
                std::cout << "  警告: " << warning << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "测试完成!" << std::endl;
    return 0;
}