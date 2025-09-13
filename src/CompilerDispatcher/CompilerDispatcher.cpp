#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    chtl_context_ = std::make_shared<ParserContext>();
    chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();
}

std::string CompilerDispatcher::compile(const std::string& source) {
    CHTLUnifiedScanner unified_scanner(source);
    chunks_ = unified_scanner.scan();

    std::stringstream pure_chtl_stream;
    std::vector<CodeChunk> script_chunks;
    std::vector<CodeChunk> style_chunks;

    // Separate chunks by type
    for (const auto& chunk : chunks_) {
        if (chunk.content.empty()) continue;

        switch(chunk.type) {
            case ChunkType::CHTL:
                pure_chtl_stream << chunk.content;
                break;
            case ChunkType::ChtlJs:
            case ChunkType::JavaScript:
                script_chunks.push_back(chunk);
                break;
            case ChunkType::Css:
                style_chunks.push_back(chunk);
                break;
            default:
                break;
        }
    }

    std::string pure_chtl_source = pure_chtl_stream.str();
    std::string html_output;
    std::vector<std::string> js_outputs;

    // Process the aggregated CHTL source
    if (!pure_chtl_source.empty()) {
        CHTLLoader loader;
        CHTLLexer lexer(pure_chtl_source);
        std::vector<Token> tokens = lexer.scanTokens();
        if (!tokens.empty() && !(tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
             CHTLParser parser(pure_chtl_source, tokens, loader, "./", chtl_context_);
             std::unique_ptr<RootNode> ast = parser.parse();

             CHTLGenerator generator;
             CompilationResult result = generator.generate(*ast);
             html_output += result.html;
             if (!result.js.empty()) {
                 js_outputs.push_back(result.js);
             }
        }
    }

    // TODO: Process script_chunks and style_chunks
    // For now, we will ignore them to get the CHTL part working.

    CodeMerger merger;
    return merger.merge(html_output, js_outputs);
}

} // namespace CHTL
