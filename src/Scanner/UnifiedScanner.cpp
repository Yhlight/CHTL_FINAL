#include "UnifiedScanner.h"
#include "../ThirdParty/MinimalJSTokenizer.h"
#include <sstream>
#include <vector>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const std::string& source) : source_(source) {}

std::string UnifiedScanner::swissCheeseScanScript(const std::string& script_content) {
    ThirdParty::MinimalJSTokenizer tokenizer(script_content);
    std::vector<ThirdParty::JSToken> all_tokens = tokenizer.tokenize();

    // Create a token stream without whitespace for easier pattern matching
    std::vector<ThirdParty::JSToken> tokens;
    for(const auto& t : all_tokens) {
        if (t.type != ThirdParty::JSTokenType::Whitespace) {
            tokens.push_back(t);
        }
    }

    std::stringstream result_ss;
    std::stringstream js_buffer;

    auto flush_js_buffer = [&]() {
        if (js_buffer.rdbuf()->in_avail() > 0) {
            std::string placeholder = "__JS_CODE_" + std::to_string(placeholder_counter_++) + "__";
            placeholders_[placeholder] = js_buffer.str();
            result_ss << placeholder;
            js_buffer.str("");
            js_buffer.clear();
        }
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];

        // Look for the CHTL JS listen pattern: IDENTIFIER -> listen { ... }
        if (i + 3 < tokens.size() &&
            token.type == ThirdParty::JSTokenType::Identifier &&
            tokens[i+1].type == ThirdParty::JSTokenType::Operator && tokens[i+1].text == "->" &&
            tokens[i+2].type == ThirdParty::JSTokenType::Keyword && tokens[i+2].text == "listen" &&
            tokens[i+3].type == ThirdParty::JSTokenType::Operator && tokens[i+3].text == "{")
        {
            flush_js_buffer(); // Flush any JS code before this construct

            // Find the matching closing brace
            int brace_level = 1;
            size_t block_start_index = i + 3;
            size_t block_end_index = block_start_index;
            for (size_t j = block_start_index + 1; j < tokens.size(); ++j) {
                if (tokens[j].text == "{") brace_level++;
                if (tokens[j].text == "}") brace_level--;
                if (brace_level == 0) {
                    block_end_index = j;
                    break;
                }
            }

            // Append the entire CHTL JS construct
            for(size_t k = i; k <= block_end_index; ++k) {
                result_ss << tokens[k].text;
            }
            // Also need to consume the semicolon if it exists
            if (block_end_index + 1 < tokens.size() && tokens[block_end_index + 1].text == ";") {
                result_ss << tokens[block_end_index + 1].text;
                block_end_index++;
            }

            i = block_end_index; // Move the main loop index forward
        }
        else if (token.type == ThirdParty::JSTokenType::CHTLLiteral) {
            flush_js_buffer();
            result_ss << token.text;
        } else {
            js_buffer << token.text;
        }
    }
    flush_js_buffer(); // Flush any remaining JS code at the end

    return result_ss.str();
}


void UnifiedScanner::scanForBlocks(
    std::string& source,
    const std::string& block_name,
    const std::string& placeholder_prefix,
    bool sub_scan
) {
    size_t pos = 0;
    while ((pos = source.find(block_name, pos)) != std::string::npos) {
        size_t block_start = source.find('{', pos);
        if (block_start == std::string::npos) {
            pos++;
            continue;
        }

        size_t current_pos = block_start + 1;
        int brace_level = 1;
        while (brace_level > 0 && current_pos < source.length()) {
            char c = source[current_pos];
            if (c == '{') brace_level++;
            else if (c == '}') brace_level--;
            current_pos++;
        }

        if (brace_level > 0) {
            pos++;
            continue;
        }

        size_t block_end = current_pos;
        std::string content = source.substr(block_start + 1, block_end - block_start - 2);

        if (sub_scan) {
            content = swissCheeseScanScript(content);
        }

        std::string placeholder = placeholder_prefix + std::to_string(placeholder_counter_++) + "__";
        placeholders_[placeholder] = content;

        source.replace(block_start, block_end - block_start, " { " + placeholder + " } ");
        pos = block_start + placeholder.length();
    }
}

void UnifiedScanner::scanForSubstrings(
    std::string& source,
    const std::string& start_delim,
    const std::string& end_delim,
    const std::string& placeholder_prefix
) {
    size_t pos = 0;
    while ((pos = source.find(start_delim, pos)) != std::string::npos) {
        size_t end_pos = source.find(end_delim, pos + start_delim.length());
        if (end_pos == std::string::npos) {
            pos++;
            continue;
        }

        size_t block_end = end_pos + end_delim.length();
        std::string content = source.substr(pos, block_end - pos);

        std::string placeholder = placeholder_prefix + std::to_string(placeholder_counter_++) + "__";
        placeholders_[placeholder] = content;

        source.replace(pos, block_end - pos, placeholder);
        pos += placeholder.length();
    }
}


ScanResult UnifiedScanner::scan() {
    std::string modified_source = source_;

    // Scan for blocks that should be treated as opaque by the main CHTL parser
    scanForBlocks(modified_source, "[Origin] @Html", "__CHTL_ORIGIN_", false);
    scanForBlocks(modified_source, "[Origin] @Style", "__CHTL_ORIGIN_", false);
    scanForBlocks(modified_source, "[Origin] @JavaScript", "__CHTL_ORIGIN_", false);

    // For script blocks, we do a sub-scan to find JS constructs
    scanForBlocks(modified_source, "script", "__CHTL_SCRIPT_", true);

    return {modified_source, placeholders_};
}

} // namespace CHTL
