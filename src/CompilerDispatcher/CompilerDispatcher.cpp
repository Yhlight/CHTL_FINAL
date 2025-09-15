#include "CompilerDispatcher.h"

// Main pipeline components
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"

// CHTL toolchain
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"

// CHTL JS toolchain
#include "CHTLJS/CHTLJSplitter.h"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"

#include <sstream>
#include <vector>
#include <stdexcept>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    chtl_context_ = std::make_shared<ParserContext>();
    chtljs_context_ = std::make_shared<CHTLJS::CHTLJSContext>();
    chtl_generator_ = std::make_unique<CHTLGenerator>();
    chtljs_generator_ = std::make_unique<CHTLJS::CHTLJSGenerator>();
    code_merger_ = std::make_unique<CodeMerger>();
}

CompilerDispatcher::~CompilerDispatcher() = default;

std::string CompilerDispatcher::compile(const std::string& source) {
    if (source.empty()) return "";

    // 1. Main Scan: Separate CHTL from script/style blocks
    CHTLUnifiedScanner scanner(source);
    ScanResult scan_result = scanner.scan();

    // 2. Compile the main CHTL source (with placeholders)
    std::string& chtl_source = scan_result.chtl_with_placeholders;
    CHTLLoader loader;
    CHTLLexer lexer(chtl_source);
    std::vector<Token> tokens = lexer.scanTokens();

    // Handle empty CHTL source after scanning
    if (tokens.empty() || (tokens.size() == 1 && tokens[0].type == TokenType::EndOfFile)) {
        // If there's no CHTL, we might still have JS/CSS fragments to process
        // For now, if the main template is empty, we return empty.
        // A more robust implementation might return a basic HTML structure with the scripts/styles.
        if (scan_result.placeholder_map.empty()) {
            return "";
        }
    }

    CHTLParser parser(chtl_source, tokens, loader, "./", chtl_context_);
    std::unique_ptr<RootNode> ast = parser.parse();
    if (!ast) {
        // Parser should have already printed an error message.
        return "";
    }
    CompilationResult chtl_compilation_result = chtl_generator_->generate(*ast);
    std::string html_output = chtl_compilation_result.html;

    // 3. Process all fragments
    std::vector<std::string> js_to_inject;
    std::vector<std::string> css_to_inject;

    // Add JS generated from CHTL (e.g., responsive values)
    if (!chtl_compilation_result.js.empty()) {
        js_to_inject.push_back(chtl_compilation_result.js);
    }

    for (const auto& pair : scan_result.placeholder_map) {
        const CodeFragment& fragment = pair.second;

        if (fragment.type == FragmentType::JS) {
            // A. Split the JS fragment into pure JS and CHTL JS
            CHTLJS::CHTLJSplitter splitter(fragment.content);
            CHTLJS::JsSplitResult split_result = splitter.split();

            // B. Compile the CHTL JS part
            CHTLJS::CHTLJSLexer cjs_lexer(split_result.cjs_with_placeholders);
            std::vector<CHTLJS::CHTLJSToken> cjs_tokens = cjs_lexer.scanTokens();
            CHTLJS::CHTLJSParser cjs_parser(split_result.cjs_with_placeholders, cjs_tokens, chtljs_context_);
            std::unique_ptr<CHTLJS::SequenceNode> cjs_ast = cjs_parser.parse();
            std::string compiled_cjs = chtljs_generator_->generate(*cjs_ast);

            // C. Re-substitute the pure JS back into the compiled CHTL JS
            for (const auto& js_pair : split_result.js_placeholder_map) {
                size_t pos = compiled_cjs.find(js_pair.first);
                if (pos != std::string::npos) {
                    compiled_cjs.replace(pos, js_pair.first.length(), js_pair.second);
                }
            }
            js_to_inject.push_back(compiled_cjs);

        } else if (fragment.type == FragmentType::CSS) {
            css_to_inject.push_back(fragment.content);
        }

        // D. Remove the placeholder from the HTML
        size_t placeholder_pos = html_output.find(fragment.placeholder);
        if (placeholder_pos != std::string::npos) {
            html_output.erase(placeholder_pos, fragment.placeholder.length());
        }
    }

    // 4. Final Merge
    return code_merger_->merge(html_output, js_to_inject, css_to_inject);
}

} // namespace CHTL
