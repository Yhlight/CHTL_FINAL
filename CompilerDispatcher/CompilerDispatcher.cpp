#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTL/CHTLAnalyzer/Analyzer.h" // New include
#include <iostream>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {}

std::string CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
    Generator generator;
    Analyzer analyzer; // Create an analyzer
    std::stringstream body_stream;

    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::CHTL) {
            // 1. Lex & Parse
            Lexer lexer(fragment.content);
            Parser parser(lexer);
            std::unique_ptr<RootNode> ast = parser.parseProgram();

            if (!parser.getErrors().empty()) {
                std::cerr << "Parsing failed with " << parser.getErrors().size() << " errors.\n";
                for(const auto& err : parser.getErrors()) {
                    std::cerr << "  - " << err << "\n";
                }
            }

            // 2. Analyze and enrich the AST
            analyzer.analyze(ast.get());

            // 3. Generate code from the (potentially modified) AST
            body_stream << generator.generate(ast.get());
        }
    }

    // Assemble the final document
    std::stringstream final_doc;
    final_doc << "<!DOCTYPE html>\n";
    final_doc << "<html>\n";
    final_doc << "<head>\n";
    final_doc << "  <meta charset=\"UTF-8\">\n";
    final_doc << "  <title>CHTL Output</title>\n";

    std::string global_css = generator.getGlobalCss();
    if (!global_css.empty()) {
        final_doc << "  <style>\n";
        final_doc << global_css;
        final_doc << "  </style>\n";
    }

    final_doc << "</head>\n";
    final_doc << "<body>\n";
    final_doc << body_stream.str();
    final_doc << "</body>\n";
    final_doc << "</html>\n";

    return final_doc.str();
}

} // namespace CHTL
