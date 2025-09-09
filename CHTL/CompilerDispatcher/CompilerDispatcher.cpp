#include "CompilerDispatcher.h"
#include "../CHTLLoader/Loader.h"
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {}

std::string CompilerDispatcher::compile(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer, m_context);
    auto program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        std::string error_messages = "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            error_messages += "\t" + error + "\n";
        }
        return error_messages;
    }

    Loader loader;
    loader.loadImports(program.get(), m_context);

    Generator generator(m_context);
    GenerationResult gen_result = generator.generate(program.get());

    CodeMerger merger;
    return merger.merge(gen_result.html, gen_result.global_css, gen_result.global_js);
}

} // namespace CHTL
