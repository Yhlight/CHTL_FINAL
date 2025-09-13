#include "CodeMerger/CodeMerger.h"
#include <sstream>
#include <string>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output, const std::string& css_output, const std::vector<std::string>& js_outputs) {
    std::string final_html = html_output;

    // Merge CSS
    if (!css_output.empty()) {
        std::string css_style_tag = "<style>\n" + css_output + "\n</style>";
        size_t head_end_pos = final_html.find("</head>");
        if (head_end_pos != std::string::npos) {
            final_html.insert(head_end_pos, css_style_tag + "\n");
        } else {
            // If no </head>, maybe no <head> at all. Let's just prepend it.
            // A more robust solution might involve creating a <head> tag if one doesn't exist.
            // For now, this is a simple approach.
            final_html = css_style_tag + "\n" + final_html;
        }
    }

    // Merge JavaScript
    if (!js_outputs.empty()) {
        std::stringstream combined_js;
        for (const auto& js : js_outputs) {
            if (!js.empty()) {
                combined_js << js << "\n";
            }
        }

        if (combined_js.str().length() > 1) { // Check if not just a newline
            std::string js_script_tag = "<script>\n" + combined_js.str() + "</script>";
            size_t body_end_pos = final_html.rfind("</body>");
            if (body_end_pos != std::string::npos) {
                final_html.insert(body_end_pos, js_script_tag + "\n");
            } else {
                final_html += "\n" + js_script_tag;
            }
        }
    }

    return final_html;
}

} // namespace CHTL
