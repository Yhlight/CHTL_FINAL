#include "CodeMerger.h"
#include <string>

namespace CHTL {

CodeMerger::CodeMerger() {}

void CodeMerger::setMainHtml(const std::string& html) {
    main_html = html;
}

void CodeMerger::addCss(const std::string& css) {
    final_css += css;
}

void CodeMerger::addJs(const std::string& js) {
    final_js += js;
}

void CodeMerger::setCompiledBlocks(const std::map<std::string, std::string>& blocks) {
    compiled_blocks = blocks;
}

CompilationResult CodeMerger::getResult() {
    std::string result_html = main_html;
    for (const auto& pair : compiled_blocks) {
        const std::string& placeholder = pair.first;
        const std::string& content = pair.second;
        size_t pos = result_html.find(placeholder);
        if (pos != std::string::npos) {
            if (placeholder.find("SCRIPT") != std::string::npos) {
                final_js += content;
                result_html.replace(pos, placeholder.length(), ""); // Remove placeholder from HTML
            } else if (placeholder.find("STYLE") != std::string::npos) {
                final_css += content;
                result_html.replace(pos, placeholder.length(), ""); // Remove placeholder from HTML
            }
        }
    }
    return {result_html, final_css, final_js};
}

} // namespace CHTL
