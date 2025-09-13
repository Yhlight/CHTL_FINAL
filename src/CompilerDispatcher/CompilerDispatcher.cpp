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
    CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    const auto& placeholder_map = scanner.getPlaceholderMap();

    std::string html_output;
    std::vector<std::string> css_outputs;
    std::vector<std::string> js_outputs;

    // First pass for CHTL and raw CSS/JS
    for (const auto& chunk : chunks) {
        if (chunk.content.empty()) continue;

        switch (chunk.type) {
            case ChunkType::CHTL: {
                CHTLLoader loader;
                CHTLLexer lexer(chunk.content);
                std::vector<Token> tokens = lexer.scanTokens();
                if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) continue;

                CHTLParser parser(chunk.content, tokens, loader, "./", chtl_context_);
                std::unique_ptr<RootNode> ast = parser.parse();

                CHTLGenerator generator;
                CompilationResult result = generator.generate(*ast);
                html_output += result.html;
                if (!result.js.empty()) {
                    js_outputs.push_back(result.js);
                }
                break;
            }
            case ChunkType::Css: {
                css_outputs.push_back(chunk.content);
                break;
            }
            case ChunkType::JavaScript: {
                js_outputs.push_back(chunk.content);
                break;
            }
            default:
                // ChtlJs and Placeholder chunks will be handled by the CHTLJSGenerator
                break;
        }
    }

    // Second pass for CHTL-JS, which needs the full context of placeholders
    CHTLJS::CHTLJSGenerator chtljs_generator;
    std::string chtljs_output = chtljs_generator.generate(chunks, placeholder_map, chtljs_context_);
    if (!chtljs_output.empty()) {
        js_outputs.push_back(chtljs_output);
    }

    CodeMerger merger;
    return merger.merge(html_output, css_outputs, js_outputs);
}

} // namespace CHTL
