#include "CodeMerger.h"
#include <sstream>
#include <cctype>

namespace CHTL {

CodeMerger::CodeMerger(const std::map<std::string, std::string>& placeholder_map)
    : placeholder_map_(placeholder_map) {}

std::string CodeMerger::resolveJsPlaceholders(std::string js_with_placeholders) {
    for (const auto& pair : placeholder_map_) {
        size_t pos = js_with_placeholders.find(pair.first);
        while (pos != std::string::npos) {
            js_with_placeholders.replace(pos, pair.first.length(), pair.second);
            pos = js_with_placeholders.find(pair.first, pos + pair.second.length());
        }
    }
    return js_with_placeholders;
}

std::string CodeMerger::merge(const std::string& html_body,
                              const std::string& css,
                              const std::string& js) {

    std::stringstream final_html_stream;

    // Basic HTML5 structure
    final_html_stream << "<!DOCTYPE html>\n";
    final_html_stream << "<html lang=\"en\">\n";
    final_html_stream << "<head>\n";
    final_html_stream << "    <meta charset=\"UTF-8\">\n";
    final_html_stream << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    final_html_stream << "    <title>CHTL Output</title>\n";

    // Inject CSS
    if (!css.empty() && css.find_first_not_of(" \t\n\r") != std::string::npos) {
        final_html_stream << "    <style>\n" << css << "    </style>\n";
    }

    final_html_stream << "</head>\n";
    final_html_stream << "<body>\n";

    // Inject Body Content
    final_html_stream << html_body;

    // Inject JS
    if (!js.empty() && js.find_first_not_of(" \t\n\r") != std::string::npos) {
        final_html_stream << "<script>\n" << js << "</script>\n";
    }

    final_html_stream << "</body>\n";
    final_html_stream << "</html>\n";

    return final_html_stream.str();
}

} // namespace CHTL
