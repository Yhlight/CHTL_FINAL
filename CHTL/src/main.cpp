#include <iostream>
#include <fstream>
#include <string>
#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLParser/Parser.hpp>

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return 1;
    }
    
    // 读取文件内容
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 创建词法分析器
    Lexer lexer(source);
    
    // 创建语法分析器
    Parser parser(lexer);
    
    // 语法分析
    auto ast = parser.parse();
    
    // 输出结果
    std::cout << "=== CHTL 语法分析结果 ===" << std::endl;
    std::cout << "文件: " << filename << std::endl;
    
    if (parser.hasError()) {
        std::cout << "错误: " << parser.getError() << std::endl;
        return 1;
    }
    
    std::cout << "AST 根节点: " << ast->toString() << std::endl;
    std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
    std::cout << std::endl;
    
    // 输出 HTML
    std::cout << "=== 生成的 HTML ===" << std::endl;
    std::cout << ast->toHTML() << std::endl;
    
    return 0;
}