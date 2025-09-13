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

    // Process style_chunks
    std::vector<std::string> css_outputs;
    for (const auto& chunk : style_chunks) {
        css_outputs.push_back(chunk.content);
    }

    // Process script_chunks
    for (const auto& chunk : script_chunks) {
        // We process the ChtlJs chunk which contains placeholders.
        if (chunk.type == ChunkType::ChtlJs) {
            CHTLJS::CHTLJSLexer lexer(chunk.content);
            std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();
            if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) continue;

            CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
            std::unique_ptr<CHTLJS::CHTLJSNode> ast = parser.parse();

            if (ast) {
                CHTLJS::CHTLJSGenerator generator;
                js_outputs.push_back(generator.generate(*ast));
            }
        }
    }

    // Decode placeholders in the generated JavaScript
    const auto& placeholder_map = unified_scanner.getPlaceholderMap();
    for (std::string& js_output : js_outputs) {
        for (const auto& pair : placeholder_map) {
            size_t pos = js_output.find(pair.first);
            while (pos != std::string::npos) {
                js_output.replace(pos, pair.first.length(), pair.second);
                pos = js_output.find(pair.first, pos + pair.second.length());
            }
        }
    }

    CodeMerger merger;
    return merger.merge(html_output, css_outputs, js_outputs);
}

} // namespace CHTL
