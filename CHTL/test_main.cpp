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
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "测试完成!" << std::endl;
    return 0;
}