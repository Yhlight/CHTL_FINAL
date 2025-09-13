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
}

std::string CompilerDispatcher::compile(const std::string& source) {
    CHTLUnifiedScanner unified_scanner(source);
    chunks_ = unified_scanner.scan();

    std::string html_output;
    std::vector<std::string> js_outputs;

    for (const auto& chunk : chunks_) {
        if (chunk.content.empty()) continue;

        if (chunk.type == ChunkType::CHTL) {
            CHTLLoader loader;
            CHTLLexer lexer(chunk.content);
            std::vector<Token> tokens = lexer.scanTokens();
            if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) continue;

            CHTLParser parser(chunk.content, tokens, loader, "./", chtl_context_);
            std::unique_ptr<RootNode> ast = parser.parse();

            CHTLGenerator generator;
            html_output += generator.generate(*ast);

        } else if (chunk.type == ChunkType::ChtlJs) {
            CHTLJS::CHTLJSLexer lexer(chunk.content);
            std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();
            if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == CHTLJS::CHTLJSTokenType::EndOfFile)) continue;

            CHTLJS::CHTLJSParser parser(tokens);
            std::unique_ptr<CHTLJS::CHTLJSNode> ast = parser.parse();

            if (ast) {
                CHTLJS::CHTLJSGenerator generator;
                js_outputs.push_back(generator.generate(*ast));
            }
        }
    }

    CodeMerger merger;
    return merger.merge(html_output, js_outputs);
}

} // namespace CHTL
