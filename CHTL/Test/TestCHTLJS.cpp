#include <iostream>
#include <string>
#include <memory>
#include "CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL JS/CHTLJSLexer/CHTLJSToken.h"
#include "CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"

using namespace CHTL;

void testCHTLJSLexer() {
    std::cout << "=== CHTL JS 词法分析器测试 ===" << std::endl;
    
    std::string source = R"(
fileloader {
    load: "./module1.cjjs",
    load: "./module2.cjjs",
    load: "./style.css"
}

{{button}}->listen {
    click: () => {
        console.log('Button clicked!');
    },
    mouseenter: () => {
        console.log('Mouse entered');
    }
}

{{.container}}->delegate {
    target: {{.button}},
    click: (e) => {
        console.log('Delegated click');
    }
}

const anim = animate {
    target: {{.box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
}

vir DemoAPI = {
    showMessage: (msg) => {
        console.log(msg);
    },
    changeTheme: (color) => {
        document.body->style->backgroundColor = color;
    }
}

router {
    "/": {{.home}},
    "/about": {{.about}},
    "/contact": {{.contact}}
}

util document->readyState === 'complete' -> change {
    console.log('Document loaded');
} -> then {
    anim->play();
}
)";
    
    try {
        CHTLJSLexer lexer(source);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        
        std::cout << "生成了 " << tokens.size() << " 个token" << std::endl;
        
        // 显示前10个token
        for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
            std::cout << "  " << tokens[i].toString() << std::endl;
        }
        
        if (tokens.size() > 10) {
            std::cout << "  ... 还有 " << (tokens.size() - 10) << " 个token" << std::endl;
        }
        
        std::cout << "CHTL JS 词法分析器测试通过！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS 词法分析器测试失败: " << e.what() << std::endl;
    }
}

void testCHTLJSParser() {
    std::cout << "\n=== CHTL JS 语法分析器测试 ===" << std::endl;
    
    std::string source = R"(
fileloader {
    load: "./module.cjjs"
}

{{button}}->listen {
    click: () => {
        console.log('clicked');
    }
}
)";
    
    try {
        CHTLJSLexer lexer(source);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        
        if (ast) {
            std::cout << "AST生成成功" << std::endl;
            std::cout << "AST节点数量: " << ast->children.size() << std::endl;
        } else {
            std::cout << "AST生成失败" << std::endl;
            return;
        }
        
        std::cout << "CHTL JS 语法分析器测试通过！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS 语法分析器测试失败: " << e.what() << std::endl;
    }
}

void testCHTLJSGenerator() {
    std::cout << "\n=== CHTL JS 代码生成器测试 ===" << std::endl;
    
    std::string source = R"(
fileloader {
    load: "./module1.cjjs",
    load: "./module2.cjjs"
}

{{button}}->listen {
    click: () => {
        console.log('Button clicked!');
    }
}

const anim = animate {
    target: {{.box}},
    duration: 1000,
    easing: ease-in-out
}
)";
    
    try {
        CHTLJSLexer lexer(source);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "生成的JavaScript代码:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << output << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "代码长度: " << output.length() << " 字符" << std::endl;
        
        std::cout << "CHTL JS 代码生成器测试通过！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS 代码生成器测试失败: " << e.what() << std::endl;
    }
}

void testCHTLJSFeatures() {
    std::cout << "\n=== CHTL JS 特性测试 ===" << std::endl;
    
    // 测试文件加载器
    std::cout << "1. 文件加载器测试..." << std::endl;
    std::string fileLoaderSource = R"(
fileloader {
    load: "./module1.cjjs",
    load: "./module2.cjjs",
    load: "./style.css"
}
)";
    
    try {
        CHTLJSLexer lexer(fileLoaderSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   文件加载器代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   文件加载器测试失败: " << e.what() << std::endl;
    }
    
    // 测试增强监听器
    std::cout << "2. 增强监听器测试..." << std::endl;
    std::string listenSource = R"(
{{button}}->listen {
    click: () => {
        console.log('clicked');
    },
    mouseenter: () => {
        console.log('entered');
    }
}
)";
    
    try {
        CHTLJSLexer lexer(listenSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   增强监听器代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   增强监听器测试失败: " << e.what() << std::endl;
    }
    
    // 测试事件委托
    std::cout << "3. 事件委托测试..." << std::endl;
    std::string delegateSource = R"(
{{.container}}->delegate {
    target: {{.button}},
    click: (e) => {
        console.log('delegated click');
    }
}
)";
    
    try {
        CHTLJSLexer lexer(delegateSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   事件委托代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   事件委托测试失败: " << e.what() << std::endl;
    }
    
    // 测试动画
    std::cout << "4. 动画测试..." << std::endl;
    std::string animateSource = R"(
const anim = animate {
    target: {{.box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
}
)";
    
    try {
        CHTLJSLexer lexer(animateSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   动画代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   动画测试失败: " << e.what() << std::endl;
    }
    
    // 测试虚对象
    std::cout << "5. 虚对象测试..." << std::endl;
    std::string virSource = R"(
vir DemoAPI = {
    showMessage: (msg) => {
        console.log(msg);
    },
    changeTheme: (color) => {
        document.body->style->backgroundColor = color;
    }
}
)";
    
    try {
        CHTLJSLexer lexer(virSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   虚对象代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   虚对象测试失败: " << e.what() << std::endl;
    }
    
    // 测试路由
    std::cout << "6. 路由测试..." << std::endl;
    std::string routerSource = R"(
router {
    "/": {{.home}},
    "/about": {{.about}},
    "/contact": {{.contact}}
}
)";
    
    try {
        CHTLJSLexer lexer(routerSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   路由代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   路由测试失败: " << e.what() << std::endl;
    }
    
    // 测试util表达式
    std::cout << "7. util表达式测试..." << std::endl;
    std::string utilSource = R"(
util document->readyState === 'complete' -> change {
    console.log('Document loaded');
} -> then {
    anim->play();
}
)";
    
    try {
        CHTLJSLexer lexer(utilSource);
        std::vector<CHTLJSToken> tokens = lexer.tokenize();
        CHTLJSParser parser(tokens);
        std::shared_ptr<BaseNode> ast = parser.parse();
        CHTLJSGenerator generator;
        std::string output = generator.generate(ast);
        
        std::cout << "   util表达式代码生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "   util表达式测试失败: " << e.what() << std::endl;
    }
    
    std::cout << "CHTL JS 特性测试完成！" << std::endl;
}

int main() {
    std::cout << "CHTL JS 编译器测试套件" << std::endl;
    std::cout << "=====================" << std::endl;
    
    try {
        testCHTLJSLexer();
        testCHTLJSParser();
        testCHTLJSGenerator();
        testCHTLJSFeatures();
        
        std::cout << "\n所有CHTL JS测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS测试套件失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}