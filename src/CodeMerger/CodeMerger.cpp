#include "CodeMerger/CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output, const std::string& css_output, const std::vector<std::string>& js_outputs) {
    std::string final_html = html_output;

    // Handle CSS injection
    if (!css_output.empty()) {
        std::string css_style_tag = "<style>\n" + css_output + "\n</style>";
        size_t head_end_pos = final_html.find("</head>");
        if (head_end_pos != std::string::npos) {
            final_html.insert(head_end_pos, css_style_tag + "\n");
        } else {
            // If no head tag, prepend to the document. This is a fallback.
            final_html = css_style_tag + "\n" + final_html;
        }
    }

    // Handle JavaScript injection
    if (!js_outputs.empty()) {
        std::stringstream combined_js;
        for (const auto& js : js_outputs) {
            combined_js << js << "\n";
        }
        std::string js_script_tag = "<script>\n" + combined_js.str() + "</script>";
        size_t body_end_pos = final_html.rfind("</body>");
        if (body_end_pos != std::string::npos) {
            final_html.insert(body_end_pos, js_script_tag + "\n");
        } else {
            // If no body tag, append to the end.
            final_html += "\n" + js_script_tag;
        }
    }

    return final_html;
}

} // namespace CHTL
