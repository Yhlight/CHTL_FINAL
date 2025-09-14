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
    const auto& placeholder_map = unified_scanner.getPlaceholderMap();

    std::string html_output;
    std::stringstream css_output;
    std::vector<std::string> js_outputs;

    for (const auto& chunk : chunks_) {
        if (chunk.content.empty()) continue;

        switch (chunk.type) {
            case ChunkType::CHTL: {
                CHTLLoader loader;
                CHTLLexer lexer(chunk.content);
                std::vector<Token> tokens = lexer.scanTokens();
                if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) continue;

                CHTLParser parser(chunk.content, tokens, loader, "./", chtl_context_);
                std::unique_ptr<RootNode> ast = parser.parse();

                if (ast) {
                    CHTLGenerator generator;
                    CompilationResult result = generator.generate(*ast);
                    html_output += result.html;
                    if (!result.css.empty()) {
                        css_output << result.css << "\n";
                    }
                    if (!result.js.empty()) {
                        js_outputs.push_back(result.js);
                    }
                }
                break;
            }
            case ChunkType::ChtlJs: {
                CHTLJS::CHTLJSLexer lexer(chunk.content);
                std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();
                if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) continue;

                CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
                std::unique_ptr<CHTLJS::CHTLJSNode> ast = parser.parse();

                if (ast) {
                    CHTLJS::CHTLJSGenerator generator;
                    js_outputs.push_back(generator.generate(*ast));
                }
                break;
            }
            case ChunkType::Placeholder: {
                auto it = placeholder_map.find(chunk.content);
                if (it != placeholder_map.end()) {
                    js_outputs.push_back(it->second);
                }
                break;
            }
            case ChunkType::JavaScript: {
                 // For future use when scanner can identify pure JS blocks
                js_outputs.push_back(chunk.content);
                break;
            }
        }
    }

    CodeMerger merger;
    return merger.merge(html_output, css_output.str(), js_outputs);
}

} // namespace CHTL
