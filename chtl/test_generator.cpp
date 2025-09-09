#include <iostream>
#include "generator/code_generator.h"
#include "ast/ast_node.h"
#include "style/style_processor.h"
#include "import/import_manager.h"

using namespace chtl::generator;
using namespace chtl::ast;

int main() {
    std::cout << "Testing CodeGenerator..." << std::endl;
    
    try {
        // 创建依赖
        auto style_processor = std::make_shared<chtl::style::StyleProcessor>();
        auto import_manager = std::make_shared<chtl::import::ImportManager>("/workspace/chtl");
        
        // 创建代码生成器
        GenerationOptions options;
        options.minify_html = false;
        options.minify_css = false;
        options.minify_js = false;
        options.include_comments = true;
        
        CodeGenerator generator(style_processor, import_manager, options);
        
        // 创建测试AST
        auto root = std::make_shared<ASTNode>(NodeType::ROOT);
        
        auto html = std::make_shared<ElementNode>("html");
        root->add_child(html);
        
        auto head = std::make_shared<ElementNode>("head");
        html->add_child(head);
        
        auto title = std::make_shared<ElementNode>("title");
        title->set_attribute("text", "My CHTL Page");
        head->add_child(title);
        
        auto body = std::make_shared<ElementNode>("body");
        html->add_child(body);
        
        auto div = std::make_shared<ElementNode>("div");
        div->set_attribute("class", "container");
        div->set_attribute("id", "main");
        body->add_child(div);
        
        auto text = std::make_shared<TextNode>("Hello, CHTL!");
        div->add_child(text);
        
        auto style = std::make_shared<StyleNode>(".container { width: 100%; height: 100vh; background-color: blue; }");
        div->add_child(style);
        
        // 生成代码
        std::string html_output = generator.generate_html(root);
        std::string css_output = generator.generate_css(root);
        std::string js_output = generator.generate_js(root);
        
        std::cout << "=== Generated HTML ===" << std::endl;
        std::cout << html_output << std::endl;
        
        std::cout << "=== Generated CSS ===" << std::endl;
        std::cout << css_output << std::endl;
        
        std::cout << "=== Generated JS ===" << std::endl;
        std::cout << js_output << std::endl;
        
        // 生成完整文档
        std::string document = generator.generate_document(root);
        std::cout << "=== Complete Document ===" << std::endl;
        std::cout << document << std::endl;
        
        // 打印生成信息
        generator.print_generation_info();
        
        std::cout << "CodeGenerator test completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}