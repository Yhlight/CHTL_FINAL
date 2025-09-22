#include <iostream>
#include <string>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/Config/Configuration.h"

using namespace CHTL;

int main() {
    std::string source = R"(
use html5;

div {
    class: "box";
    text: "Hello World";
    style {
        height: 200px;
        .box {
            color: red;
        }
    }
}
)";
    
    auto config = std::make_shared<Configuration>();
    CHTLLexer lexer(source, config);
    std::vector<Token> tokens = lexer.scanTokens();
    
    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  " << token.type << " '" << token.lexeme << "'" << std::endl;
    }
    
    CHTLParser parser(source, tokens, "test.chtl", config);
    auto root = parser.parse();
    
    if (root) {
        std::cout << "Parsing successful!" << std::endl;
        
        // Check if it's an ElementNode
        if (auto* element = dynamic_cast<ElementNode*>(root.get())) {
            std::cout << "Root element: " << element->tagName << std::endl;
            std::cout << "Children count: " << element->children.size() << std::endl;
            
            for (size_t i = 0; i < element->children.size(); i++) {
                const auto& child = element->children[i];
                std::cout << "Child " << i << ": ";
                if (auto* childElement = dynamic_cast<ElementNode*>(child.get())) {
                    std::cout << "ElementNode(" << childElement->tagName << ")";
                } else if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
                    std::cout << "StyleNode";
                    std::cout << " - direct_properties: " << styleNode->direct_properties.size();
                    std::cout << " - global_rules: " << styleNode->global_rules.size();
                } else if (auto* textNode = dynamic_cast<TextNode*>(child.get())) {
                    std::cout << "TextNode(" << textNode->text << ")";
                } else {
                    std::cout << "Other";
                }
                std::cout << std::endl;
            }
        }
    } else {
        std::cout << "Parsing failed!" << std::endl;
    }
    
    return 0;
}