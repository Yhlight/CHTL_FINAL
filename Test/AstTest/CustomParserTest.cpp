#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include <iostream>
#include <cassert>
#include <algorithm>

void testCustomStyleSpecialization() {
    std::cout << "--- Running Test: testCustomStyleSpecialization ---" << std::endl;

    std::string source = R"(
        [Template] @Style BaseStyle {
            border: "1px solid black";
            padding: "10px";
        }

        [Custom] @Style MySpecialStyle {
            @Style BaseStyle;
            color: red;
            font-size, margin-top;
        }

        div {
            style {
                @Style MySpecialStyle {
                    font-size: "16px";
                    margin-top: "20px";
                    delete padding;
                    delete @Style BaseStyle;
                }
            }
        }
    )";

    try {
        // 1. Lexing & Parsing
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 2. Verification
        assert(ast != nullptr);
        CHTL::ElementNode* divNode = dynamic_cast<CHTL::ElementNode*>(ast.get());
        assert(divNode != nullptr);
        assert(divNode->tagName == "div");

        CHTL::StyleNode* styleNode = nullptr;
        for (const auto& child : divNode->children) {
            if ((styleNode = dynamic_cast<CHTL::StyleNode*>(child.get()))) {
                break;
            }
        }
        assert(styleNode != nullptr);
        std::cout << "Assertion Passed: Found StyleNode in AST." << std::endl;

        // Check for deleted properties
        assert(styleNode->deleted_properties.size() == 1);
        assert(styleNode->deleted_properties[0] == "padding");
        std::cout << "Assertion Passed: Correctly parsed 'delete property'." << std::endl;

        // Check for deleted inherited styles
        assert(styleNode->deleted_inherited_styles.size() == 1);
        assert(styleNode->deleted_inherited_styles[0] == "BaseStyle");
        std::cout << "Assertion Passed: Correctly parsed 'delete @Style'." << std::endl;

        // Check for filled-in properties
        // Note: The generator will handle the actual application/deletion.
        // The parser's job is just to populate the nodes correctly.
        // The parser will apply valued properties from the custom definition ('color') and then the filled-in properties from the usage block.
        assert(styleNode->inline_properties.size() == 3); // color + font-size + margin-top

        bool hasColor = false;
        bool hasFontSize = false;
        bool hasMarginTop = false;
        for(const auto& prop : styleNode->inline_properties) {
            if (prop.key == "color") hasColor = true;
            if (prop.key == "font-size") hasFontSize = true;
            if (prop.key == "margin-top") hasMarginTop = true;
        }
        assert(hasColor && hasFontSize && hasMarginTop);
        std::cout << "Assertion Passed: Valueless properties were correctly filled." << std::endl;


        std::cout << "--- Test Passed ---" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Test Failed with exception: " << e.what() << std::endl;
        assert(false);
    } catch (...) {
        std::cerr << "Test Failed with unknown exception." << std::endl;
        assert(false);
    }
}

int main() {
    testCustomStyleSpecialization();
    return 0;
}
