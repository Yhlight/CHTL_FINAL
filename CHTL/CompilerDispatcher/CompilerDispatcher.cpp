#include "CompilerDispatcher.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLNode/ImportNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <algorithm>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

FinalOutput CompilerDispatcher::dispatch() {
    // This is a simplified dispatcher that doesn't handle imports for now.
    // It focuses on correctly using the scanner.

    std::cout << "--- Parsing for templates: <main> ---" << std::endl;
    CHTLUnifiedScanner scanner(this->initial_source);
    std::vector<CodeFragment> fragments = scanner.scan();

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            if (fragment.content.find_first_not_of(" \t\n\r") == std::string::npos) {
                continue;
            }
            CHTLLexer lexer(fragment.content);
            auto tokens = lexer.scanTokens();
            CHTLParser parser(fragment.content, tokens, this->all_template_definitions);
            parser.parse();
        }
    }

    std::cout << "--- Generating final output ---" << std::endl;
    FinalOutput final_output;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            if (fragment.content.find_first_not_of(" \t\n\r") == std::string::npos) {
                continue;
            }
            CHTLLexer lexer(fragment.content);
            std::vector<Token> tokens = lexer.scanTokens();
            CHTLParser parser(fragment.content, tokens, this->all_template_definitions);
            auto ast = parser.parse();
            CHTLGenerator generator(this->all_template_definitions);
            auto output = generator.generate(ast.get());

            final_output.html += output.html;
            final_output.css += output.css;
            final_output.js += output.js;
        } else if (fragment.type == FragmentType::HTML) {
            final_output.html += fragment.content;
        } else if (fragment.type == FragmentType::CSS) {
            final_output.css += fragment.content;
        } else if (fragment.type == FragmentType::JS) {
            final_output.js += fragment.content;
        }
    }

    return final_output;
}

} // namespace CHTL
