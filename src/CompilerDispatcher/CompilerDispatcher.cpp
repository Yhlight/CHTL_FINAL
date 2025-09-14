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
    // Initialize contexts
    chtl_context_ = std::make_shared<ParserContext>();
    chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();

    // Initialize components
    chtl_loader_ = std::make_unique<CHTLLoader>();
    // CHTLParser needs to be re-created per-chunk because it takes chunk-specific data in constructor
    chtl_generator_ = std::make_unique<CHTLGenerator>();
    // CHTLJSParser also needs to be re-created per-chunk
    chtljs_generator_ = std::make_unique<CHTLJS::CHTLJSGenerator>();
    code_merger_ = std::make_unique<CodeMerger>();
}

// Destructor must be defined here where the full types are known
CompilerDispatcher::~CompilerDispatcher() = default;

namespace {
    std::string restore_placeholders(std::string generated_js, const std::map<std::string, std::string>& placeholder_map) {
        for (const auto& pair : placeholder_map) {
            size_t pos = generated_js.find(pair.first);
            while (pos != std::string::npos) {
                generated_js.replace(pos, pair.first.length(), pair.second);
                pos = generated_js.find(pair.first, pos + pair.second.length());
            }
        }
        return generated_js;
    }
}

std::string CompilerDispatcher::compile(const std::string& source) {
    CHTLUnifiedScanner unified_scanner(source);
    std::vector<CodeChunk> chunks = unified_scanner.scan();
    auto placeholder_map = unified_scanner.getPlaceholderMap();

    std::string html_output;
    std::string css_output;
    std::vector<std::string> js_outputs;

    for (const auto& chunk : chunks) {
        if (chunk.content.empty()) continue;

        if (chunk.type == ChunkType::CHTL) {
            CHTLLexer lexer(chunk.content);
            auto tokens = lexer.scanTokens();
            if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) continue;

            CHTLParser parser(chunk.content, tokens, *chtl_loader_, "./", chtl_context_);
            std::unique_ptr<RootNode> ast = parser.parse();

            CompilationResult result = chtl_generator_->generate(*ast);
            html_output += result.html;
            if (!result.js.empty()) {
                js_outputs.push_back(result.js);
            }

        } else if (chunk.type == ChunkType::ChtlJs) {
            CHTLJS::CHTLJSLexer lexer(chunk.content);
            auto tokens = lexer.scanTokens();
            if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) continue;

            CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
            std::unique_ptr<CHTLJS::CHTLJSNode> ast = parser.parse();

            if (ast) {
                std::string js_with_placeholders = chtljs_generator_->generate(*ast);
                std::string final_js = restore_placeholders(js_with_placeholders, placeholder_map);
                js_outputs.push_back(final_js);
            }
        } else if (chunk.type == ChunkType::Css) {
            css_output += chunk.content;
        }
    }

    return code_merger_->merge(html_output, css_output, js_outputs);
}

} // namespace CHTL
