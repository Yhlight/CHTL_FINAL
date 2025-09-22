#include <iostream>
#include <string>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/Config/Configuration.h"

using namespace CHTL;

int main() {
    std::string source = R"(
use html5;

html {
    head {
        title { text: "Full Gen Test"; }
    }
    body {
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
    }
}
)";
    
    auto config = std::make_shared<Configuration>();
    CHTLLexer lexer(source, config);
    std::vector<Token> tokens = lexer.scanTokens();
    
    CHTLParser parser(source, tokens, "test.chtl", config);
    auto root = parser.parse();
    
    if (root) {
        std::cout << "Parsing successful!" << std::endl;
        
        // Check if it's an ElementNode
        if (auto* element = dynamic_cast<ElementNode*>(root.get())) {
            std::cout << "Root element: " << element->tagName << std::endl;
            
            // Find the div element
            for (const auto& child : element->children) {
                if (auto* body = dynamic_cast<ElementNode*>(child.get())) {
                    if (body->tagName == "body") {
                        for (const auto& bodyChild : body->children) {
                            if (auto* div = dynamic_cast<ElementNode*>(bodyChild.get())) {
                                if (div->tagName == "div") {
                                    std::cout << "Found div element" << std::endl;
                                    
                                    // Check for style node
                                    for (const auto& divChild : div->children) {
                                        if (auto* style = dynamic_cast<StyleNode*>(divChild.get())) {
                                            std::cout << "Found style node" << std::endl;
                                            std::cout << "Direct properties: " << style->direct_properties.size() << std::endl;
                                            std::cout << "Global rules: " << style->global_rules.size() << std::endl;
                                            
                                            for (const auto& rule : style->global_rules) {
                                                std::cout << "Rule selector: '" << rule.selector << "'" << std::endl;
                                                std::cout << "Rule properties: " << rule.properties.size() << std::endl;
                                                for (const auto& prop : rule.properties) {
                                                    std::cout << "  Property: " << prop.key << std::endl;
                                                    if (prop.value_expr) {
                                                        std::cout << "    Has value expression" << std::endl;
                                                    } else {
                                                        std::cout << "    No value expression" << std::endl;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        std::cout << "Parsing failed!" << std::endl;
    }
    
    return 0;
}