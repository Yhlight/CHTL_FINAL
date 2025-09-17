#include "CompilerDispatcher.h"
#include "../CodeMerger/CodeMerger.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {}

std::string CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
    CodeMerger merger;
    Generator generator; // Create one generator and reuse it.

    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::CHTL) {
            // Instantiate the CHTL compiler pipeline
            Lexer lexer(fragment.content);
            Parser parser(lexer);
            std::unique_ptr<RootNode> ast = parser.parseProgram();

            // Check for parsing errors
            if (!parser.getErrors().empty()) {
                std::cerr << "Parsing failed with " << parser.getErrors().size() << " errors.\n";
                // For now, we'll continue and try to generate what we can.
                // A real compiler might stop here.
            }

            // Generate HTML from the AST
            std::string html_output = generator.generate(ast.get());
            merger.addHtml(html_output);

        } else if (fragment.type == CodeType::CSS) {
            // In a future phase, this would go to a CSS compiler/processor.
            // For now, we can just merge it directly.
            merger.addCss(fragment.content);
        } else if (fragment.type == CodeType::JS) {
            // To be handled by the JS compiler.
            merger.addJs(fragment.content);
        }
        // CHTL_JS will also be handled in a future phase.
    }

    // The merger will combine the pieces into the final output.
    return merger.getFinalOutput();
}

} // namespace CHTL
