#include "CodeMerger.h"
#include <sstream>

namespace CHTL {

std::string CodeMerger::merge(std::string html_output,
                              const std::vector<std::string>& js_scripts,
                              const std::vector<std::string>& css_styles) {

    // 1. Merge all CSS styles into a single <style> tag and inject into <head>
    if (!css_styles.empty()) {
        std::stringstream combined_css_stream;
        for (const auto& css : css_styles) {
            if (!css.empty()) {
                combined_css_stream << css << "\n";
            }
        }
        std::string combined_css = combined_css_stream.str();
        if (!combined_css.empty()) {
            std::string style_tag = "<style>\n" + combined_css + "</style>";
            size_t head_end_pos = html_output.find("</head>");
            if (head_end_pos != std::string::npos) {
                html_output.insert(head_end_pos, style_tag + "\n");
            } else {
                // Fallback if no <head> tag is found
                html_output.insert(0, style_tag + "\n");
            }
        }
    }

    // 2. Merge all JS scripts into a single <script> tag and inject before </body>
    if (!js_scripts.empty()) {
        std::stringstream combined_js_stream;
        for (const auto& js : js_scripts) {
            if (!js.empty()) {
                combined_js_stream << js << "\n";
            }
        }
        std::string combined_js = combined_js_stream.str();
        if (!combined_js.empty()) {
            std::string script_tag = "<script>\n" + combined_js + "</script>";
            size_t body_end_pos = html_output.rfind("</body>");
            if (body_end_pos != std::string::npos) {
                html_output.insert(body_end_pos, script_tag + "\n");
            } else {
                // Fallback if no </body> tag is found
                html_output += "\n" + script_tag;
            }
        }
    }

    return html_output;
}

} // namespace CHTL
