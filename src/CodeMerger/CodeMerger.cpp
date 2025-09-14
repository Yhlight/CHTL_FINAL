#include "CodeMerger/CodeMerger.h"
#include <sstream>

namespace CHTL {

// Helper function to perform string replacement
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::string CodeMerger::merge(
    const std::string& html_output,
    const std::vector<std::string>& js_outputs,
    const std::map<std::string, std::string>& placeholder_map
) {
    if (js_outputs.empty()) {
        return html_output;
    }

    std::stringstream combined_js_stream;
    for (const auto& js : js_outputs) {
        combined_js_stream << js << "\n";
    }

    std::string combined_js = combined_js_stream.str();

    // Perform placeholder substitution
    for (const auto& pair : placeholder_map) {
        replaceAll(combined_js, pair.first, pair.second);
    }

    std::string js_script_tag = "<script>\n" + combined_js + "</script>";

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
