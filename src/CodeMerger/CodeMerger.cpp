#include "CodeMerger/CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output,
                              const std::vector<std::string>& css_outputs,
                              const std::vector<std::string>& js_outputs) {
    std::string final_html = html_output;

    // Merge CSS
    if (!css_outputs.empty()) {
        std::stringstream combined_css;
        for (const auto& css : css_outputs) {
            combined_css << css << "\n";
        }
        std::string css_style_tag = "<style>\n" + combined_css.str() + "</style>";

        size_t head_end_pos = final_html.rfind("</head>");
        if (head_end_pos != std::string::npos) {
            final_html.insert(head_end_pos, css_style_tag + "\n");
        } else {
            // If no head tag, prepend it. This is a simple heuristic.
            final_html = "<head>\n" + css_style_tag + "\n</head>\n" + final_html;
        }
    }

    // Merge JS
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
            // If no body tag is found, just append it to the end.
            final_html += "\n" + js_script_tag;
        }
    }

    return final_html;
}

} // namespace CHTL
