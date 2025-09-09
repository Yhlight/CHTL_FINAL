#include <iostream>
#include <string>
#include <memory>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

using namespace CHTL;

void testCompleteCHTL() {
    std::cout << "=== 完整CHTL功能测试 ===" << std::endl;
    
    // 测试复杂的CHTL代码
    std::string source = R"(
// 样式组模板
[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 16px;
    transition: all 0.3s ease;
}

[Template] @Style PrimaryButton
{
    @Style ButtonStyle;
    background-color: #007bff;
    color: white;
    
    &:hover
    {
        background-color: #0056b3;
    }
}

// 元素模板
[Template] @Element Button
{
    button
    {
        style
        {
            @Style PrimaryButton;
        }
        
        text
        {
            Click Me
        }
    }
}

// 变量组模板
[Template] @Var Theme
{
    primaryColor: "#007bff";
    secondaryColor: "#6c757d";
    successColor: "#28a745";
}

// 自定义样式组
[Custom] @Style CustomButton
{
    @Style PrimaryButton;
    border: 2px solid Theme(primaryColor);
    
    &:hover
    {
        transform: translateY(-2px);
        box-shadow: 0 4px 8px rgba(0,0,0,0.2);
    }
}

html
{
    head
    {
        title
        {
            text
            {
                CHTL Complete Test
            }
        }
    }
    
    body
    {
        style
        {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: Theme(secondaryColor);
        }
        
        div
        {
            class: "container";
            
            style
            {
                max-width: 800px;
                margin: 0 auto;
                background-color: white;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }
            
            h1
            {
                text
                {
                    CHTL Complete Test
                }
                
                style
                {
                    color: Theme(primaryColor);
                    text-align: center;
                    margin-bottom: 30px;
                }
            }
            
            div
            {
                class: "button-group";
                
                style
                {
                    text-align: center;
                    margin: 20px 0;
                }
                
                // 使用元素模板
                @Element Button;
                
                button
                {
                    style
                    {
                        @Style CustomButton;
                        margin-left: 10px;
                    }
                    
                    text
                    {
                        Custom Button
                    }
                }
            }
            
            div
            {
                class: "content";
                
                style
                {
                    margin-top: 20px;
                    padding: 15px;
                    background-color: Theme(successColor);
                    color: white;
                    border-radius: 5px;
                }
                
                text
                {
                    This demonstrates CHTL's advanced features including templates, custom styles, and theme variables.
                }
            }
        }
        
        // CHTL JS脚本
        script
        {
            {{.button-group}}->listen {
                click: () => {
                    console.log('Button clicked!');
                }
            };
            
            const anim = animate {
                target: {{.content}},
                duration: 1000,
                easing: ease-in-out,
                begin: {
                    opacity: 0
                },
                end: {
                    opacity: 1
                }
            };
        }
    }
}
)";
    
    try {
        // 词法分析
        std::cout << "1. 词法分析..." << std::endl;
        CHTLLexer lexer(source);
        TokenStream tokens = lexer.tokenize();
        
        std::cout << "   生成了 " << tokens.size() << " 个token" << std::endl;
        
        // 语法分析
        std::cout << "2. 语法分析..." << std::endl;
        CHTLParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        
        if (ast) {
            std::cout << "   AST生成成功" << std::endl;
        } else {
            std::cout << "   AST生成失败" << std::endl;
            return;
        }
        
        // 代码生成
        std::cout << "3. 代码生成..." << std::endl;
        CHTLGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   HTML输出长度: " << output.length() << " 字符" << std::endl;
        std::cout << "   CSS输出长度: " << generator.getCSSOutput().length() << " 字符" << std::endl;
        std::cout << "   JS输出长度: " << generator.getJSOutput().length() << " 字符" << std::endl;
        
        // 保存输出
        std::ofstream file("test_output.html");
        if (file.is_open()) {
            file << output;
            file.close();
            std::cout << "   输出已保存到 test_output.html" << std::endl;
        }
        
        std::cout << "完整CHTL功能测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
    }
}

void testTemplateSystem() {
    std::cout << "\n=== 模板系统测试 ===" << std::endl;
    
    // 测试样式组模板
    auto styleTemplate = std::make_shared<TemplateNode>(TemplateType::STYLE, "TestStyle");
    styleTemplate->setVariable("color", "red");
    styleTemplate->setVariable("font-size", "16px");
    
    std::cout << "样式组模板: " << styleTemplate->toCSS() << std::endl;
    
    // 测试元素模板
    auto elementTemplate = std::make_shared<TemplateNode>(TemplateType::ELEMENT, "TestElement");
    auto div = std::make_shared<ElementNode>("div");
    div->setAttribute("class", "test");
    elementTemplate->addChild(div);
    
    std::cout << "元素模板: " << elementTemplate->toHTML() << std::endl;
    
    // 测试变量组模板
    auto varTemplate = std::make_shared<TemplateNode>(TemplateType::VAR, "TestVar");
    varTemplate->setVariable("primaryColor", "#007bff");
    varTemplate->setVariable("secondaryColor", "#6c757d");
    
    std::cout << "变量组模板变量数量: " << varTemplate->getVariables().size() << std::endl;
}

void testCustomSystem() {
    std::cout << "\n=== 自定义系统测试 ===" << std::endl;
    
    // 测试自定义样式组
    auto customStyle = std::make_shared<CustomNode>(CustomType::STYLE, "CustomStyle");
    customStyle->setVariable("color", "blue");
    customStyle->setVariable("font-size", "18px");
    customStyle->deleteProperty("font-size");
    customStyle->overrideProperty("color", "green");
    
    std::cout << "自定义样式组: " << customStyle->toCSS() << std::endl;
    std::cout << "删除的属性: " << customStyle->getDeletedProperties().size() << std::endl;
    std::cout << "覆盖的属性: " << customStyle->getOverriddenProperties().size() << std::endl;
    
    // 测试自定义元素
    auto customElement = std::make_shared<CustomNode>(CustomType::ELEMENT, "CustomElement");
    auto span = std::make_shared<ElementNode>("span");
    span->setAttribute("class", "custom");
    customElement->addChild(span);
    
    std::cout << "自定义元素: " << customElement->toHTML() << std::endl;
}

void testStyleSystem() {
    std::cout << "\n=== 样式系统测试 ===" << std::endl;
    
    // 测试内联样式
    auto inlineStyle = std::make_shared<StyleNode>(StyleType::INLINE);
    inlineStyle->setProperty("color", "red");
    inlineStyle->setProperty("font-size", "16px");
    
    std::cout << "内联样式: " << inlineStyle->toHTML() << std::endl;
    
    // 测试类选择器
    auto classStyle = std::make_shared<StyleNode>(StyleType::CLASS, ".test");
    classStyle->setProperty("background-color", "yellow");
    classStyle->setProperty("padding", "10px");
    
    std::cout << "类选择器样式: " << classStyle->toCSS() << std::endl;
    
    // 测试属性条件表达式
    auto conditionalStyle = std::make_shared<StyleNode>(StyleType::CONDITIONAL);
    conditionalStyle->setConditionalExpression("width > 50px ? 'red' : 'blue'");
    conditionalStyle->setProperty("background-color", "width > 50px ? 'red' : 'blue'");
    
    std::cout << "条件表达式: " << conditionalStyle->getConditionalExpression() << std::endl;
}

void testScriptSystem() {
    std::cout << "\n=== 脚本系统测试 ===" << std::endl;
    
    // 测试CHTL JS脚本
    auto chtlScript = std::make_shared<ScriptNode>(ScriptType::CHTL_JS);
    chtlScript->setScriptContent("{{button}}->addEventListener('click', () => { console.log('clicked'); });");
    
    std::cout << "CHTL JS脚本: " << chtlScript->toJS() << std::endl;
    
    // 测试文件加载器
    auto fileLoader = std::make_shared<ScriptNode>(ScriptType::FILELOADER);
    fileLoader->addFileLoaderPath("./module1.cjjs");
    fileLoader->addFileLoaderPath("./module2.cjjs");
    fileLoader->addFileLoaderPath("./style.css");
    
    std::cout << "文件加载器: " << fileLoader->toJS() << std::endl;
    
    // 测试增强监听器
    auto listener = std::make_shared<ScriptNode>(ScriptType::LISTEN);
    listener->addEventListener("click", "() => console.log('clicked')");
    listener->addEventListener("mouseenter", "() => console.log('entered')");
    
    std::cout << "增强监听器: " << listener->toJS() << std::endl;
}

int main() {
    std::cout << "CHTL 完整功能测试套件" << std::endl;
    std::cout << "=====================" << std::endl;
    
    try {
        testCompleteCHTL();
        testTemplateSystem();
        testCustomSystem();
        testStyleSystem();
        testScriptSystem();
        
        std::cout << "\n所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试套件失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}