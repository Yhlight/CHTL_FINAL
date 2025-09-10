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
#include "CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedSelector.h"
#include "CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedListener.h"
#include "CHTL_JS/CHTLJSEnhanced/CHTLJSEventDelegation.h"
#include "CHTL/CMODSystem/CMODModule.h"
#include "CHTL/CMODSystem/CMODLoader.h"
#include "CHTL_JS/CJMODSystem/CJMODModule.h"
#include "CHTL_JS/CJMODSystem/CJMODSyntax.h"
#include "CHTL_JS/CJMODSystem/CJMODArg.h"
#include "CHTL_JS/CJMODSystem/CJMODScanner.h"
#include "CHTL_JS/CJMODSystem/CJMODGenerator.h"
#include "Scanner/UnifiedScanner.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CompilerDispatcher/CSSCompiler.h"
#include "CompilerDispatcher/JSCompiler.h"
#include "CLI/CLITool.h"
#include <thread>
#include <chrono>

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
    
    // CHTL JS增强功能测试
    std::cout << "\n\nCHTL JS增强功能测试" << std::endl;
    std::cout << "===================" << std::endl;
    
    try {
        // 测试增强选择器
        std::cout << "\n1. 增强选择器测试:" << std::endl;
        CHTL_JS::CHTLJSEnhancedSelector selector(".test-class");
        std::cout << "选择器: " << selector.toString() << std::endl;
        std::cout << "JavaScript代码: " << selector.toJavaScript() << std::endl;
        std::cout << "CHTL JS代码: " << selector.toCHTLJS() << std::endl;
        std::cout << "是否有效: " << (selector.isValid() ? "是" : "否") << std::endl;
        std::cout << "是否完整: " << (selector.isComplete() ? "是" : "否") << std::endl;
        
        // 测试增强监听器
        std::cout << "\n2. 增强监听器测试:" << std::endl;
        CHTL_JS::EventHandlerWrapper handler([](const std::string& eventType, const std::map<std::string, std::any>& data) {
            std::cout << "事件触发: " << eventType << std::endl;
        });
        
        CHTL_JS::EventOptions options;
        options.once = true;
        options.preventDefault = true;
        options.throttle = 100;
        
        CHTL_JS::CHTLJSEnhancedListener listener("click", handler, options);
        std::cout << "监听器类型: " << listener.getEventType() << std::endl;
        std::cout << "JavaScript代码: " << listener.toJavaScript() << std::endl;
        std::cout << "CHTL JS代码: " << listener.toCHTLJS() << std::endl;
        std::cout << "是否有效: " << (listener.isValid() ? "是" : "否") << std::endl;
        std::cout << "是否完整: " << (listener.isComplete() ? "是" : "否") << std::endl;
        
        // 测试事件委托
        std::cout << "\n3. 事件委托测试:" << std::endl;
        CHTL_JS::DelegationHandler delegationHandler([](const CHTL_JS::DelegationEventInfo& eventInfo) {
            std::cout << "委托事件处理: " << eventInfo.eventType << std::endl;
        });
        
        CHTL_JS::DelegationOptions delegationOptions;
        delegationOptions.strategy = CHTL_JS::DelegationStrategy::BUBBLE;
        delegationOptions.stopPropagation = true;
        
        CHTL_JS::CHTLJSEventDelegation delegation("#container", ".item", delegationHandler, delegationOptions);
        std::cout << "委托选择器: " << delegation.toString() << std::endl;
        std::cout << "JavaScript代码: " << delegation.toJavaScript() << std::endl;
        std::cout << "CHTL JS代码: " << delegation.toCHTLJS() << std::endl;
        std::cout << "是否有效: " << (delegation.isValid() ? "是" : "否") << std::endl;
        std::cout << "是否完整: " << (delegation.isComplete() ? "是" : "否") << std::endl;
        
        // 测试监听器管理器
        std::cout << "\n4. 监听器管理器测试:" << std::endl;
        CHTL_JS::CHTLJSEnhancedListenerManager listenerManager;
        listenerManager.addListener(std::make_shared<CHTL_JS::CHTLJSEnhancedListener>(listener));
        std::cout << "监听器数量: " << listenerManager.getListenerCount() << std::endl;
        std::cout << "JavaScript代码: " << listenerManager.toJavaScript() << std::endl;
        std::cout << "CHTL JS代码: " << listenerManager.toCHTLJS() << std::endl;
        
        // 测试事件委托管理器
        std::cout << "\n5. 事件委托管理器测试:" << std::endl;
        CHTL_JS::CHTLJSEventDelegationManager delegationManager;
        delegationManager.addDelegation(std::make_shared<CHTL_JS::CHTLJSEventDelegation>(delegation));
        std::cout << "委托数量: " << delegationManager.getDelegationCount() << std::endl;
        std::cout << "JavaScript代码: " << delegationManager.toJavaScript() << std::endl;
        std::cout << "CHTL JS代码: " << delegationManager.toCHTLJS() << std::endl;
        
        // 测试增强功能集成
        std::cout << "\n6. 增强功能集成测试:" << std::endl;
        
        // 创建复杂的选择器
        auto complexSelector = std::make_shared<CHTL_JS::CHTLJSEnhancedSelector>("#main .item.active");
        complexSelector->addChild(std::make_shared<CHTL_JS::CHTLJSEnhancedSelector>(".sub-item"));
        std::cout << "复杂选择器: " << complexSelector->toString() << std::endl;
        std::cout << "子选择器数量: " << complexSelector->getChildren().size() << std::endl;
        
        // 创建带修饰符的监听器
        auto enhancedListener = std::make_shared<CHTL_JS::CHTLJSEnhancedListener>("mouseover", handler);
        enhancedListener->addModifier(CHTL_JS::EventModifier::THROTTLE);
        enhancedListener->addModifier(CHTL_JS::EventModifier::PREVENT);
        enhancedListener->setThrottle(200);
        std::cout << "增强监听器修饰符数量: " << enhancedListener->getModifiers().size() << std::endl;
        std::cout << "节流间隔: " << enhancedListener->getThrottle() << "ms" << std::endl;
        
        // 创建带过滤器的委托
        auto enhancedDelegation = std::make_shared<CHTL_JS::CHTLJSEventDelegation>("#list", ".item", delegationHandler);
        enhancedDelegation->addFilter([](const CHTL_JS::DelegationEventInfo& eventInfo) {
            return eventInfo.eventType == "click";
        });
        enhancedDelegation->addTransformer([](const CHTL_JS::DelegationEventInfo& eventInfo) {
            CHTL_JS::DelegationEventInfo transformed = eventInfo;
            transformed.data["transformed"] = true;
            return transformed;
        });
        std::cout << "增强委托过滤器数量: " << enhancedDelegation->getFilters().size() << std::endl;
        std::cout << "增强委托转换器数量: " << enhancedDelegation->getTransformers().size() << std::endl;
        
        // 测试性能统计
        std::cout << "\n7. 性能统计测试:" << std::endl;
        std::cout << "监听器执行次数: " << listener.getExecutionCount() << std::endl;
        std::cout << "委托执行次数: " << delegation.getExecutionCount() << std::endl;
        std::cout << "监听器总执行时间: " << listener.getTotalExecutionTime().count() << "ms" << std::endl;
        std::cout << "委托总执行时间: " << delegation.getTotalExecutionTime().count() << "ms" << std::endl;
        
        // 测试验证功能
        std::cout << "\n8. 验证功能测试:" << std::endl;
        auto selectorErrors = selector.validate();
        auto listenerErrors = listener.validate();
        auto delegationErrors = delegation.validate();
        
        std::cout << "选择器验证错误数: " << selectorErrors.size() << std::endl;
        std::cout << "监听器验证错误数: " << listenerErrors.size() << std::endl;
        std::cout << "委托验证错误数: " << delegationErrors.size() << std::endl;
        
        // 测试克隆功能
        std::cout << "\n9. 克隆功能测试:" << std::endl;
        auto clonedSelector = selector.clone();
        auto clonedListener = listener.clone();
        auto clonedDelegation = delegation.clone();
        
        std::cout << "选择器克隆成功: " << (clonedSelector ? "是" : "否") << std::endl;
        std::cout << "监听器克隆成功: " << (clonedListener ? "是" : "否") << std::endl;
        std::cout << "委托克隆成功: " << (clonedDelegation ? "是" : "否") << std::endl;
        
        // 测试格式化功能
        std::cout << "\n10. 格式化功能测试:" << std::endl;
        std::cout << "选择器格式化: " << selector.format() << std::endl;
        std::cout << "监听器格式化: " << listener.format() << std::endl;
        std::cout << "委托格式化: " << delegation.format() << std::endl;
        
        std::cout << "选择器压缩: " << selector.minify() << std::endl;
        std::cout << "监听器压缩: " << listener.minify() << std::endl;
        std::cout << "委托压缩: " << delegation.minify() << std::endl;
        
        std::cout << "选择器美化: " << selector.beautify() << std::endl;
        std::cout << "监听器美化: " << listener.beautify() << std::endl;
        std::cout << "委托美化: " << delegation.beautify() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS增强功能测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    // 模块系统测试
    std::cout << "\n\n模块系统测试" << std::endl;
    std::cout << "=============" << std::endl;
    
    try {
        // 测试CMOD模块
        std::cout << "\n1. CMOD模块测试:" << std::endl;
        CHTL::CMODModule cmodModule("test-module", "1.0.0");
        cmodModule.setDescription("测试模块");
        cmodModule.setAuthor("测试作者");
        cmodModule.setLicense("MIT");
        cmodModule.setType(CHTL::ModuleType::CHTL);
        
        std::cout << "模块名称: " << cmodModule.getName() << std::endl;
        std::cout << "模块版本: " << cmodModule.getVersion() << std::endl;
        std::cout << "模块描述: " << cmodModule.getDescription() << std::endl;
        std::cout << "模块作者: " << cmodModule.getAuthor() << std::endl;
        std::cout << "模块许可证: " << cmodModule.getLicense() << std::endl;
        std::cout << "模块类型: " << static_cast<int>(cmodModule.getType()) << std::endl;
        std::cout << "是否有效: " << (cmodModule.isValid() ? "是" : "否") << std::endl;
        std::cout << "是否完整: " << (cmodModule.isComplete() ? "是" : "否") << std::endl;
        
        // 测试CMOD模块依赖
        std::cout << "\n2. CMOD模块依赖测试:" << std::endl;
        cmodModule.addDependency("dependency1", "1.0.0", true);
        cmodModule.addDependency("dependency2", "2.0.0", false);
        std::cout << "依赖数量: " << cmodModule.getDependencyCount() << std::endl;
        std::cout << "必需依赖数量: " << cmodModule.getRequiredDependencies().size() << std::endl;
        std::cout << "可选依赖数量: " << cmodModule.getOptionalDependencies().size() << std::endl;
        
        // 测试CMOD模块导出
        std::cout << "\n3. CMOD模块导出测试:" << std::endl;
        cmodModule.addExport("function1", std::string("function"), "function");
        cmodModule.addExport("class1", std::string("class"), "class");
        cmodModule.addDefaultExport(std::string("default"), "any");
        std::cout << "导出数量: " << cmodModule.getExportCount() << std::endl;
        std::cout << "命名导出数量: " << cmodModule.getNamedExports().size() << std::endl;
        std::cout << "默认导出: " << (cmodModule.getDefaultExport().name.empty() ? "无" : "有") << std::endl;
        
        // 测试CMOD模块元数据
        std::cout << "\n4. CMOD模块元数据测试:" << std::endl;
        cmodModule.addMetadata("key1", std::string("value1"));
        cmodModule.addMetadata("key2", 42);
        cmodModule.addMetadata("key3", true);
        std::cout << "元数据数量: " << cmodModule.getMetadataCount() << std::endl;
        std::cout << "元数据key1: " << std::any_cast<std::string>(cmodModule.getMetadata("key1")) << std::endl;
        std::cout << "元数据key2: " << std::any_cast<int>(cmodModule.getMetadata("key2")) << std::endl;
        std::cout << "元数据key3: " << (std::any_cast<bool>(cmodModule.getMetadata("key3")) ? "true" : "false") << std::endl;
        
        // 测试CMOD模块转换
        std::cout << "\n5. CMOD模块转换测试:" << std::endl;
        std::cout << "JSON格式: " << cmodModule.toJSON() << std::endl;
        std::cout << "XML格式: " << cmodModule.toXML() << std::endl;
        std::cout << "YAML格式: " << cmodModule.toYAML() << std::endl;
        std::cout << "字符串格式: " << cmodModule.toString() << std::endl;
        std::cout << "调试格式: " << cmodModule.toDebugString() << std::endl;
        
        // 测试CJMOD模块
        std::cout << "\n6. CJMOD模块测试:" << std::endl;
        CHTL_JS::CJMODModule cjmodModule("test-js-module", "1.0.0");
        cjmodModule.setDescription("测试JavaScript模块");
        cjmodModule.setAuthor("测试作者");
        cjmodModule.setLicense("MIT");
        cjmodModule.setType(CHTL_JS::CJMODModuleType::CHTL_JS);
        
        std::cout << "模块名称: " << cjmodModule.getName() << std::endl;
        std::cout << "模块版本: " << cjmodModule.getVersion() << std::endl;
        std::cout << "模块描述: " << cjmodModule.getDescription() << std::endl;
        std::cout << "模块作者: " << cjmodModule.getAuthor() << std::endl;
        std::cout << "模块许可证: " << cjmodModule.getLicense() << std::endl;
        std::cout << "模块类型: " << static_cast<int>(cjmodModule.getType()) << std::endl;
        std::cout << "是否有效: " << (cjmodModule.isValid() ? "是" : "否") << std::endl;
        std::cout << "是否完整: " << (cjmodModule.isComplete() ? "是" : "否") << std::endl;
        
        // 测试CJMOD模块导出
        std::cout << "\n7. CJMOD模块导出测试:" << std::endl;
        cjmodModule.addFunctionExport("testFunction", std::string("function"), "function");
        cjmodModule.addClassExport("TestClass", std::string("class"), "class");
        cjmodModule.addVariableExport("testVariable", std::string("variable"), "variable");
        cjmodModule.addDefaultExport(std::string("default"), "any");
        
        std::cout << "导出数量: " << cjmodModule.getExportCount() << std::endl;
        std::cout << "函数导出数量: " << cjmodModule.getFunctionExportCount() << std::endl;
        std::cout << "类导出数量: " << cjmodModule.getClassExportCount() << std::endl;
        std::cout << "变量导出数量: " << cjmodModule.getVariableExportCount() << std::endl;
        std::cout << "命名导出数量: " << cjmodModule.getNamedExports().size() << std::endl;
        std::cout << "默认导出: " << (cjmodModule.getDefaultExport().name.empty() ? "无" : "有") << std::endl;
        
        // 测试CJMOD模块转换
        std::cout << "\n8. CJMOD模块转换测试:" << std::endl;
        std::cout << "JSON格式: " << cjmodModule.toJSON() << std::endl;
        std::cout << "XML格式: " << cjmodModule.toXML() << std::endl;
        std::cout << "YAML格式: " << cjmodModule.toYAML() << std::endl;
        std::cout << "字符串格式: " << cjmodModule.toString() << std::endl;
        std::cout << "调试格式: " << cjmodModule.toDebugString() << std::endl;
        
        // 测试CMOD加载器
        std::cout << "\n9. CMOD加载器测试:" << std::endl;
        CHTL::CMODLoader cmodLoader;
        cmodLoader.addSearchPath("/tmp");
        cmodLoader.setEnabled(true);
        cmodLoader.setDebug(true);
        
        std::cout << "加载器是否启用: " << (cmodLoader.isEnabled() ? "是" : "否") << std::endl;
        std::cout << "加载器是否调试: " << (cmodLoader.isDebug() ? "是" : "否") << std::endl;
        std::cout << "搜索路径数量: " << cmodLoader.getSearchPaths().size() << std::endl;
        std::cout << "已加载模块数量: " << cmodLoader.getLoadedModuleCount() << std::endl;
        std::cout << "缓存大小: " << cmodLoader.getCacheSize() << std::endl;
        
        // 测试模块验证
        std::cout << "\n10. 模块验证测试:" << std::endl;
        auto cmodErrors = cmodModule.validate();
        auto cjmodErrors = cjmodModule.validate();
        auto loaderErrors = cmodLoader.validate();
        
        std::cout << "CMOD模块验证错误数: " << cmodErrors.size() << std::endl;
        std::cout << "CJMOD模块验证错误数: " << cjmodErrors.size() << std::endl;
        std::cout << "CMOD加载器验证错误数: " << loaderErrors.size() << std::endl;
        
        // 测试模块克隆
        std::cout << "\n11. 模块克隆测试:" << std::endl;
        auto clonedCmod = cmodModule.clone();
        auto clonedCjmod = cjmodModule.clone();
        
        std::cout << "CMOD模块克隆成功: " << (clonedCmod ? "是" : "否") << std::endl;
        std::cout << "CJMOD模块克隆成功: " << (clonedCjmod ? "是" : "否") << std::endl;
        
        if (clonedCmod) {
            std::cout << "克隆的CMOD模块名称: " << clonedCmod->getName() << std::endl;
            std::cout << "克隆的CMOD模块版本: " << clonedCmod->getVersion() << std::endl;
        }
        
        if (clonedCjmod) {
            std::cout << "克隆的CJMOD模块名称: " << clonedCjmod->getName() << std::endl;
            std::cout << "克隆的CJMOD模块版本: " << clonedCjmod->getVersion() << std::endl;
        }
        
        // 测试模块比较
        std::cout << "\n12. 模块比较测试:" << std::endl;
        std::cout << "CMOD模块相等: " << (cmodModule.equals(clonedCmod) ? "是" : "否") << std::endl;
        std::cout << "CJMOD模块相等: " << (cjmodModule.equals(clonedCjmod) ? "是" : "否") << std::endl;
        
        // 测试模块格式化
        std::cout << "\n13. 模块格式化测试:" << std::endl;
        std::cout << "CMOD模块格式化: " << cmodModule.format() << std::endl;
        std::cout << "CJMOD模块格式化: " << cjmodModule.format() << std::endl;
        std::cout << "CMOD模块压缩: " << cmodModule.minify() << std::endl;
        std::cout << "CJMOD模块压缩: " << cjmodModule.minify() << std::endl;
        std::cout << "CMOD模块美化: " << cmodModule.beautify() << std::endl;
        std::cout << "CJMOD模块美化: " << cjmodModule.beautify() << std::endl;
        
        // 测试模块统计
        std::cout << "\n14. 模块统计测试:" << std::endl;
        std::cout << "CMOD模块依赖数量: " << cmodModule.getDependencyCount() << std::endl;
        std::cout << "CMOD模块导出数量: " << cmodModule.getExportCount() << std::endl;
        std::cout << "CMOD模块子模块数量: " << cmodModule.getSubmoduleCount() << std::endl;
        std::cout << "CMOD模块元数据数量: " << cmodModule.getMetadataCount() << std::endl;
        
        std::cout << "CJMOD模块依赖数量: " << cjmodModule.getDependencyCount() << std::endl;
        std::cout << "CJMOD模块导出数量: " << cjmodModule.getExportCount() << std::endl;
        std::cout << "CJMOD模块函数导出数量: " << cjmodModule.getFunctionExportCount() << std::endl;
        std::cout << "CJMOD模块类导出数量: " << cjmodModule.getClassExportCount() << std::endl;
        std::cout << "CJMOD模块变量导出数量: " << cjmodModule.getVariableExportCount() << std::endl;
        std::cout << "CJMOD模块子模块数量: " << cjmodModule.getSubmoduleCount() << std::endl;
        std::cout << "CJMOD模块元数据数量: " << cjmodModule.getMetadataCount() << std::endl;
        
        // 测试模块重置
        std::cout << "\n15. 模块重置测试:" << std::endl;
        cmodModule.reset();
        cjmodModule.reset();
        cmodLoader.reset();
        
        std::cout << "CMOD模块重置后名称: " << cmodModule.getName() << std::endl;
        std::cout << "CJMOD模块重置后名称: " << cjmodModule.getName() << std::endl;
        std::cout << "CMOD加载器重置后已加载模块数量: " << cmodLoader.getLoadedModuleCount() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "模块系统测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    // CJMOD API测试
    std::cout << "\n\nCJMOD API测试" << std::endl;
    std::cout << "=============" << std::endl;
    
    try {
        // 测试CJMOD语法分析器
        std::cout << "\n1. CJMOD语法分析器测试:" << std::endl;
        CHTL_JS::CJMODSyntax syntax;
        
        // 测试基本类型分析
        std::cout << "分析字符串: " << syntax.analyze(std::string("hello")).description << std::endl;
        std::cout << "分析数字: " << syntax.analyze(42).description << std::endl;
        std::cout << "分析布尔值: " << syntax.analyze(true).description << std::endl;
        std::cout << "分析数组: " << syntax.analyze(std::vector<std::any>{1, 2, 3}).description << std::endl;
        std::cout << "分析对象: " << syntax.analyze(std::map<std::string, std::any>{{"key", "value"}}).description << std::endl;
        
        // 测试类型检查
        std::cout << "\n2. 类型检查测试:" << std::endl;
        std::cout << "是字符串: " << (syntax.isString(std::string("test")) ? "是" : "否") << std::endl;
        std::cout << "是数字: " << (syntax.isNumber(42) ? "是" : "否") << std::endl;
        std::cout << "是布尔值: " << (syntax.isBoolean(true) ? "是" : "否") << std::endl;
        std::cout << "是数组: " << (syntax.isArray(std::vector<std::any>{1, 2, 3}) ? "是" : "否") << std::endl;
        std::cout << "是对象: " << (syntax.isObject(std::map<std::string, std::any>{{"key", "value"}}) ? "是" : "否") << std::endl;
        
        // 测试类型推断
        std::cout << "\n3. 类型推断测试:" << std::endl;
        std::cout << "代码类型推断: " << static_cast<int>(syntax.inferType("function test() {}")) << std::endl;
        std::cout << "代码类型推断: " << static_cast<int>(syntax.inferType("class Test {}")) << std::endl;
        std::cout << "代码类型推断: " << static_cast<int>(syntax.inferType("async function test() {}")) << std::endl;
        std::cout << "代码类型推断: " << static_cast<int>(syntax.inferType("CHTLJSFunction test() {}")) << std::endl;
        
        // 测试验证
        std::cout << "\n4. 验证测试:" << std::endl;
        auto validationResult = syntax.validate(std::string("test"));
        std::cout << "字符串验证: " << (validationResult.valid ? "有效" : "无效") << std::endl;
        
        auto arrayValidationResult = syntax.validate(std::vector<std::any>{1, 2, 3});
        std::cout << "数组验证: " << (arrayValidationResult.valid ? "有效" : "无效") << std::endl;
        
        auto objectValidationResult = syntax.validate(std::map<std::string, std::any>{{"key", "value"}});
        std::cout << "对象验证: " << (objectValidationResult.valid ? "有效" : "无效") << std::endl;
        
        // 测试转换
        std::cout << "\n5. 转换测试:" << std::endl;
        std::cout << "转换为字符串: " << syntax.transformToString(std::string("test")) << std::endl;
        std::cout << "转换为字符串: " << syntax.transformToString(42) << std::endl;
        std::cout << "转换为字符串: " << syntax.transformToString(true) << std::endl;
        
        // 测试序列化
        std::cout << "\n6. 序列化测试:" << std::endl;
        std::cout << "序列化字符串: " << syntax.serialize(std::string("test")) << std::endl;
        std::cout << "序列化数字: " << syntax.serialize(42) << std::endl;
        std::cout << "序列化布尔值: " << syntax.serialize(true) << std::endl;
        std::cout << "序列化数组: " << syntax.serialize(std::vector<std::any>{1, 2, 3}) << std::endl;
        std::cout << "序列化对象: " << syntax.serialize(std::map<std::string, std::any>{{"key", "value"}}) << std::endl;
        
        // 测试比较
        std::cout << "\n7. 比较测试:" << std::endl;
        std::cout << "字符串相等: " << (syntax.equals(std::string("test"), std::string("test")) ? "是" : "否") << std::endl;
        std::cout << "数字相等: " << (syntax.equals(42, 42) ? "是" : "否") << std::endl;
        std::cout << "布尔值相等: " << (syntax.equals(true, true) ? "是" : "否") << std::endl;
        
        // 测试克隆
        std::cout << "\n8. 克隆测试:" << std::endl;
        auto clonedString = syntax.clone(std::string("test"));
        std::cout << "克隆字符串: " << (clonedString.has_value() ? "成功" : "失败") << std::endl;
        
        auto clonedArray = syntax.cloneArray(std::vector<std::any>{1, 2, 3});
        std::cout << "克隆数组: " << (clonedArray.size() == 3 ? "成功" : "失败") << std::endl;
        
        auto clonedObject = syntax.cloneObject(std::map<std::string, std::any>{{"key", "value"}});
        std::cout << "克隆对象: " << (clonedObject.size() == 1 ? "成功" : "失败") << std::endl;
        
        // 测试合并
        std::cout << "\n9. 合并测试:" << std::endl;
        auto mergedArray = syntax.mergeArrays(std::vector<std::any>{1, 2}, std::vector<std::any>{3, 4});
        std::cout << "合并数组: " << (mergedArray.size() == 4 ? "成功" : "失败") << std::endl;
        
        auto mergedObject = syntax.mergeObjects(
            std::map<std::string, std::any>{{"key1", "value1"}},
            std::map<std::string, std::any>{{"key2", "value2"}}
        );
        std::cout << "合并对象: " << (mergedObject.size() == 2 ? "成功" : "失败") << std::endl;
        
        // 测试过滤
        std::cout << "\n10. 过滤测试:" << std::endl;
        auto filteredArray = syntax.filterArray(std::vector<std::any>{1, 2, 3, 4, 5}, 
            [](const std::any& value) { 
                return value.type() == typeid(int) && std::any_cast<int>(value) % 2 == 0; 
            });
        std::cout << "过滤数组: " << (filteredArray.size() == 2 ? "成功" : "失败") << std::endl;
        
        // 测试映射
        std::cout << "\n11. 映射测试:" << std::endl;
        auto mappedArray = syntax.mapArray(std::vector<std::any>{1, 2, 3}, 
            [](const std::any& value) { 
                return std::any{std::any_cast<int>(value) * 2}; 
            });
        std::cout << "映射数组: " << (mappedArray.size() == 3 ? "成功" : "失败") << std::endl;
        
        // 测试归约
        std::cout << "\n12. 归约测试:" << std::endl;
        auto reducedValue = syntax.reduceArray(std::vector<std::any>{1, 2, 3, 4, 5}, 
            [](const std::any& acc, const std::any& value) { 
                return std::any{std::any_cast<int>(acc) + std::any_cast<int>(value)}; 
            }, std::any{0});
        std::cout << "归约数组: " << (std::any_cast<int>(reducedValue) == 15 ? "成功" : "失败") << std::endl;
        
        // 测试查找
        std::cout << "\n13. 查找测试:" << std::endl;
        auto foundValue = syntax.findArray(std::vector<std::any>{1, 2, 3, 4, 5}, 
            [](const std::any& value) { 
                return value.type() == typeid(int) && std::any_cast<int>(value) == 3; 
            });
        std::cout << "查找数组: " << (foundValue.has_value() ? "成功" : "失败") << std::endl;
        
        // 测试排序
        std::cout << "\n14. 排序测试:" << std::endl;
        auto sortedArray = syntax.sortArray(std::vector<std::any>{3, 1, 4, 1, 5}, 
            [](const std::any& a, const std::any& b) { 
                return std::any_cast<int>(a) < std::any_cast<int>(b); 
            });
        std::cout << "排序数组: " << (sortedArray.size() == 5 ? "成功" : "失败") << std::endl;
        
        // 测试统计
        std::cout << "\n15. 统计测试:" << std::endl;
        std::cout << "数组长度: " << syntax.getArrayLength(std::vector<std::any>{1, 2, 3, 4, 5}) << std::endl;
        std::cout << "对象大小: " << syntax.getObjectSize(std::map<std::string, std::any>{{"key1", "value1"}, {"key2", "value2"}}) << std::endl;
        
        auto objectKeys = syntax.getObjectKeys(std::map<std::string, std::any>{{"key1", "value1"}, {"key2", "value2"}});
        std::cout << "对象键数量: " << objectKeys.size() << std::endl;
        
        auto objectValues = syntax.getObjectValues(std::map<std::string, std::any>{{"key1", "value1"}, {"key2", "value2"}});
        std::cout << "对象值数量: " << objectValues.size() << std::endl;
        
        // 测试CJMOD参数
        std::cout << "\n16. CJMOD参数测试:" << std::endl;
        CHTL_JS::CJMODArg arg("testArg", std::string("testValue"), CHTL_JS::ArgType::REQUIRED);
        arg.setDescription("测试参数");
        arg.setTypeName("string");
        arg.setPosition(0);
        arg.setIndex(0);
        
        std::cout << "参数名称: " << arg.getName() << std::endl;
        std::cout << "参数类型: " << static_cast<int>(arg.getType()) << std::endl;
        std::cout << "是否必需: " << (arg.isRequired() ? "是" : "否") << std::endl;
        std::cout << "是否可选: " << (arg.isOptional() ? "是" : "否") << std::endl;
        std::cout << "参数描述: " << arg.getDescription() << std::endl;
        std::cout << "类型名称: " << arg.getTypeName() << std::endl;
        std::cout << "位置: " << arg.getPosition() << std::endl;
        std::cout << "索引: " << arg.getIndex() << std::endl;
        
        // 测试参数验证
        std::cout << "\n17. 参数验证测试:" << std::endl;
        auto argValidationResult = arg.validate();
        std::cout << "参数验证: " << (argValidationResult.valid ? "有效" : "无效") << std::endl;
        
        auto valueValidationResult = arg.validate(std::string("testValue"));
        std::cout << "值验证: " << (valueValidationResult.valid ? "有效" : "无效") << std::endl;
        
        // 测试参数绑定
        std::cout << "\n18. 参数绑定测试:" << std::endl;
        auto boundValue = arg.bind(std::string("boundValue"));
        std::cout << "绑定值: " << (boundValue.has_value() ? "成功" : "失败") << std::endl;
        
        auto boundArrayValue = arg.bind(std::vector<std::any>{std::string("arrayValue")});
        std::cout << "绑定数组值: " << (boundArrayValue.has_value() ? "成功" : "失败") << std::endl;
        
        auto boundObjectValue = arg.bind(std::map<std::string, std::any>{{"testArg", "objectValue"}});
        std::cout << "绑定对象值: " << (boundObjectValue.has_value() ? "成功" : "失败") << std::endl;
        
        // 测试参数转换
        std::cout << "\n19. 参数转换测试:" << std::endl;
        auto transformedValue = arg.transform(std::string("transformValue"));
        std::cout << "转换值: " << (transformedValue.has_value() ? "成功" : "失败") << std::endl;
        
        auto transformedString = arg.transformToString(std::string("transformString"));
        std::cout << "转换为字符串: " << transformedString << std::endl;
        
        // 测试参数填充
        std::cout << "\n20. 参数填充测试:" << std::endl;
        auto filledValue = arg.fillValue(std::string("filledValue"));
        std::cout << "填充值: " << (filledValue.has_value() ? "成功" : "失败") << std::endl;
        
        // 测试参数检查
        std::cout << "\n21. 参数检查测试:" << std::endl;
        std::cout << "类型检查: " << (arg.checkType(std::string("test")) ? "通过" : "失败") << std::endl;
        std::cout << "值检查: " << (arg.checkValue(std::string("test")) ? "通过" : "失败") << std::endl;
        std::cout << "必需检查: " << (arg.checkRequired(std::string("test")) ? "通过" : "失败") << std::endl;
        std::cout << "可选检查: " << (arg.checkOptional(std::string("test")) ? "通过" : "失败") << std::endl;
        
        // 测试参数比较
        std::cout << "\n22. 参数比较测试:" << std::endl;
        CHTL_JS::CJMODArg otherArg("testArg", std::string("testValue"), CHTL_JS::ArgType::REQUIRED);
        std::cout << "参数相等: " << (arg.equals(otherArg) ? "是" : "否") << std::endl;
        std::cout << "值相等: " << (arg.equals(std::string("testValue")) ? "是" : "否") << std::endl;
        
        // 测试参数克隆
        std::cout << "\n23. 参数克隆测试:" << std::endl;
        auto clonedArg = arg.clone();
        std::cout << "参数克隆: " << (clonedArg ? "成功" : "失败") << std::endl;
        
        if (clonedArg) {
            std::cout << "克隆参数名称: " << clonedArg->getName() << std::endl;
            std::cout << "克隆参数类型: " << static_cast<int>(clonedArg->getType()) << std::endl;
        }
        
        // 测试参数转换
        std::cout << "\n24. 参数转换测试:" << std::endl;
        std::cout << "字符串格式: " << arg.toString() << std::endl;
        std::cout << "调试格式: " << arg.toDebugString() << std::endl;
        std::cout << "JSON格式: " << arg.toJSON() << std::endl;
        std::cout << "XML格式: " << arg.toXML() << std::endl;
        std::cout << "YAML格式: " << arg.toYAML() << std::endl;
        
        // 测试参数格式化
        std::cout << "\n25. 参数格式化测试:" << std::endl;
        std::cout << "格式化: " << arg.format() << std::endl;
        std::cout << "压缩: " << arg.minify() << std::endl;
        std::cout << "美化: " << arg.beautify() << std::endl;
        
        // 测试参数统计
        std::cout << "\n26. 参数统计测试:" << std::endl;
        std::cout << "值大小: " << arg.getValueSize() << std::endl;
        std::cout << "值类型: " << arg.getValueType() << std::endl;
        std::cout << "有值: " << (arg.hasValue() ? "是" : "否") << std::endl;
        
        // 测试参数重置
        std::cout << "\n27. 参数重置测试:" << std::endl;
        arg.reset();
        std::cout << "重置后名称: " << arg.getName() << std::endl;
        std::cout << "重置后类型: " << static_cast<int>(arg.getType()) << std::endl;
        std::cout << "重置后是否必需: " << (arg.isRequired() ? "是" : "否") << std::endl;
        
        // 测试CJMOD扫描器
        std::cout << "\n28. CJMOD扫描器测试:" << std::endl;
        
        // 创建测试参数
        std::vector<CHTL_JS::CJMODArg> testArgs;
        testArgs.emplace_back("$", std::string("test1"));
        testArgs.emplace_back("**", std::string("power"));
        testArgs.emplace_back("$", std::string("test2"));
        
        // 双指针扫描测试
        auto scanResult = CHTL_JS::CJMODScanner::scan(testArgs, "**");
        std::cout << "双指针扫描结果数量: " << scanResult.size() << std::endl;
        
        // 前置截取测试
        std::string extractedCode = CHTL_JS::CJMODScanner::preExtract("arg ** arg2", "**");
        std::cout << "前置截取结果: " << extractedCode << std::endl;
        
        // 关键字检测测试
        bool hasKeyword = CHTL_JS::CJMODScanner::hasKeyword("arg ** arg2", "**");
        std::cout << "关键字检测: " << (hasKeyword ? "找到" : "未找到") << std::endl;
        
        // 代码片段处理测试
        auto fragments = CHTL_JS::CJMODScanner::extractFragments("arg ** arg2", "**");
        std::cout << "代码片段数量: " << fragments.size() << std::endl;
        
        // 参数处理测试
        auto processedArgs = CHTL_JS::CJMODScanner::processArgs(testArgs, "**");
        std::cout << "处理后参数数量: " << processedArgs.size() << std::endl;
        
        // 代码验证测试
        bool codeValid = CHTL_JS::CJMODScanner::validateCode("arg ** arg2");
        std::cout << "代码验证: " << (codeValid ? "有效" : "无效") << std::endl;
        
        bool argsValid = CHTL_JS::CJMODScanner::validateArgs(testArgs);
        std::cout << "参数验证: " << (argsValid ? "有效" : "无效") << std::endl;
        
        // 代码格式化测试
        std::string formattedCode = CHTL_JS::CJMODScanner::formatCode("arg ** arg2");
        std::cout << "代码格式化: " << formattedCode << std::endl;
        
        std::string formattedArgs = CHTL_JS::CJMODScanner::formatArgs(testArgs);
        std::cout << "参数格式化: " << formattedArgs << std::endl;
        
        // 统计信息测试
        auto codeStats = CHTL_JS::CJMODScanner::getStatistics("arg ** arg2");
        std::cout << "代码统计长度: " << codeStats["length"] << std::endl;
        
        auto argsStats = CHTL_JS::CJMODScanner::getArgsStatistics(testArgs);
        std::cout << "参数统计数量: " << argsStats["count"] << std::endl;
        
        // 调试功能测试
        std::string debugCode = CHTL_JS::CJMODScanner::debugCode("arg ** arg2");
        std::cout << "代码调试: " << debugCode << std::endl;
        
        std::string debugArgs = CHTL_JS::CJMODScanner::debugArgs(testArgs);
        std::cout << "参数调试: " << debugArgs << std::endl;
        
        // 序列化测试
        std::string jsonArgs = CHTL_JS::CJMODScanner::toJSON(testArgs);
        std::cout << "参数JSON序列化长度: " << jsonArgs.length() << std::endl;
        
        // 克隆测试
        auto clonedArgs = CHTL_JS::CJMODScanner::clone(testArgs);
        std::cout << "参数克隆数量: " << clonedArgs.size() << std::endl;
        
        // 比较测试
        bool argsEqual = CHTL_JS::CJMODScanner::equals(testArgs, clonedArgs);
        std::cout << "参数比较: " << (argsEqual ? "相等" : "不相等") << std::endl;
        
        // 测试CJMOD生成器
        std::cout << "\n29. CJMOD生成器测试:" << std::endl;
        
        // 基本生成功能测试
        std::string exportResult = CHTL_JS::CJMODGenerator::exportResult(testArgs);
        std::cout << "导出结果长度: " << exportResult.length() << std::endl;
        
        // 代码生成测试
        std::string generatedCode = CHTL_JS::CJMODGenerator::generateCode(testArgs);
        std::cout << "生成代码长度: " << generatedCode.length() << std::endl;
        
        // 模板处理测试
        std::string templateResult = CHTL_JS::CJMODGenerator::processTemplate("pow($, $)", testArgs);
        std::cout << "模板处理结果: " << templateResult << std::endl;
        
        // 变量替换测试
        std::map<std::string, std::any> variables;
        variables["$"] = std::string("3");
        std::string replacedCode = CHTL_JS::CJMODGenerator::replaceVariables("pow($, $)", variables);
        std::cout << "变量替换结果: " << replacedCode << std::endl;
        
        // 代码转换测试
        std::string jsCode = CHTL_JS::CJMODGenerator::convertToJavaScript("pow(3, 4)");
        std::cout << "转换为JavaScript: " << jsCode << std::endl;
        
        // 代码优化测试
        std::string optimizedCode = CHTL_JS::CJMODGenerator::optimizeCode("pow(3, 4)");
        std::cout << "优化代码: " << optimizedCode << std::endl;
        
        // 代码压缩测试
        std::string minifiedCode = CHTL_JS::CJMODGenerator::minifyCode("pow( 3 , 4 )");
        std::cout << "压缩代码: " << minifiedCode << std::endl;
        
        // 代码美化测试
        std::string beautifiedCode = CHTL_JS::CJMODGenerator::beautifyCode("pow(3,4)");
        std::cout << "美化代码: " << beautifiedCode << std::endl;
        
        // 代码验证测试
        bool generatorCodeValid = CHTL_JS::CJMODGenerator::validateCode("pow(3, 4)");
        std::cout << "生成器代码验证: " << (generatorCodeValid ? "有效" : "无效") << std::endl;
        
        // 代码分析测试
        auto analysis = CHTL_JS::CJMODGenerator::analyzeCode("pow(3, 4)");
        std::cout << "代码分析结果数量: " << analysis.size() << std::endl;
        
        // 代码统计测试
        auto generatorStats = CHTL_JS::CJMODGenerator::getStatistics("pow(3, 4)");
        std::cout << "生成器统计结果数量: " << generatorStats.size() << std::endl;
        
        // 代码调试测试
        std::string generatorDebug = CHTL_JS::CJMODGenerator::debugCode("pow(3, 4)");
        std::cout << "生成器调试: " << generatorDebug << std::endl;
        
        // 代码比较测试
        bool codeComparison = CHTL_JS::CJMODGenerator::compareCode("pow(3, 4)", "pow(3, 4)");
        std::cout << "代码比较: " << (codeComparison ? "相同" : "不同") << std::endl;
        
        // 代码合并测试
        std::vector<std::string> codes = {"pow(", "3, 4", ")"};
        std::string mergedCode = CHTL_JS::CJMODGenerator::mergeCode(codes);
        std::cout << "代码合并: " << mergedCode << std::endl;
        
        // 代码分割测试
        auto splitResult = CHTL_JS::CJMODGenerator::splitCode("pow(3, 4)");
        std::cout << "代码分割数量: " << splitResult.size() << std::endl;
        
        // 工具函数测试
        std::string typeString = CHTL_JS::CJMODGenerator::codeTypeToString("javascript");
        std::cout << "类型转换: " << typeString << std::endl;
        
        bool validType = CHTL_JS::CJMODGenerator::isValidCodeType("javascript");
        std::cout << "类型验证: " << (validType ? "有效" : "无效") << std::endl;
        
        // 序列化测试
        std::string jsonCode = CHTL_JS::CJMODGenerator::toJSON("pow(3, 4)");
        std::cout << "代码JSON序列化长度: " << jsonCode.length() << std::endl;
        
        // 克隆测试
        std::string clonedCode = CHTL_JS::CJMODGenerator::clone("pow(3, 4)");
        std::cout << "代码克隆: " << clonedCode << std::endl;
        
        // 格式化测试
        std::string formattedGeneratorCode = CHTL_JS::CJMODGenerator::format("pow(3,4)");
        std::cout << "生成器格式化: " << formattedGeneratorCode << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CJMOD API测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    // 统一扫描器测试
    std::cout << "\n\n统一扫描器测试" << std::endl;
    std::cout << "=================" << std::endl;
    
    try {
        // 测试统一扫描器
        std::cout << "\n1. 统一扫描器基本功能测试:" << std::endl;
        CHTL::UnifiedScanner scanner;
        
        // 测试代码扫描
        std::string testCode = R"(
            [Element] div {
                class: "container"
            }
            
            script {
                {{console.log("Hello World")}};
                function test() {
                    return "test";
                }
            }
            
            style {
                .container {
                    color: red;
                }
            }
        )";
        
        CHTL::ScanConfig config;
        config.enableCHTL = true;
        config.enableCHTLJS = true;
        config.enableJavaScript = true;
        config.enableCSS = true;
        config.enableHTML = true;
        
        auto scanResult = scanner.scan(testCode, config);
        std::cout << "扫描成功: " << (scanResult.success ? "是" : "否") << std::endl;
        std::cout << "代码片段数量: " << scanResult.fragments.size() << std::endl;
        std::cout << "语法边界数量: " << scanResult.boundaries.size() << std::endl;
        std::cout << "占位符数量: " << scanResult.placeholders.size() << std::endl;
        
        // 测试代码分离
        std::cout << "\n2. 代码分离测试:" << std::endl;
        auto fragments = scanner.separateCode(testCode, config);
        std::cout << "分离片段数量: " << fragments.size() << std::endl;
        
        auto fragmentsByType = scanner.separateCodeByType(testCode, config);
        std::cout << "CHTL片段数量: " << fragmentsByType[CHTL::CodeType::CHTL].size() << std::endl;
        std::cout << "CHTL JS片段数量: " << fragmentsByType[CHTL::CodeType::CHTL_JS].size() << std::endl;
        std::cout << "JavaScript片段数量: " << fragmentsByType[CHTL::CodeType::JAVASCRIPT].size() << std::endl;
        std::cout << "CSS片段数量: " << fragmentsByType[CHTL::CodeType::CSS].size() << std::endl;
        std::cout << "HTML片段数量: " << fragmentsByType[CHTL::CodeType::HTML].size() << std::endl;
        
        // 测试语法边界检测
        std::cout << "\n3. 语法边界检测测试:" << std::endl;
        auto boundaries = scanner.detectBoundaries(testCode, config);
        std::cout << "检测到边界数量: " << boundaries.size() << std::endl;
        
        auto chtlBoundaries = scanner.detectBoundariesByType(testCode, CHTL::CodeType::CHTL, config);
        std::cout << "CHTL边界数量: " << chtlBoundaries.size() << std::endl;
        
        auto chtljsBoundaries = scanner.detectBoundariesByType(testCode, CHTL::CodeType::CHTL_JS, config);
        std::cout << "CHTL JS边界数量: " << chtljsBoundaries.size() << std::endl;
        
        // 测试占位符管理
        std::cout << "\n4. 占位符管理测试:" << std::endl;
        auto placeholders = scanner.createPlaceholders(testCode, config);
        std::cout << "创建占位符数量: " << placeholders.size() << std::endl;
        
        std::string replacedCode = scanner.replacePlaceholders(testCode, placeholders);
        std::cout << "替换后代码长度: " << replacedCode.length() << std::endl;
        
        std::string restoredCode = scanner.restorePlaceholders(replacedCode, placeholders);
        std::cout << "恢复后代码长度: " << restoredCode.length() << std::endl;
        
        // 测试代码类型检测
        std::cout << "\n5. 代码类型检测测试:" << std::endl;
        auto detectedType = scanner.detectCodeType(testCode, 0, config);
        std::cout << "检测到的代码类型: " << static_cast<int>(detectedType) << std::endl;
        
        auto chtlType = scanner.detectCodeTypeByContent("[Element] div", config);
        std::cout << "CHTL类型检测: " << static_cast<int>(chtlType) << std::endl;
        
        auto chtljsType = scanner.detectCodeTypeByContent("{{console.log}}", config);
        std::cout << "CHTL JS类型检测: " << static_cast<int>(chtljsType) << std::endl;
        
        // 测试代码验证
        std::cout << "\n6. 代码验证测试:" << std::endl;
        bool codeValid = scanner.validateCode(testCode, config);
        std::cout << "代码验证: " << (codeValid ? "有效" : "无效") << std::endl;
        
        if (!fragments.empty()) {
            bool fragmentValid = scanner.validateFragment(fragments[0], config);
            std::cout << "片段验证: " << (fragmentValid ? "有效" : "无效") << std::endl;
        }
        
        if (!boundaries.empty()) {
            bool boundaryValid = scanner.validateBoundary(boundaries[0], config);
            std::cout << "边界验证: " << (boundaryValid ? "有效" : "无效") << std::endl;
        }
        
        // 测试变长切片功能
        std::cout << "\n7. 变长切片功能测试:" << std::endl;
        auto variableSlices = scanner.variableLengthSlicing(testCode, config);
        std::cout << "变长切片数量: " << variableSlices.size() << std::endl;
        
        auto expandedSlices = scanner.smartExpansion(testCode, 0, 50, config);
        std::cout << "智能展开片段数量: " << expandedSlices.size() << std::endl;
        
        auto rolledBackSlices = scanner.smartRollback(testCode, 0, 100, config);
        std::cout << "智能回滚片段数量: " << rolledBackSlices.size() << std::endl;
        
        // 测试语法边界识别
        std::cout << "\n8. 语法边界识别测试:" << std::endl;
        bool isBoundary = scanner.isSyntaxBoundary(testCode, 10, config);
        std::cout << "是否为语法边界: " << (isBoundary ? "是" : "否") << std::endl;
        
        auto boundaryType = scanner.getBoundaryType(testCode, 10, config);
        std::cout << "边界类型: " << static_cast<int>(boundaryType) << std::endl;
        
        auto foundBoundaries = scanner.findBoundaries(testCode, CHTL::SyntaxBoundaryType::CHTL_START, config);
        std::cout << "找到的CHTL开始边界数量: " << foundBoundaries.size() << std::endl;
        
        // 测试宽泛和严格判断
        std::cout << "\n9. 宽泛和严格判断测试:" << std::endl;
        bool isWide = scanner.isWideMode(testCode, 10, config);
        std::cout << "是否为宽泛模式: " << (isWide ? "是" : "否") << std::endl;
        
        bool isStrict = scanner.isStrictMode(testCode, 10, config);
        std::cout << "是否为严格模式: " << (isStrict ? "是" : "否") << std::endl;
        
        bool shouldUseWide = scanner.shouldUseWideMode(testCode, 10, config);
        std::cout << "应使用宽泛模式: " << (shouldUseWide ? "是" : "否") << std::endl;
        
        bool shouldUseStrict = scanner.shouldUseStrictMode(testCode, 10, config);
        std::cout << "应使用严格模式: " << (shouldUseStrict ? "是" : "否") << std::endl;
        
        // 测试针对性处理
        std::cout << "\n10. 针对性处理测试:" << std::endl;
        auto targetedFragments = scanner.processTargeted(testCode, fragments, config);
        std::cout << "针对性处理片段数量: " << targetedFragments.size() << std::endl;
        
        auto chtlProcessed = scanner.processByType(testCode, CHTL::CodeType::CHTL, config);
        std::cout << "CHTL类型处理片段数量: " << chtlProcessed.size() << std::endl;
        
        auto boundaryProcessed = scanner.processByBoundary(testCode, boundaries, config);
        std::cout << "边界处理片段数量: " << boundaryProcessed.size() << std::endl;
        
        // 测试代码转换
        std::cout << "\n11. 代码转换测试:" << std::endl;
        std::string convertedCode = scanner.convertCode(testCode, CHTL::CodeType::CHTL, CHTL::CodeType::HTML, config);
        std::cout << "代码转换结果长度: " << convertedCode.length() << std::endl;
        
        if (!fragments.empty()) {
            std::string convertedFragment = scanner.convertFragment(fragments[0], CHTL::CodeType::HTML, config);
            std::cout << "片段转换结果长度: " << convertedFragment.length() << std::endl;
        }
        
        if (!boundaries.empty()) {
            std::string convertedBoundary = scanner.convertBoundary(boundaries[0], CHTL::CodeType::HTML, config);
            std::cout << "边界转换结果长度: " << convertedBoundary.length() << std::endl;
        }
        
        // 测试代码合并
        std::cout << "\n12. 代码合并测试:" << std::endl;
        std::string mergedFragments = scanner.mergeFragments(fragments, config);
        std::cout << "合并片段结果长度: " << mergedFragments.length() << std::endl;
        
        std::string mergedByType = scanner.mergeByType(fragmentsByType, config);
        std::cout << "按类型合并结果长度: " << mergedByType.length() << std::endl;
        
        std::string mergedWithBoundaries = scanner.mergeWithBoundaries(fragments, boundaries, config);
        std::cout << "带边界合并结果长度: " << mergedWithBoundaries.length() << std::endl;
        
        // 测试代码格式化
        std::cout << "\n13. 代码格式化测试:" << std::endl;
        std::string formattedCode = scanner.formatCode(testCode, config);
        std::cout << "格式化代码长度: " << formattedCode.length() << std::endl;
        
        if (!fragments.empty()) {
            std::string formattedFragment = scanner.formatFragment(fragments[0], config);
            std::cout << "格式化片段长度: " << formattedFragment.length() << std::endl;
        }
        
        if (!boundaries.empty()) {
            std::string formattedBoundary = scanner.formatBoundary(boundaries[0], config);
            std::cout << "格式化边界长度: " << formattedBoundary.length() << std::endl;
        }
        
        // 测试代码压缩
        std::cout << "\n14. 代码压缩测试:" << std::endl;
        std::string minifiedCode = scanner.minifyCode(testCode, config);
        std::cout << "压缩代码长度: " << minifiedCode.length() << std::endl;
        
        if (!fragments.empty()) {
            std::string minifiedFragment = scanner.minifyFragment(fragments[0], config);
            std::cout << "压缩片段长度: " << minifiedFragment.length() << std::endl;
        }
        
        if (!boundaries.empty()) {
            std::string minifiedBoundary = scanner.minifyBoundary(boundaries[0], config);
            std::cout << "压缩边界长度: " << minifiedBoundary.length() << std::endl;
        }
        
        // 测试代码美化
        std::cout << "\n15. 代码美化测试:" << std::endl;
        std::string beautifiedCode = scanner.beautifyCode(testCode, config);
        std::cout << "美化代码长度: " << beautifiedCode.length() << std::endl;
        
        if (!fragments.empty()) {
            std::string beautifiedFragment = scanner.beautifyFragment(fragments[0], config);
            std::cout << "美化片段长度: " << beautifiedFragment.length() << std::endl;
        }
        
        if (!boundaries.empty()) {
            std::string beautifiedBoundary = scanner.beautifyBoundary(boundaries[0], config);
            std::cout << "美化边界长度: " << beautifiedBoundary.length() << std::endl;
        }
        
        // 测试统计信息
        std::cout << "\n16. 统计信息测试:" << std::endl;
        auto scannerStats = scanner.getStatistics(testCode, config);
        std::cout << "代码统计信息数量: " << scannerStats.size() << std::endl;
        
        auto fragmentStats = scanner.getFragmentStatistics(fragments, config);
        std::cout << "片段统计信息数量: " << fragmentStats.size() << std::endl;
        
        auto boundaryStats = scanner.getBoundaryStatistics(boundaries, config);
        std::cout << "边界统计信息数量: " << boundaryStats.size() << std::endl;
        
        // 测试调试功能
        std::cout << "\n17. 调试功能测试:" << std::endl;
        std::string debugCode = scanner.debugCode(testCode, config);
        std::cout << "调试代码长度: " << debugCode.length() << std::endl;
        
        if (!fragments.empty()) {
            std::string debugFragment = scanner.debugFragment(fragments[0], config);
            std::cout << "调试片段长度: " << debugFragment.length() << std::endl;
        }
        
        if (!boundaries.empty()) {
            std::string debugBoundary = scanner.debugBoundary(boundaries[0], config);
            std::cout << "调试边界长度: " << debugBoundary.length() << std::endl;
        }
        
        std::string debugScanResult = scanner.debugScanResult(scanResult, config);
        std::cout << "调试扫描结果长度: " << debugScanResult.length() << std::endl;
        
        // 测试配置管理
        std::cout << "\n18. 配置管理测试:" << std::endl;
        scanner.setConfig(config);
        auto currentConfig = scanner.getConfig();
        std::cout << "获取配置成功: " << (currentConfig.enableCHTL ? "是" : "否") << std::endl;
        
        scanner.resetConfig();
        auto resetConfig = scanner.getConfig();
        std::cout << "重置配置成功: " << (resetConfig.enableCHTL ? "是" : "否") << std::endl;
        
        // 测试状态管理
        std::cout << "\n19. 状态管理测试:" << std::endl;
        std::cout << "是否为空: " << (scanner.isEmpty() ? "是" : "否") << std::endl;
        std::cout << "片段数量: " << scanner.getFragmentCount() << std::endl;
        std::cout << "边界数量: " << scanner.getBoundaryCount() << std::endl;
        std::cout << "占位符数量: " << scanner.getPlaceholderCount() << std::endl;
        
        scanner.clear();
        std::cout << "清理后是否为空: " << (scanner.isEmpty() ? "是" : "否") << std::endl;
        
        scanner.reset();
        std::cout << "重置后是否为空: " << (scanner.isEmpty() ? "是" : "否") << std::endl;
        
        // 测试错误处理
        std::cout << "\n20. 错误处理测试:" << std::endl;
        std::cout << "是否有错误: " << (scanner.hasError() ? "是" : "否") << std::endl;
        std::cout << "最后错误: " << scanner.getLastError() << std::endl;
        
        scanner.clearError();
        std::cout << "清除错误后是否有错误: " << (scanner.hasError() ? "是" : "否") << std::endl;
        
        // 测试工具函数
        std::cout << "\n21. 工具函数测试:" << std::endl;
        std::string typeString = CHTL::UnifiedScanner::codeTypeToString(CHTL::CodeType::CHTL);
        std::cout << "代码类型转字符串: " << typeString << std::endl;
        
        auto stringType = CHTL::UnifiedScanner::stringToCodeType("CHTL");
        std::cout << "字符串转代码类型: " << static_cast<int>(stringType) << std::endl;
        
        bool validCodeType = CHTL::UnifiedScanner::isValidCodeType(CHTL::CodeType::CHTL);
        std::cout << "代码类型有效: " << (validCodeType ? "是" : "否") << std::endl;
        
        std::string boundaryString = CHTL::UnifiedScanner::boundaryTypeToString(CHTL::SyntaxBoundaryType::CHTL_START);
        std::cout << "边界类型转字符串: " << boundaryString << std::endl;
        
        auto stringBoundary = CHTL::UnifiedScanner::stringToBoundaryType("CHTL_START");
        std::cout << "字符串转边界类型: " << static_cast<int>(stringBoundary) << std::endl;
        
        bool validBoundaryType = CHTL::UnifiedScanner::isValidBoundaryType(CHTL::SyntaxBoundaryType::CHTL_START);
        std::cout << "边界类型有效: " << (validBoundaryType ? "是" : "否") << std::endl;
        
        // 测试序列化
        std::cout << "\n22. 序列化测试:" << std::endl;
        std::string jsonString = scanner.toJSON();
        std::cout << "JSON序列化长度: " << jsonString.length() << std::endl;
        
        std::string xmlString = scanner.toXML();
        std::cout << "XML序列化长度: " << xmlString.length() << std::endl;
        
        std::string yamlString = scanner.toYAML();
        std::cout << "YAML序列化长度: " << yamlString.length() << std::endl;
        
        std::string scannerString = scanner.toString();
        std::cout << "字符串序列化长度: " << scannerString.length() << std::endl;
        
        std::string debugString = scanner.toDebugString();
        std::cout << "调试字符串序列化长度: " << debugString.length() << std::endl;
        
        // 测试克隆
        std::cout << "\n23. 克隆测试:" << std::endl;
        auto clonedScanner = scanner.clone();
        std::cout << "克隆扫描器: " << (clonedScanner ? "成功" : "失败") << std::endl;
        
        // 测试比较
        std::cout << "\n24. 比较测试:" << std::endl;
        if (clonedScanner) {
            bool scannersEqual = scanner.equals(*clonedScanner);
            std::cout << "扫描器相等: " << (scannersEqual ? "是" : "否") << std::endl;
        }
        
        bool codeEqual = scanner.equals(testCode);
        std::cout << "代码相等: " << (codeEqual ? "是" : "否") << std::endl;
        
        // 测试格式化
        std::cout << "\n25. 格式化测试:" << std::endl;
        std::string scannerFormat = scanner.format();
        std::cout << "扫描器格式化长度: " << scannerFormat.length() << std::endl;
        
        std::string scannerMinify = scanner.minify();
        std::cout << "扫描器压缩长度: " << scannerMinify.length() << std::endl;
        
        std::string scannerBeautify = scanner.beautify();
        std::cout << "扫描器美化长度: " << scannerBeautify.length() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "统一扫描器测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    // 编译器调度器测试
    std::cout << "\n\n编译器调度器测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    try {
        // 测试编译器调度器
        std::cout << "\n1. 编译器调度器基本功能测试:" << std::endl;
        CHTL::CompilerDispatcher dispatcher;
        
        // 设置配置
        dispatcher.setDebugMode(true);
        dispatcher.setStrictMode(true);
        dispatcher.setGenerateDefaultStructure(false);
        dispatcher.setOutputFormat("html");
        
        std::cout << "调试模式: " << (dispatcher.getErrors().empty() ? "设置成功" : "设置失败") << std::endl;
        std::cout << "严格模式: " << (dispatcher.getErrors().empty() ? "设置成功" : "设置失败") << std::endl;
        std::cout << "输出格式: HTML" << std::endl;
        
        // 测试配置验证
        bool configValid = dispatcher.validateConfiguration();
        std::cout << "配置验证: " << (configValid ? "通过" : "失败") << std::endl;
        
        // 测试编译器信息
        std::string compilerInfo = dispatcher.getCompilerInfo();
        std::cout << "编译器信息长度: " << compilerInfo.length() << std::endl;
        
        std::string version = dispatcher.getVersion();
        std::cout << "编译器版本: " << version << std::endl;
        
        // 测试CSS编译器
        std::cout << "\n2. CSS编译器测试:" << std::endl;
        CHTL::CSSCompiler cssCompiler;
        
        std::string cssCode = R"(
            .container {
                color: red;
                background: blue;
            }
            
            .header {
                font-size: 16px;
                margin: 10px;
            }
        )";
        
        auto cssResult = cssCompiler.compile(cssCode);
        std::cout << "CSS编译成功: " << (cssResult.success ? "是" : "否") << std::endl;
        std::cout << "CSS输出长度: " << cssResult.css.length() << std::endl;
        std::cout << "CSS错误数量: " << cssResult.errors.size() << std::endl;
        std::cout << "CSS警告数量: " << cssResult.warnings.size() << std::endl;
        
        // 测试CSS规则解析
        auto cssRules = cssCompiler.parseRules(cssCode);
        std::cout << "CSS规则数量: " << cssRules.size() << std::endl;
        
        // 测试CSS生成
        std::string generatedCSS = cssCompiler.generateCSS(cssRules);
        std::cout << "生成CSS长度: " << generatedCSS.length() << std::endl;
        
        // 测试CSS优化
        std::string optimizedCSS = cssCompiler.optimizeCSS(cssCode);
        std::cout << "优化CSS长度: " << optimizedCSS.length() << std::endl;
        
        std::string minifiedCSS = cssCompiler.minifyCSS(cssCode);
        std::cout << "压缩CSS长度: " << minifiedCSS.length() << std::endl;
        
        std::string beautifiedCSS = cssCompiler.beautifyCSS(cssCode);
        std::cout << "美化CSS长度: " << beautifiedCSS.length() << std::endl;
        
        // 测试CSS验证
        bool cssValid = cssCompiler.validateCSS(cssCode);
        std::cout << "CSS验证: " << (cssValid ? "通过" : "失败") << std::endl;
        
        // 测试CSS统计
        auto cssStats = cssCompiler.getStatistics(cssCode);
        std::cout << "CSS统计信息数量: " << cssStats.size() << std::endl;
        
        // 测试CSS调试
        std::string cssDebug = cssCompiler.debugCSS(cssCode);
        std::cout << "CSS调试信息长度: " << cssDebug.length() << std::endl;
        
        // 测试JS编译器
        std::cout << "\n3. JS编译器测试:" << std::endl;
        CHTL::JSCompiler jsCompiler;
        
        std::string jsCode = R"(
            function test() {
                var x = 1;
                var y = 2;
                return x + y;
            }
            
            if (true) {
                console.log("Hello World");
            }
        )";
        
        auto jsResult = jsCompiler.compile(jsCode);
        std::cout << "JS编译成功: " << (jsResult.success ? "是" : "否") << std::endl;
        std::cout << "JS输出长度: " << jsResult.javascript.length() << std::endl;
        std::cout << "JS错误数量: " << jsResult.errors.size() << std::endl;
        std::cout << "JS警告数量: " << jsResult.warnings.size() << std::endl;
        
        // 测试JS AST解析
        auto jsAST = jsCompiler.parseAST(jsCode);
        std::cout << "JS AST节点数量: " << (jsAST ? jsAST->children.size() : 0) << std::endl;
        
        // 测试JS生成
        std::string generatedJS = jsCompiler.generateJS(jsAST);
        std::cout << "生成JS长度: " << generatedJS.length() << std::endl;
        
        // 测试JS优化
        std::string optimizedJS = jsCompiler.optimizeJS(jsCode);
        std::cout << "优化JS长度: " << optimizedJS.length() << std::endl;
        
        std::string minifiedJS = jsCompiler.minifyJS(jsCode);
        std::cout << "压缩JS长度: " << minifiedJS.length() << std::endl;
        
        std::string beautifiedJS = jsCompiler.beautifyJS(jsCode);
        std::cout << "美化JS长度: " << beautifiedJS.length() << std::endl;
        
        // 测试JS验证
        bool jsValid = jsCompiler.validateJS(jsCode);
        std::cout << "JS验证: " << (jsValid ? "通过" : "失败") << std::endl;
        
        // 测试JS统计
        auto jsStats = jsCompiler.getStatistics(jsCode);
        std::cout << "JS统计信息数量: " << jsStats.size() << std::endl;
        
        // 测试JS调试
        std::string jsDebug = jsCompiler.debugJS(jsCode);
        std::cout << "JS调试信息长度: " << jsDebug.length() << std::endl;
        
        // 测试编译器调度器完整流程
        std::cout << "\n4. 编译器调度器完整流程测试:" << std::endl;
        
        // 创建测试文件内容
        std::string testFileContent = R"(
            [Element] div {
                class: "container"
            }
            
            script {
                {{console.log("Hello World")}};
                function test() {
                    return "test";
                }
            }
            
            style {
                .container {
                    color: red;
                }
            }
        )";
        
        // 写入测试文件
        std::ofstream testFile("test.chtl");
        testFile << testFileContent;
        testFile.close();
        
        // 测试扫描
        auto scanResult = dispatcher.scan("test.chtl");
        std::cout << "扫描成功: " << (scanResult.errors.empty() ? "是" : "否") << std::endl;
        std::cout << "扫描片段数量: " << scanResult.fragments.size() << std::endl;
        std::cout << "扫描错误数量: " << scanResult.errors.size() << std::endl;
        std::cout << "扫描警告数量: " << scanResult.warnings.size() << std::endl;
        
        // 测试编译
        auto compileResult = dispatcher.compile(scanResult);
        std::cout << "编译成功: " << (compileResult.errors.empty() ? "是" : "否") << std::endl;
        std::cout << "编译输出类型数量: " << compileResult.outputs.size() << std::endl;
        std::cout << "编译错误数量: " << compileResult.errors.size() << std::endl;
        std::cout << "编译警告数量: " << compileResult.warnings.size() << std::endl;
        
        // 测试合并
        auto finalResult = dispatcher.merge(compileResult);
        std::cout << "合并成功: " << (finalResult.errors.empty() ? "是" : "否") << std::endl;
        std::cout << "HTML输出长度: " << finalResult.html.length() << std::endl;
        std::cout << "CSS输出长度: " << finalResult.css.length() << std::endl;
        std::cout << "JavaScript输出长度: " << finalResult.javascript.length() << std::endl;
        std::cout << "合并错误数量: " << finalResult.errors.size() << std::endl;
        std::cout << "合并警告数量: " << finalResult.warnings.size() << std::endl;
        
        // 测试输出
        dispatcher.output(finalResult, "test_output.html");
        std::cout << "输出文件创建: 成功" << std::endl;
        
        // 测试一步完成编译
        auto oneStepResult = dispatcher.compileFile("test.chtl");
        std::cout << "一步编译成功: " << (oneStepResult.errors.empty() ? "是" : "否") << std::endl;
        std::cout << "一步编译HTML长度: " << oneStepResult.html.length() << std::endl;
        
        // 测试编译到输出文件
        dispatcher.compileFileToOutput("test.chtl", "test_final.html");
        std::cout << "编译到输出文件: 成功" << std::endl;
        
        // 测试错误和警告获取
        std::cout << "\n5. 错误和警告处理测试:" << std::endl;
        const auto& errors = dispatcher.getErrors();
        const auto& warnings = dispatcher.getWarnings();
        std::cout << "当前错误数量: " << errors.size() << std::endl;
        std::cout << "当前警告数量: " << warnings.size() << std::endl;
        
        // 测试清除消息
        dispatcher.clearMessages();
        std::cout << "清除消息后错误数量: " << dispatcher.getErrors().size() << std::endl;
        std::cout << "清除消息后警告数量: " << dispatcher.getWarnings().size() << std::endl;
        
        // 测试代码合并器
        std::cout << "\n6. 代码合并器测试:" << std::endl;
        CHTL::CodeMerger merger;
        
        // 设置配置
        merger.setDebugMode(true);
        merger.setMinifyOutput(false);
        merger.setBeautifyOutput(true);
        merger.setTitle("测试文档");
        merger.setLanguage("zh-CN");
        
        // 添加元标签
        merger.addMetaTag("description", "CHTL测试文档");
        merger.addMetaTag("keywords", "CHTL, 测试, 编译器");
        merger.addMetaTag("author", "CHTL Team");
        
        // 添加外部资源
        merger.addExternalCSS("bootstrap.css");
        merger.addExternalJS("jquery.js");
        
        std::cout << "代码合并器配置完成" << std::endl;
        std::cout << "调试模式: " << (merger.getDebugMode() ? "开启" : "关闭") << std::endl;
        std::cout << "文档标题: " << merger.getTitle() << std::endl;
        std::cout << "语言: " << merger.getLanguage() << std::endl;
        
        // 测试代码分析
        auto htmlStats = merger.analyzeHTML(finalResult.html);
        std::cout << "HTML统计信息数量: " << htmlStats.size() << std::endl;
        
        auto cssStats = merger.analyzeCSS(finalResult.css);
        std::cout << "CSS统计信息数量: " << cssStats.size() << std::endl;
        
        auto jsStats = merger.analyzeJavaScript(finalResult.javascript);
        std::cout << "JavaScript统计信息数量: " << jsStats.size() << std::endl;
        
        // 测试代码转换
        std::string inlineHTML = merger.convertToInline(finalResult);
        std::cout << "内联HTML长度: " << inlineHTML.length() << std::endl;
        
        std::string externalHTML = merger.convertToExternal(finalResult);
        std::cout << "外部HTML长度: " << externalHTML.length() << std::endl;
        
        std::string minifiedHTML = merger.convertToMinified(finalResult);
        std::cout << "压缩HTML长度: " << minifiedHTML.length() << std::endl;
        
        std::string beautifiedHTML = merger.convertToBeautified(finalResult);
        std::cout << "美化HTML长度: " << beautifiedHTML.length() << std::endl;
        
        // 测试模板管理
        std::string currentTemplate = merger.getHTMLTemplate();
        std::cout << "当前模板长度: " << currentTemplate.length() << std::endl;
        
        merger.setHTMLTemplate("<html><body>Custom Template</body></html>");
        std::cout << "自定义模板设置: 完成" << std::endl;
        
        merger.resetHTMLTemplate();
        std::cout << "模板重置: 完成" << std::endl;
        
        // 测试清理功能
        merger.clearMetaTags();
        std::cout << "元标签清理: 完成" << std::endl;
        
        merger.clearExternalCSS();
        std::cout << "外部CSS清理: 完成" << std::endl;
        
        merger.clearExternalJS();
        std::cout << "外部JS清理: 完成" << std::endl;
        
        // 测试调试信息
        std::string debugInfo = merger.getDebugInfo();
        std::cout << "调试信息长度: " << debugInfo.length() << std::endl;
        
        // 测试验证
        bool mergerValid = merger.validate(finalResult);
        std::cout << "合并器验证: " << (mergerValid ? "通过" : "失败") << std::endl;
        
        // 测试CLI工具
        std::cout << "\n7. CLI工具测试:" << std::endl;
        CHTL::CLITool cliTool;
        
        // 设置版本和描述
        cliTool.setVersion("1.0.0");
        cliTool.setDescription("CHTL Compiler CLI Tool");
        
        // 测试文件处理器
        std::cout << "测试文件处理器功能:" << std::endl;
        std::string testFile = "test_cli.chtl";
        std::string testContent = "div: Hello World";
        
        // 创建测试文件
        if (CHTL::FileProcessor::writeFile(testFile, testContent)) {
            std::cout << "测试文件创建: 成功" << std::endl;
        }
        
        // 测试文件存在性检查
        bool fileExists = CHTL::FileProcessor::fileExists(testFile);
        std::cout << "文件存在性检查: " << (fileExists ? "通过" : "失败") << std::endl;
        
        // 测试文件读取
        std::string readContent = CHTL::FileProcessor::readFile(testFile);
        std::cout << "文件读取: " << (readContent == testContent ? "成功" : "失败") << std::endl;
        
        // 测试文件扩展名获取
        std::string extension = CHTL::FileProcessor::getFileExtension(testFile);
        std::cout << "文件扩展名: " << extension << std::endl;
        
        // 测试文件名获取
        std::string fileName = CHTL::FileProcessor::getFileName(testFile);
        std::cout << "文件名: " << fileName << std::endl;
        
        // 测试目录获取
        std::string directory = CHTL::FileProcessor::getDirectory(testFile);
        std::cout << "目录: " << directory << std::endl;
        
        // 测试颜色输出
        std::cout << "测试颜色输出功能:" << std::endl;
        CHTL::ColorOutput::printInfo("信息消息");
        CHTL::ColorOutput::printSuccess("成功消息");
        CHTL::ColorOutput::printWarning("警告消息");
        CHTL::ColorOutput::printError("错误消息");
        
        // 测试进度条
        std::cout << "测试进度条功能:" << std::endl;
        CHTL::ProgressBar progressBar(100, 50);
        for (int i = 0; i <= 100; i += 10) {
            progressBar.update(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        progressBar.finish();
        
        // 测试文件列表
        std::vector<std::string> files = CHTL::FileProcessor::listFiles(".", ".chtl");
        std::cout << "找到的.chtl文件数量: " << files.size() << std::endl;
        
        // 测试CHTL核心词法分析器
        std::cout << "\n8. CHTL核心词法分析器测试:" << std::endl;
        CHTL::CHTLLexer lexer;
        
        // 测试基本词法分析
        std::string testCode = R"(
// 这是单行注释
/* 这是多行注释 */
-- 这是生成器注释

div: Hello World
div.class: Text content
div#id: Another text

[Template] MyTemplate
[Custom] MyCustom
[Origin] @Html
[Import] MyModule
[Namespace] MyNamespace
[Configuration] MyConfig

use html5

div {
    class: "container"
    id: "main"
    style: "color: red"
}

div: Text with := colon equal
        )";
        
        lexer.setSource(testCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "词法分析完成，Token数量: " << tokens.size() << std::endl;
        
        // 测试特定Token类型
        int elementCount = 0;
        int commentCount = 0;
        int colonEqualCount = 0;
        int templateCount = 0;
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            auto token = tokens.peek(i);
            if (token.type == CHTL::TokenType::ELEMENT_NAME) {
                elementCount++;
            } else if (token.type == CHTL::TokenType::SINGLE_COMMENT || 
                      token.type == CHTL::TokenType::MULTI_COMMENT || 
                      token.type == CHTL::TokenType::GENERATOR_COMMENT) {
                commentCount++;
            } else if (token.type == CHTL::TokenType::COLON_EQUAL) {
                colonEqualCount++;
            } else if (token.type == CHTL::TokenType::TEMPLATE) {
                templateCount++;
            }
        }
        
        std::cout << "元素名称Token数量: " << elementCount << std::endl;
        std::cout << "注释Token数量: " << commentCount << std::endl;
        std::cout << "CE对等式Token数量: " << colonEqualCount << std::endl;
        std::cout << "模板Token数量: " << templateCount << std::endl;
        
        // 测试位置信息
        if (tokens.size() > 0) {
            auto firstToken = tokens.peek(0);
            std::cout << "第一个Token位置: 行" << firstToken.line << ", 列" << firstToken.column << std::endl;
        }
        
        // 测试Token流功能
        tokens.reset();
        std::cout << "Token流重置: 完成" << std::endl;
        
        int consumedCount = 0;
        while (tokens.hasNext()) {
            auto token = tokens.consume();
            consumedCount++;
            if (consumedCount > 10) break; // 限制输出
        }
        std::cout << "消费的Token数量: " << consumedCount << std::endl;
        
        // 测试测试系统
        std::cout << "\n9. 测试系统测试:" << std::endl;
        CHTL::Test::TestFramework testFramework;
        
        // 创建简单测试用例
        auto testSuite = std::make_unique<CHTL::Test::TestSuite>("BasicTest");
        
        // 添加基本测试用例
        testSuite->addTestCase(std::make_unique<CHTL::Test::TestCase>(
            "testBasicAssertion", 
            "测试基本断言功能", 
            []() {
                CHTL::Test::TestFramework::assertTrue(true, "基本断言测试");
                CHTL::Test::TestFramework::assertFalse(false, "基本断言测试");
                CHTL::Test::TestFramework::assertEqual("hello", "hello", "字符串相等测试");
            }
        ));
        
        testSuite->addTestCase(std::make_unique<CHTL::Test::TestCase>(
            "testStringOperations", 
            "测试字符串操作", 
            []() {
                std::string str = "Hello World";
                CHTL::Test::TestFramework::assertTrue(str.length() > 0, "字符串长度测试");
                CHTL::Test::TestFramework::assertEqual("Hello", str.substr(0, 5), "字符串截取测试");
            }
        ));
        
        testSuite->addTestCase(std::make_unique<CHTL::Test::TestCase>(
            "testMathOperations", 
            "测试数学运算", 
            []() {
                int a = 5;
                int b = 3;
                CHTL::Test::TestFramework::assertTrue(a + b == 8, "加法运算测试");
                CHTL::Test::TestFramework::assertTrue(a * b == 15, "乘法运算测试");
            }
        ));
        
        testFramework.addTestSuite(std::move(testSuite));
        
        // 运行测试
        std::cout << "运行基本测试套件..." << std::endl;
        testFramework.runAllTests();
        
        std::cout << "测试系统验证: 完成" << std::endl;
        
        // 清理测试文件
        std::remove("test.chtl");
        std::remove("test_output.html");
        std::remove("test_final.html");
        std::remove("test_cli.chtl");
        std::cout << "测试文件清理: 完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "编译器调度器测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "所有测试完成!" << std::endl;
    return 0;
}