#include "CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(
    const std::string& html_output,
    const std::vector<std::string>& js_outputs,
    const std::map<std::string, std::string>& placeholder_map
) {
    if (js_outputs.empty()) {
        return html_output;
    }

    std::stringstream combined_js;
    for (const auto& js_part : js_outputs) {
        // Check if the part is a placeholder
        if (placeholder_map.count(js_part)) {
            // It's a placeholder, so substitute it with the real JS code
            combined_js << placeholder_map.at(js_part);
        } else {
            // It's not a placeholder (it's JS generated from ChtlJs), so use it directly
            combined_js << js_part;
        }
        combined_js << "\n";
    }

    // Avoid creating an empty script tag if the combined JS is empty
    // (e.g., if all chunks were empty placeholders)
    if (combined_js.str().find_first_not_of(" \n\r\t") == std::string::npos) {
        return html_output;
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
