#include "CodeMerger/CodeMerger.h"
#include <sstream>
#include <map>

namespace CHTL {

std::string CodeMerger::merge(
    const std::string& html_output,
    const std::vector<std::string>& js_outputs,
    const std::map<std::string, std::string>& placeholder_map
) {
    std::string final_html = html_output;

    // First, replace all the placeholders for <script> and <style> tags
    for (const auto& pair : placeholder_map) {
        size_t pos = final_html.find(pair.first);
        if (pos != std::string::npos) {
            final_html.replace(pos, pair.first.length(), pair.second);
        }
    }

    // Then, merge the CHTL-JS generated script blocks
    if (js_outputs.empty()) {
        return final_html;
    }

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

    return final_html;
}

} // namespace CHTL
