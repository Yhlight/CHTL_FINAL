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
            std::cout << indentation << "CssProperty: " << prop->getKey() << ":" << std::endl;
            printAst(prop->getValue(), indent + 1);
            break;
        }
        case CHTL::NodeType::Literal: {
            auto literal = static_cast<const CHTL::LiteralNode*>(node);
            std::cout << indentation << "Literal: " << literal->getValue() << std::endl;
            break;
        }
        case CHTL::NodeType::BinaryOperation: {
            auto binOp = static_cast<const CHTL::BinaryOperationNode*>(node);
            std::cout << indentation << "BinaryOp: " << binOp->getOperator().value << std::endl;
            printAst(binOp->getLeft(), indent + 1);
            printAst(binOp->getRight(), indent + 1);
            break;
        }
        case CHTL::NodeType::TernaryOperation: {
            auto ternOp = static_cast<const CHTL::TernaryOperationNode*>(node);
            std::cout << indentation << "TernaryOp:" << std::endl;
            std::cout << indentation << "  Condition:" << std::endl;
            printAst(ternOp->getCondition(), indent + 2);
            std::cout << indentation << "  True:" << std::endl;
            printAst(ternOp->getTrueValue(), indent + 2);
            std::cout << indentation << "  False:" << std::endl;
            printAst(ternOp->getFalseValue(), indent + 2);
            break;
        }
        default:
            std::cout << indentation << "Unknown Node Type" << std::endl;
            break;
        case CHTL::NodeType::PropertyAccess: {
            auto propAccess = static_cast<const CHTL::PropertyAccessNode*>(node);
            std::cout << indentation << "PropertyAccess: " << propAccess->getSelector() << "." << propAccess->getPropertyName() << std::endl;
            break;
        }
    }
}

bool runTest(const char* filename) {
    std::cout << "====================================================\n";
    std::cout << "--- Running Test: " << filename << " ---\n";
    std::cout << "====================================================\n";

    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string source = buffer.str();

    try {
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.getAllTokens();
        CHTL::Parser parser(std::move(tokens));
        std::vector<std::unique_ptr<CHTL::Node>> ast = parser.parse();

        // For now, a successful test is one that parses without throwing an exception.
        // The generator is still incomplete for expressions.
        std::cout << "Parsing successful for " << filename << std::endl;

        // We can still try to generate to see what happens
        CHTL::Generator generator;
        std::string html = generator.generate(ast);
        std::cout << "\n--- Generated HTML for " << filename << " ---\n";
        std::cout << html << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "\n--- ERROR in " << filename << " ---" << std::endl;
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <test_file1.chtl> <test_file2.chtl> ..." << std::endl;
        return 1;
    }

    int failed_tests = 0;
    for (int i = 1; i < argc; ++i) {
        if (!runTest(argv[i])) {
            failed_tests++;
        }
    }

    if (failed_tests > 0) {
        std::cerr << "\n" << failed_tests << " test(s) failed." << std::endl;
        return 1;
    }

    std::cout << "\nAll tests passed successfully!" << std::endl;
    return 0;
}
