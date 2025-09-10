#include <iostream>
#include <vector>
#include <memory>
#include <string>

// CHTL Compiler
#include "Scanner/UnifiedScanner.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

// CHTL JS Compiler (Stubs)
#include "CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"


int main() {
    std::string source = R"(
        div {
            p { text { "This is CHTL." } }
            script {
                // This is CHTL JS
                let a = 1;
            }
        }
    )";

    std::cout << "--- CHTL Toolchain Execution ---" << std::endl;

    try {
        // 1. Scanning
        CHTL::UnifiedScanner scanner;
        std::vector<CHTL::CodeFragment> fragments = scanner.Scan(source);

        std::string html_output;
        std::string js_output;

        // 2. Dispatching
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case CHTL::LanguageType::CHTL: {
                    CHTL::CHTLLexer lexer(fragment.content);
                    auto tokens = lexer.Tokenize();
                    CHTL::CHTLParser parser(tokens);
                    auto ast = parser.Parse();
                    if (!ast.empty()) {
                        CHTL::CHTLGenerator generator;
                        html_output += generator.Generate(ast);
                    }
                    break;
                }
                case CHTL::LanguageType::CHTL_JS: {
                    // Use the stub CHTL JS compiler
                    CHTL_JS::CHTLJSLexer lexer;
                    lexer.Tokenize(fragment.content);
                    CHTL_JS::CHTLJSParser parser;
                    parser.Parse();
                    CHTL_JS::CHTLJSGenerator generator;
                    js_output += generator.Generate();
                    break;
                }
                default:
                    // Ignore CSS and JS fragments for now
                    break;
            }
        }

        // 3. Merging
        if (!js_output.empty()) {
            // Simple injection before closing body tag if it exists
            size_t body_end_pos = html_output.rfind("</body>");
            if (body_end_pos != std::string::npos) {
                std::string script_tag = "<script>" + js_output + "</script>";
                html_output.insert(body_end_pos, script_tag);
            } else {
                html_output += "<script>" + js_output + "</script>";
            }
        }

        std::cout << "\n--- Final Output ---\n" << html_output << std::endl;
        std::cout << "\nCompilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
