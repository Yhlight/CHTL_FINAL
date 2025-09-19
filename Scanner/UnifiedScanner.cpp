#include "UnifiedScanner.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const std::string& source) : source(source) {}

ScanResult UnifiedScanner::scan() {
    ScanResult result;
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::stringstream modified_source_ss;
    size_t last_pos = 0;
    int script_placeholder_count = 0;
    int style_placeholder_count = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        bool is_script_block = (tokens[i].type == TokenType::SCRIPT);
        bool is_style_block = (tokens[i].type == TokenType::STYLE);

        if ((is_script_block || is_style_block) && (i + 1 < tokens.size()) && (tokens[i + 1].type == TokenType::LEFT_BRACE)) {
            // Found a script or style block
            modified_source_ss << source.substr(last_pos, tokens[i].position - last_pos);

            int brace_count = 1;
            size_t block_start_token_index = i + 2;
            size_t block_end_token_index = std::string::npos;

            for (size_t j = block_start_token_index; j < tokens.size(); ++j) {
                if (tokens[j].type == TokenType::LEFT_BRACE) {
                    brace_count++;
                } else if (tokens[j].type == TokenType::RIGHT_BRACE) {
                    brace_count--;
                    if (brace_count == 0) {
                        block_end_token_index = j;
                        break;
                    }
                }
            }

            if (block_end_token_index != std::string::npos) {
                const Token& open_brace_token = tokens[i + 1];
                const Token& close_brace_token = tokens[block_end_token_index];

                size_t content_start_pos = open_brace_token.position + open_brace_token.lexeme.length();
                size_t content_end_pos = close_brace_token.position;
                std::string block_content = source.substr(content_start_pos, content_end_pos - content_start_pos);

                std::string placeholder;
                FragmentType type;

                if (is_script_block) {
                    placeholder = "__SCRIPT_PLACEHOLDER_" + std::to_string(script_placeholder_count++) + "__";
                    // Heuristic to detect CHTL JS
                    if (block_content.find("Listen") != std::string::npos ||
                        block_content.find("Animate") != std::string::npos ||
                        block_content.find("{{") != std::string::npos) {
                        type = FragmentType::CHTL_JS;
                    } else {
                        type = FragmentType::JS;
                    }
                } else { // is_style_block
                    placeholder = "__STYLE_PLACEHOLDER_" + std::to_string(style_placeholder_count++) + "__";
                    type = FragmentType::CSS; // Simplification for now
                }

                result.extracted_blocks.push_back({block_content, type, placeholder});
                modified_source_ss << placeholder;

                last_pos = close_brace_token.position + close_brace_token.lexeme.length();
                i = block_end_token_index; // Continue scanning after the block
            } else {
                // Unterminated block, treat as normal CHTL
                // This part of the source will be appended in the next iteration or after the loop
            }
        }
    }

    if (last_pos < source.length()) {
        modified_source_ss << source.substr(last_pos);
    }

    result.modified_source = modified_source_ss.str();
    return result;
}

} // namespace CHTL
