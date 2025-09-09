#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/Node.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <string>
#include <vector>



#include <fstream>
#include <sstream>

// A simple recursive function to print the AST for visualization
void printAst(const CHTL::Node* node, int indent = 0) {
    if (!node) return;

    std::string indentation(indent * 2, ' ');

    switch (node->getType()) {
        case CHTL::NodeType::Element: {
            auto element = static_cast<const CHTL::ElementNode*>(node);
            std::cout << indentation << "Element: <" << element->getTagName() << ">" << std::endl;

            for (const auto& attr : element->getAttributes()) {
                std::cout << indentation << "  Attribute: " << attr->getKey() << " = \"" << attr->getValue() << "\"" << std::endl;
            }

            if (element->getStyleBlock()) {
                 printAst(element->getStyleBlock(), indent + 1);
            }

            for (const auto& child : element->getChildren()) {
                printAst(child.get(), indent + 1);
            }
            break;
        }
        case CHTL::NodeType::Text: {
            auto text = static_cast<const CHTL::TextNode*>(node);
            std::cout << indentation << "Text: \"" << text->getText() << "\"" << std::endl;
            break;
        }
        case CHTL::NodeType::StyleBlock: {
            auto style = static_cast<const CHTL::StyleBlockNode*>(node);
            std::cout << indentation << "StyleBlock:" << std::endl;
            for (const auto& prop : style->getInlineProperties()) {
                printAst(prop.get(), indent + 1);
            }
            for (const auto& rule : style->getCssRules()) {
                printAst(rule.get(), indent + 1);
            }
            break;
        }
        case CHTL::NodeType::CssRule: {
            auto rule = static_cast<const CHTL::CssRuleNode*>(node);
            std::cout << indentation << "CssRule: " << rule->getSelector() << std::endl;
            for (const auto& prop : rule->getProperties()) {
                printAst(prop.get(), indent + 1);
            }
            break;
        }
        case CHTL::NodeType::CssProperty: {
            auto prop = static_cast<const CHTL::CssPropertyNode*>(node);
            std::cout << indentation << "CssProperty: " << prop->getKey() << ": " << prop->getValue() << std::endl;
            break;
        }
        default:
            std::cout << indentation << "Unknown Node" << std::endl;
            break;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Compiling: " << argv[1] << " ---\n" << source << "\n\n";

    try {
        // 1. Lexing
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.getAllTokens();

        std::cout << "--- Tokens ---" << std::endl;
        for (const auto& token : tokens) {
            if (token.type == CHTL::TokenType::EndOfFile) break;
            std::cout << "  Type: " << CHTL::TokenTypeToString(token.type)
                      << ",\tValue: '" << token.value << "'" << std::endl;
        }

        // 2. Parsing
        CHTL::Parser parser(std::move(tokens));
        std::vector<std::unique_ptr<CHTL::Node>> ast = parser.parse();

        std::cout << "\n--- Abstract Syntax Tree ---" << std::endl;
        if (!ast.empty()) {
            std::cout << "Parsing successful. AST has " << ast.size() << " root node(s)." << std::endl;
            for (const auto& node : ast) {
                printAst(node.get());
            }

            std::cout << "\n--- Generated HTML ---" << std::endl;
            CHTL::Generator generator;
            std::string html = generator.generate(ast);
            std::cout << html << std::endl;

        } else {
            std::cout << "Parsing produced an empty AST." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "\n--- ERROR ---" << std::endl;
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
