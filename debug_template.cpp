#include "CHTL/CompilerDispatcher.h"
#include "CHTL/TemplateParser.h"
#include <iostream>
#include <fstream>

int main() {
    // 读取测试文件
    std::ifstream file("../test_simple_template.chtl");
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    std::cout << "Input content:" << std::endl;
    std::cout << content << std::endl;
    std::cout << "---" << std::endl;
    
    // 测试模板解析器
    CHTL::TemplateParser parser;
    auto templates = parser.parseTemplates(content);
    
    std::cout << "Found " << templates.size() << " templates:" << std::endl;
    for (const auto& template_info : templates) {
        std::cout << "Template: " << template_info.name << " (type: " << static_cast<int>(template_info.type) << ")" << std::endl;
        std::cout << "Content: " << template_info.content << std::endl;
        std::cout << "Properties: " << template_info.properties.size() << std::endl;
        for (const auto& prop : template_info.properties) {
            std::cout << "  " << prop.first << ": " << prop.second << std::endl;
        }
        std::cout << "---" << std::endl;
    }
    
    // 测试编译器
    CHTL::CompilerDispatcher dispatcher;
    auto result = dispatcher.compileCode(content);
    
    std::cout << "Compilation result:" << std::endl;
    std::cout << "Success: " << result.success << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << result.output << std::endl;
    
    return 0;
}