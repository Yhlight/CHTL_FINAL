#include "CodeMerger.h"

// Helper function to replace all occurrences of a substring
void replace_all(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

CodeMerger::CodeMerger(const CompilationResult& result, const ScannedContent& content)
    : result(result), content(content) {}

std::string CodeMerger::merge() {
    std::string final_html = result.base_html;

    // Replace CSS placeholders from local <style> blocks
    for (const auto& pair : content.css_placeholders) {
        replace_all(final_html, pair.first, pair.second);
    }

    // Replace JS placeholders with their *compiled* code
    for (const auto& pair : result.compiled_scripts) {
        replace_all(final_html, pair.first, pair.second);
    }

    // Append globally imported CSS
    if (!result.compiled_css.empty()) {
        final_html += "\n<style>\n" + result.compiled_css + "\n</style>";
    }

    return final_html;
}
