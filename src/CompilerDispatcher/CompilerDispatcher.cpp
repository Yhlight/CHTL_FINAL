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
    // 1. Setup components
    CHTLLoader loader;
    CHTLGenerator generator;
    CodeMerger merger;

    // 2. Lex and Parse CHTL source
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // Handle empty source
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
        return "";
    }

    CHTLParser parser(source, tokens, loader, "./", chtl_context_);
    std::unique_ptr<RootNode> ast = parser.parse();

    if (!ast) {
        // Parser should have already printed an error message.
        return "";
    }

    // 3. Generate code using the two-pass generator.
    // The generator is responsible for the entire compilation, including handling script blocks.
    CompilationResult compilation_result = generator.generate(*ast);

    // 4. Merge the final results.
    // The merger will substitute placeholders in the JS and inject the final script into the HTML.
    std::vector<std::string> js_outputs = { compilation_result.js };
    return merger.merge(compilation_result.html, js_outputs, compilation_result.placeholder_map);
}

} // namespace CHTL
