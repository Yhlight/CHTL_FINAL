#include "CodeMerger.h"
#include <sstream>
#include <cctype> // for isspace

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output,
                              const std::vector<std::string>& js_fragments) {

    if (js_fragments.empty()) {
        return html_output;
    }

    std::string final_html = html_output;

    std::stringstream combined_js_stream;
    for (const auto& js : js_fragments) {
        combined_js_stream << js << "\n";
    }
    std::string combined_js = combined_js_stream.str();

    // Avoid adding empty script tags if fragments only contained whitespace
    bool only_whitespace = true;
    for(char c : combined_js) {
        if(!isspace(c)) {
            only_whitespace = false;
            break;
        }
    }

    if (!only_whitespace) {
        std::string js_script_tag = "<script>\n" + combined_js + "</script>";

        size_t body_end_pos = final_html.rfind("</body>");
        if (body_end_pos != std::string::npos) {
            final_html.insert(body_end_pos, js_script_tag + "\n");
        } else {
            // If no body tag, just append to the end.
            final_html += "\n" + js_script_tag;
        }
    }

    return final_html;
}

} // namespace CHTL
