#include "CompilerDispatcher.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include <vector>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string input_filepath, const std::map<std::string, std::string>& placeholder_map)
    : input_filepath_(std::move(input_filepath)),
      placeholder_map_(placeholder_map),
      chtl_context_(std::make_shared<ParserContext>()),
      chtl_generator_(chtl_context_),
      chtljs_context_(std::make_shared<CHTLJS::CHTLJSContext>()),
      code_merger_(placeholder_map)
       {
}

FinalCompilationResult CompilerDispatcher::dispatch(const std::vector<CodeFragment>& fragments) {
    chtl_context_->all_elements_.clear();
    chtl_context_->symbol_table_.clear();
    chtl_context_->unresolved_properties_.clear();
    chtl_context_->responsive_variables_.clear();
    chtljs_context_->virtual_objects.clear();

    std::vector<std::unique_ptr<RootNode>> chtl_asts;

    // --- PASS 1: Analysis ---
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            if (fragment.content.empty() || fragment.content.find_first_not_of(" \t\n\r") == std::string::npos) continue;

            CHTL::CHTLLexer lexer(fragment.content);
            std::vector<CHTL::Token> tokens = lexer.scanTokens();
            if (tokens.empty() || tokens.back().type != TokenType::EndOfFile) {
                tokens.push_back({TokenType::EndOfFile, "", tokens.empty() ? 1 : tokens.back().line, 0});
            }
            if (tokens.size() <= 1) continue;

            CHTL::CHTLParser parser(fragment.content, tokens, loader_, input_filepath_, chtl_context_);
            std::unique_ptr<CHTL::RootNode> ast = parser.parse();
            if (ast) {
                chtl_generator_.analyze(*ast);
                chtl_asts.push_back(std::move(ast));
            }
        }
    }

    // --- PASS 2: Rendering ---
    RenderResult render_result = chtl_generator_.render(chtl_asts);

    for (const auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL_JS:
                processChtlJsFragment(fragment.content);
                break;
            case FragmentType::CSS:
                processCssFragment(fragment.content);
                break;
            default:
                break;
        }
    }

    std::string final_js_with_placeholders = compiled_js_.str() + render_result.js;
    std::string final_js = code_merger_.resolveJsPlaceholders(final_js_with_placeholders);

    FinalCompilationResult result;
    result.html = code_merger_.merge(render_result.html, compiled_css_.str() + render_result.css, final_js);

    return result;
}


void CompilerDispatcher::processCssFragment(const std::string& content) {
    compiled_css_ << content << std::endl;
}

void CompilerDispatcher::processChtlJsFragment(const std::string& content) {
    if (content.empty() || content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return;
    }
    CHTLJS::CHTLJSLexer lexer(content);
    std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();
    if (tokens.empty() || tokens.back().type != CHTLJS::CHTLJSTokenType::EndOfFile) {
        tokens.push_back({CHTLJS::CHTLJSTokenType::EndOfFile, "", tokens.empty() ? 1 : tokens.back().line, 0});
    }
    if (tokens.size() <= 1) return;

    CHTLJS::CHTLJSParser parser(tokens, chtljs_context_);
    std::unique_ptr<CHTLJS::SequenceNode> ast = parser.parse();

    if (ast) {
        CHTLJS::CHTLJSGenerator js_generator(chtljs_context_);
        std::string js_output = js_generator.generate(*ast, placeholder_map_);
        compiled_js_ << js_output << std::endl;
    }
}

void CompilerDispatcher::processJsFragment(const std::string& content) {
    compiled_js_ << content << std::endl;
}

} // namespace CHTL
