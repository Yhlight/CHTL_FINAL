#include "CodeMerger/CodeMerger.h"
#include <sstream>
#include <map>

namespace CHTL {

std::string CodeMerger::merge(const std::string& html_output,
                              const std::vector<std::string>& js_outputs,
                              const std::map<std::string, std::string>& placeholder_map) {

    std::string final_html = html_output;

    // Merge JS into <body>
    if (!js_outputs.empty()) {
        std::stringstream combined_js_stream;
        for (const auto& js : js_outputs) {
            combined_js_stream << js << "\n";
        }
        std::string combined_js = combined_js_stream.str();

        // Substitute placeholders
        for (const auto& pair : placeholder_map) {
            size_t pos = combined_js.find(pair.first);
            while (pos != std::string::npos) {
                combined_js.replace(pos, pair.first.length(), pair.second);
                pos = combined_js.find(pair.first, pos + pair.second.length());
            }
        }

        if (!combined_js.empty()) {
             // Avoid adding empty script tags
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
                    final_html += "\n" + js_script_tag;
                }
            }
        }
    }

    return final_html;
}

} // namespace CHTL
