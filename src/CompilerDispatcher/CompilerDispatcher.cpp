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

#include "Scanner/CHTLUnifiedScanner.h"

std::string CompilerDispatcher::compile(const std::string& source) {
    // Step 1: Use the Unified Scanner to separate CHTL from other languages.
    // The scanner replaces script blocks with placeholders.
    CHTLUnifiedScanner scanner(source);
    ScanningResult scan_result = scanner.scan();

    // Step 2: Compile the CHTL-only source code.
    // This source now contains placeholders instead of raw script content.
    CHTLLoader loader;
    CHTLGenerator generator;
    CodeMerger merger;

    CHTLLexer lexer(scan_result.chtl_with_placeholders);
    std::vector<Token> tokens = lexer.scanTokens();

    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
        return "";
    }

    CHTLParser parser(scan_result.chtl_with_placeholders, tokens, loader, "./", chtl_context_);
    std::unique_ptr<RootNode> ast = parser.parse();

    if (!ast) {
        return ""; // Parser errors are handled internally.
    }

    // The CHTL generator will now generate HTML and will encounter the placeholders
    // that the CHTL parser has stored in ScriptBlockNodes. The generator will
    // pass these placeholders along in its own placeholder map.
    CompilationResult chtl_compilation_result = generator.generate(*ast);

    // Step 3: Process the script content from the scanner's placeholder map.
    // For now, we are not compiling the JS/CHTL-JS. We are just collecting the
    // raw content from the scanner's map.
    // In the future, this is where we would dispatch to CHTLJSParser.
    std::map<std::string, std::string> processed_scripts;
    for (const auto& pair : scan_result.placeholder_map) {
        // The key is the placeholder (e.g., __CHTL_SCRIPT_PLACEHOLDER_1__)
        // The value is the CodeFragment containing the raw JS.
        // For now, we just pass the raw JS to be merged.
        processed_scripts[pair.first] = pair.second.content;
    }


    // Step 4: Merge the final results.
    // The merger will take the HTML from the CHTL generator and substitute the
    // script placeholders with the (currently raw, eventually compiled) JS.
    return merger.merge(chtl_compilation_result.html, processed_scripts);
}

} // namespace CHTL
