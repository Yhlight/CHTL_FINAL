#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include <sstream>
#include <vector>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    chtl_context_ = std::make_shared<ParserContext>();
    chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();
}

std::string CompilerDispatcher::compile(const std::string& source) {
    // 1. Lex and Parse the raw source.
    // The parser now knows how to handle script blocks correctly.
    CHTLLoader loader;
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
        return "";
    }

    CHTLParser parser(source, tokens, loader, "./", chtl_context_);
    std::unique_ptr<RootNode> ast = parser.parse();

    if (!ast) {
        // Parser failed and should have printed an error.
        return "";
    }

    // 2. Generate the output from the AST.
    // The generator will invoke the scanner on script content.
    CHTLGenerator generator;
    CompilationResult compilation_result = generator.generate(*ast);

    // 3. Merge the final results.
    // The merger will substitute the placeholders.
    CodeMerger merger;
    std::vector<std::string> js_outputs = { compilation_result.js };
    // The generator doesn't produce separate CSS yet. This is handled by the generator internally for now.
    std::vector<std::string> css_outputs = {};

    return merger.merge(compilation_result.html, js_outputs, compilation_result.placeholder_map);
}

} // namespace CHTL
