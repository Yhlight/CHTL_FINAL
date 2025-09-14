#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    chtl_context_ = std::make_shared<ParserContext>();
    // CHTL JS context can be initialized later when its parser is mature
    // chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();
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
            case ChunkType::Css: {
                css_output << chunk.content << "\n";
                break;
            }
            case ChunkType::ChtlJs: {
                // For now, treat {{...}} as a simple JS expression.
                // A more advanced CHTLJSParser would handle this.
                std::string js_expression = chunk.content;
                if (js_expression.rfind("{{", 0) == 0 && js_expression.size() > 4) {
                    js_expression = js_expression.substr(2, js_expression.size() - 4);
                }
                js_outputs.push_back(js_expression);
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
