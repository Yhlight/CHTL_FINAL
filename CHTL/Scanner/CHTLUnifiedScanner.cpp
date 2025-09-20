#include "CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <string_view>
#include <set>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source), current(0), line(1) {}

void CHTLUnifiedScanner::processScriptBlock(std::string_view content, int start_line, std::vector<CodeFragment>& fragments) {
    std::string chtl_js_output;
    std::string js_buffer;
    size_t last_pos = 0;

    const std::set<std::string> chtl_js_keywords = {"Listen", "Animate", "Delegate"};

    for (size_t i = 0; i < content.length(); ) {
        bool is_chtl_js = false;
        size_t construct_end = i;

        if (content.substr(i, 2) == "{{") {
            size_t end_pos = content.find("}}", i);
            if (end_pos != std::string::npos) {
                is_chtl_js = true;
                construct_end = end_pos + 2;
                // Check for -> operator immediately following
                size_t temp_pos = construct_end;
                while(temp_pos < content.length() && isspace(content[temp_pos])) temp_pos++;
                if (temp_pos + 1 < content.length() && content.substr(temp_pos, 2) == "->") {
                    construct_end = temp_pos + 2;
                }
            }
        } else {
            for (const auto& keyword : chtl_js_keywords) {
                if (content.substr(i, keyword.length()) == keyword) {
                    size_t lookahead = i + keyword.length();
                    while(lookahead < content.length() && isspace(content[lookahead])) lookahead++;
                    if (lookahead < content.length() && content[lookahead] == '{') {
                        int brace_level = 1;
                        construct_end = lookahead + 1;
                        while(construct_end < content.length() && brace_level > 0) {
                            if(content[construct_end] == '{') brace_level++;
                            else if(content[construct_end] == '}') brace_level--;
                            construct_end++;
                        }
                        is_chtl_js = true;
                        break;
                    }
                }
            }
        }

        if (is_chtl_js) {
            if (!js_buffer.empty()) {
                fragments.push_back({FragmentType::JS, js_buffer, start_line});
                chtl_js_output += "_JS_CODE_PLACEHOLDER_ ";
                js_buffer.clear();
            }
            chtl_js_output.append(content.substr(i, construct_end - i));
            i = construct_end;
            last_pos = i;
        } else {
            js_buffer += content[i];
            i++;
        }
    }

    if (!js_buffer.empty()) {
        fragments.push_back({FragmentType::JS, js_buffer, start_line});
        chtl_js_output += " _JS_CODE_PLACEHOLDER_";
    }

    if (!chtl_js_output.empty()) {
        fragments.push_back({FragmentType::CHTL_JS, chtl_js_output, start_line});
    }
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    CHTLLexer lexer{std::string(source)};
    std::vector<Token> tokens = lexer.scanTokens();

    if (tokens.empty()) {
        return fragments;
    }

    size_t last_pos = 0;
    int global_brace_level = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {

        bool is_style = tokens[i].type == TokenType::STYLE;
        bool is_script = tokens[i].type == TokenType::SCRIPT;

        // Track brace level to identify top-level blocks only
        if (tokens[i].type == TokenType::LEFT_BRACE) {
            global_brace_level++;
        } else if (tokens[i].type == TokenType::RIGHT_BRACE) {
            global_brace_level--;
        }

        if ((is_style || is_script) && global_brace_level == 0) {
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::LEFT_BRACE) {

                size_t keyword_start_pos = tokens[i].position;
                if (keyword_start_pos > last_pos) {
                    fragments.push_back({FragmentType::CHTL, std::string(source.substr(last_pos, keyword_start_pos - last_pos)), line});
                }

                int inner_brace_level = 0;
                size_t block_end_idx = i + 1;
                bool found_end = false;
                for (size_t j = i + 1; j < tokens.size(); ++j) {
                    if (tokens[j].type == TokenType::LEFT_BRACE) inner_brace_level++;
                    else if (tokens[j].type == TokenType::RIGHT_BRACE) {
                        inner_brace_level--;
                        if (inner_brace_level == 0) {
                            block_end_idx = j;
                            found_end = true;
                            break;
                        }
                    }
                }

                if (found_end) {
                    const auto& start_token = tokens[i];
                    const auto& open_brace_token = tokens[i+1];
                    const auto& end_token = tokens[block_end_idx];

                    size_t content_start_pos = open_brace_token.position + open_brace_token.lexeme.length();
                    size_t content_end_pos = end_token.position;

                    fragments.push_back({FragmentType::CHTL, std::string(source.substr(start_token.position, content_start_pos - start_token.position)), start_token.line});

                    std::string_view content(source.data() + content_start_pos, content_end_pos - content_start_pos);

                    if (is_style) {
                        fragments.push_back({FragmentType::CHTL, std::string(content), start_token.line});
                    } else {
                        processScriptBlock(content, start_token.line, fragments);
                    }

                    fragments.push_back({FragmentType::CHTL, "}", end_token.line});

                    last_pos = end_token.position + end_token.lexeme.length();
                    i = block_end_idx;
                    // The brace level is now back to 0 because we processed the block
                }
            }
        }
    }

    if (last_pos < source.length()) {
        fragments.push_back({FragmentType::CHTL, std::string(source.substr(last_pos)), line});
    }

    return fragments;
}

} // namespace CHTL
