#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace CHTL {

// Helper function to check if a substring matches at a given position
bool match_at(const std::string& s, size_t pos, const std::string& keyword) {
    if (pos + keyword.length() > s.length()) {
        return false;
    }
    return s.substr(pos, keyword.length()) == keyword;
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

// Forward declaration for the script content scanner
std::vector<CodeFragment> scanScriptContent(const std::string& script_source, int line_offset);

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    int current_line = 1;

    while (pos < source.length()) {
        size_t next_style = source.find("<style", pos);
        size_t next_script = source.find("<script", pos);

        size_t next_tag = std::min(next_style, next_script);

        if (next_tag == std::string::npos) {
            // No more style or script tags, the rest is CHTL
            if (pos < source.length()) {
                fragments.push_back({FragmentType::CHTL, source.substr(pos), current_line});
            }
            break;
        }

        // Add the CHTL fragment before the tag
        if (next_tag > pos) {
            fragments.push_back({FragmentType::CHTL, source.substr(pos, next_tag - pos), current_line});
            current_line += std::count(source.begin() + pos, source.begin() + next_tag, '\n');
        }

        // Find the end of the opening tag
        size_t tag_end = source.find('>', next_tag);
        if (tag_end == std::string::npos) {
            // Malformed tag, treat rest as CHTL and stop
            fragments.push_back({FragmentType::CHTL, source.substr(next_tag), current_line});
            break;
        }

        std::string open_tag = source.substr(next_tag, tag_end - next_tag + 1);
        fragments.push_back({FragmentType::CHTL, open_tag, current_line});
        current_line += std::count(open_tag.begin(), open_tag.end(), '\n');
        pos = tag_end + 1;

        // Handle the content of the tag
        if (next_tag == next_style) {
            size_t end_style = source.find("</style>", pos);
            if (end_style == std::string::npos) {
                // Malformed, treat rest as CHTL
                fragments.push_back({FragmentType::CHTL, source.substr(pos), current_line});
                break;
            }
            fragments.push_back({FragmentType::CSS, source.substr(pos, end_style - pos), current_line});
            current_line += std::count(source.begin() + pos, source.begin() + end_style, '\n');
            fragments.push_back({FragmentType::CHTL, "</style>", current_line});
            pos = end_style + 8; // length of "</style>"
        } else { // next_tag == next_script
            size_t end_script = source.find("</script>", pos);
             if (end_script == std::string::npos) {
                // Malformed, treat rest as CHTL
                fragments.push_back({FragmentType::CHTL, source.substr(pos), current_line});
                break;
            }

            std::string script_content = source.substr(pos, end_script - pos);

            auto script_fragments = scanScriptContent(script_content, current_line);
            fragments.insert(fragments.end(), script_fragments.begin(), script_fragments.end());

            current_line += std::count(script_content.begin(), script_content.end(), '\n');

            fragments.push_back({FragmentType::CHTL, "</script>", current_line});
            pos = end_script + 9; // length of "</script>"
        }
    }

    return fragments;
}


// Implementation of the detailed CHTL JS vs JS scanner
std::vector<CodeFragment> scanScriptContent(const std::string& script_source, int line_offset) {
    std::vector<CodeFragment> fragments;
    const std::vector<std::string> chtl_js_keywords = {"Listen", "Delegate", "Animate", "Vir", "Router", "ScriptLoader"};

    size_t pos = 0;
    int current_line = line_offset;

    while (pos < script_source.length()) {
        size_t start_of_js = pos;

        // Find the next potential CHTL_JS construct
        size_t next_construct = std::string::npos;

        // Check for {{
        size_t next_mustache = script_source.find("{{", pos);
        if (next_mustache != std::string::npos) {
            next_construct = std::min(next_construct, next_mustache);
        }

        // Check for $...$
        size_t next_dollar = script_source.find('$', pos);
        if (next_dollar != std::string::npos && next_dollar + 1 < script_source.length() && isalpha(script_source[next_dollar+1])) {
            next_construct = std::min(next_construct, next_dollar);
        }

        // Check for keywords
        for (const auto& keyword : chtl_js_keywords) {
            size_t next_keyword = script_source.find(keyword, pos);
            if (next_keyword != std::string::npos) {
                // Basic check to avoid matching keywords in the middle of other words
                if ((next_keyword == 0 || !isalnum(script_source[next_keyword-1])) &&
                    (next_keyword + keyword.length() >= script_source.length() || !isalnum(script_source[next_keyword + keyword.length()]))) {
                    next_construct = std::min(next_construct, next_keyword);
                }
            }
        }

        if (next_construct == std::string::npos) {
            // No more CHTL_JS, the rest is JS
            if (pos < script_source.length()) {
                fragments.push_back({FragmentType::JS, script_source.substr(pos), current_line});
            }
            break;
        }

        // Add the JS fragment before the construct
        if (next_construct > pos) {
            fragments.push_back({FragmentType::JS, script_source.substr(pos, next_construct - pos), current_line});
            current_line += std::count(script_source.begin() + pos, script_source.begin() + next_construct, '\n');
        }

        pos = next_construct;

        // Now, parse the CHTL_JS construct itself
        size_t construct_start = pos;
        if (match_at(script_source, pos, "{{")) {
            size_t end_mustache = script_source.find("}}", pos);
            if (end_mustache != std::string::npos) {
                pos = end_mustache + 2;
            }
        } else if (script_source[pos] == '$') {
            size_t end_dollar = script_source.find('$', pos + 1);
            if (end_dollar != std::string::npos) {
                pos = end_dollar + 1;
            }
        } else { // It's a keyword
            size_t lookahead = pos;
            while(lookahead < script_source.length() && isalpha(script_source[lookahead])) {
                lookahead++;
            }
            while(lookahead < script_source.length() && isspace(script_source[lookahead])) {
                lookahead++;
            }
            if (lookahead < script_source.length() && script_source[lookahead] == '{') {
                int brace_level = 1;
                pos = lookahead + 1;
                while(pos < script_source.length() && brace_level > 0) {
                    if (script_source[pos] == '{') brace_level++;
                    if (script_source[pos] == '}') brace_level--;
                    pos++;
                }
            }
        }

        // If we failed to parse a construct, just advance by one to avoid infinite loops
        if (pos == construct_start) {
             pos++;
        }

        fragments.push_back({FragmentType::CHTL_JS, script_source.substr(construct_start, pos - construct_start), current_line});
        current_line += std::count(script_source.begin() + construct_start, script_source.begin() + pos, '\n');
    }

    return fragments;
}


} // namespace CHTL
