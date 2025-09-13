#include "UnifiedScanner.h"
#include <cctype>

namespace CHTL {

UnifiedScanner::UnifiedScanner(const std::string& source) : source_(source) {}

FragmentList UnifiedScanner::scan() {
    while (current_ < source_.length()) {
        findNextFragment();
    }
    return fragments_;
}

void UnifiedScanner::findNextFragment() {
    size_t script_pos = source_.find("script", current_);

    if (script_pos == std::string::npos) {
        // No more script tags, the rest is CHTL
        processChtlFragment(source_.length());
        return;
    }

    size_t brace_pos = source_.find('{', script_pos + 6);
    if (brace_pos == std::string::npos) {
        // Found "script" but no brace, treat as CHTL
        processChtlFragment(source_.length());
        return;
    }

    // Check for only whitespace between "script" and "{"
    bool is_script_block = true;
    for(size_t i = script_pos + 6; i < brace_pos; ++i) {
        if (!isspace(source_[i])) {
            is_script_block = false;
            break;
        }
    }

    if (is_script_block) {
        // We found a script block. Everything before it is CHTL.
        processChtlFragment(script_pos);
        // current_ is now at the start of "script"
        processScriptFragment();
    } else {
        // "script" was not followed by a brace, so it's just part of a CHTL block.
        // Continue searching from after this occurrence.
        current_ = script_pos + 6;
        findNextFragment();
    }
}

void UnifiedScanner::processChtlFragment(size_t end) {
    if (end > current_) {
        fragments_.push_back({source_.substr(current_, end - current_), FragmentType::CHTL});
    }
    current_ = end;
}

void UnifiedScanner::processScriptFragment() {
    // current_ is at the start of the word "script"
    size_t start_brace = source_.find('{', current_);
    if (start_brace == std::string::npos) return; // Should not happen

    current_ = start_brace + 1;

    int brace_level = 1;
    size_t end_brace = std::string::npos;

    for (size_t i = current_; i < source_.length(); ++i) {
        if (source_[i] == '{') {
            brace_level++;
        } else if (source_[i] == '}') {
            brace_level--;
            if (brace_level == 0) {
                end_brace = i;
                break;
            }
        }
    }

    if (end_brace != std::string::npos) {
        fragments_.push_back({source_.substr(start_brace + 1, end_brace - (start_brace + 1)), FragmentType::JavaScript});
        current_ = end_brace + 1;
    } else {
        // Unterminated script block, treat rest of file as JS for now
        fragments_.push_back({source_.substr(current_), FragmentType::JavaScript});
        current_ = source_.length();
    }
}

} // namespace CHTL
