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

// Helper function to replace all occurrences of a substring
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string CompilerDispatcher::compile(const std::string& source) {
    // Step 1: Scan the source code
    CHTLUnifiedScanner unified_scanner(source);
    chunks_ = unified_scanner.scan();
    auto placeholder_map = unified_scanner.getPlaceholderMap();

    // Step 2: Separate chunks by type
    std::string unified_chtl_content;
    std::string unified_css_content;
    std::vector<std::string> chtl_js_chunks;

    for (const auto& chunk : chunks_) {
        if (chunk.content.empty()) continue;
        switch (chunk.type) {
            case ChunkType::CHTL:
                unified_chtl_content += chunk.content;
                break;
            case ChunkType::Css:
                unified_css_content += chunk.content;
                break;
            case ChunkType::ChtlJs:
                chtl_js_chunks.push_back(chunk.content);
                break;
            default: break;
        }
    }

    // Step 3: Process the unified CHTL content
    std::string html_output;
    std::vector<std::string> js_from_chtl;
    if (!unified_chtl_content.empty()) {
        CHTLLoader loader;
        CHTLLexer lexer(unified_chtl_content);
        std::vector<Token> tokens = lexer.scanTokens();
        if (!tokens.empty() && !(tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
            CHTLParser parser(unified_chtl_content, tokens, loader, "./", chtl_context_);
            std::unique_ptr<RootNode> ast = parser.parse();
            CHTLGenerator generator;
            CompilationResult result = generator.generate(*ast);
            html_output = result.html;
            if (!result.js.empty()) {
                js_from_chtl.push_back(result.js);
            }
        }
    }

    // Step 4: Process the CHTL-JS chunks
    std::vector<std::string> js_from_chtljs;
    for (const auto& chunk_content : chtl_js_chunks) {
        CHTLJS::CHTLJSLexer lexer(chunk_content);
        std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();
        if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) continue;

        CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
        std::unique_ptr<CHTLJS::CHTLJSNode> ast = parser.parse();

        if (ast) {
            CHTLJS::CHTLJSGenerator generator;
            std::string generated_js = generator.generate(*ast);
            for (const auto& pair : placeholder_map) {
                replaceAll(generated_js, pair.first, pair.second);
            }
            js_from_chtljs.push_back(generated_js);
        }
    }

    // Step 5: Merge all outputs
    CodeMerger merger;
    js_from_chtl.insert(js_from_chtl.end(), js_from_chtljs.begin(), js_from_chtljs.end());
    std::string final_html = merger.merge(html_output, js_from_chtl);
    if (!unified_css_content.empty()) {
        final_html += "\n<style>\n" + unified_css_content + "\n</style>";
    }

    return final_html;
}

} // namespace CHTL
