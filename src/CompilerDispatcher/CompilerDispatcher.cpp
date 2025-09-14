#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CodeMerger/CodeMerger.h"
#include <sstream>
#include <vector>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    chtl_context_ = std::make_shared<ParserContext>();
}

std::string CompilerDispatcher::compile(const std::string& source) {
    // 1. Scan the source to get code fragments
    CHTLUnifiedScanner unified_scanner(source);
    std::vector<CodeFragment> fragments = unified_scanner.scan();

    std::string chtl_source;
    std::vector<std::string> js_fragments;

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            chtl_source += fragment.content;
        } else if (fragment.type == FragmentType::JS) {
            js_fragments.push_back(fragment.content);
        }
        // CSS and CHTL_JS fragments will be handled here in the future
    }

    // 2. Compile the CHTL part
    std::string html_output;
    std::string css_output; // Future use
    std::string js_from_chtl; // JS generated from CHTL (e.g. for responsive values)

    if (!chtl_source.empty()) {
        CHTLLoader loader;
        CHTLLexer lexer(chtl_source);
        std::vector<Token> tokens = lexer.scanTokens();

        if (!tokens.empty() && !(tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
             CHTLParser parser(chtl_source, tokens, loader, "./", chtl_context_);
            std::unique_ptr<RootNode> ast = parser.parse();

            if (ast) {
                CHTLGenerator generator;
                CompilationResult compilation_result = generator.generate(*ast);
                html_output = compilation_result.html;
                // For now, CHTL generator might produce JS. We'll add it to our list.
                if (!compilation_result.js.empty()) {
                    js_fragments.insert(js_fragments.begin(), compilation_result.js);
                }
            }
        }
    }


    // 3. Merge the final results.
    // The merger will combine the HTML from CHTL compilation with all JS code.
    CodeMerger merger;
    return merger.merge(html_output, js_fragments);
}

} // namespace CHTL
