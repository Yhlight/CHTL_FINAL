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
        
    } catch (const std::exception& e) {
        std::cerr << "CJMOD API测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "所有测试完成!" << std::endl;
    return 0;
}