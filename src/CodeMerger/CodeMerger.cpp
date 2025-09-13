#include "CodeMerger/CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output, const std::vector<std::string>& js_outputs, const std::vector<std::string>& css_outputs) {
    std::string final_html = html_output;

    // Combine and prepend CSS
    if (!css_outputs.empty()) {
        std::stringstream combined_css;
        for (const auto& css : css_outputs) {
            combined_css << css << "\n";
        }
        std::string css_style_tag = "<style>\n" + combined_css.str() + "</style>\n";
        final_html.insert(0, css_style_tag);
    }

    // Combine and inject JavaScript
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
            final_html += "\n" + js_script_tag;
        }
    }

    return final_html;
}

} // namespace CHTL
