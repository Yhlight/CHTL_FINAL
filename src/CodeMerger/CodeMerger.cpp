#include "CodeMerger/CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output, const std::vector<std::string>& js_outputs) {
    if (js_outputs.empty()) {
        return html_output;
    }

    std::stringstream combined_js;
    for (const auto& js : js_outputs) {
        combined_js << js << "\n";
    }

    std::string js_script_tag = "<script>\n" + combined_js.str() + "</script>";

    std::string final_html = html_output;
    size_t body_end_pos = final_html.rfind("</body>");

    if (body_end_pos != std::string::npos) {
        final_html.insert(body_end_pos, js_script_tag + "\n");
    } else {
        // If no body tag is found, just append it to the end.
        final_html += "\n" + js_script_tag;
    }

    return final_html;
}

} // namespace CHTL
