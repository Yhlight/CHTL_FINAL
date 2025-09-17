#include "CodeMerger.h"

namespace CHTL {

CodeMerger::CodeMerger() {}

void CodeMerger::addHtml(const std::string& html) {
    m_html_stream << html;
}

void CodeMerger::addCss(const std::string& css) {
    m_css_stream << css;
}

void CodeMerger::addJs(const std::string& js) {
    m_js_stream << js;
}

std::string CodeMerger::getFinalOutput() {
    // For Phase 1, we are only concerned with HTML output.
    // In the future, this function will intelligently combine the HTML, CSS, and JS streams.
    // For example, it could put CSS into a <style> tag and JS into a <script> tag
    // within the final HTML document.
    return m_html_stream.str();
}

} // namespace CHTL
