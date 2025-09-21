#include "CodeMerger.h"
#include <fstream>
#include <stdexcept>

namespace CHTL {

CodeMerger::CodeMerger(const FinalOutput& output) : output(output) {}

std::string CodeMerger::mergeToSingleFile() const {
    std::string result = "<html>\n<head>\n";

    std::string final_css = output.imported_css + "\n" + output.css;
    if (!final_css.empty()) {
        result += "<style>\n" + final_css + "\n</style>\n";
    }

    result += "</head>\n<body>\n";
    result += output.html;

    std::string final_js = output.js + "\n" + output.imported_js;
    if (!final_js.empty()) {
        result += "<script>\n" + final_js + "\n</script>\n";
    }

    result += "</body>\n</html>";
    return result;
}

void CodeMerger::saveToSeparateFiles(const std::string& base_filename) const {
    // Save HTML
    std::ofstream html_file(base_filename + ".html");
    if (!html_file.is_open()) {
        throw std::runtime_error("Could not open file to save HTML: " + base_filename + ".html");
    }
    html_file << "<html>\n<head>\n"
              << "<link rel=\"stylesheet\" href=\"" << base_filename << ".css\">\n"
              << "</head>\n<body>\n"
              << output.html
              << "<script src=\"" << base_filename << ".js\"></script>\n"
              << "</body>\n</html>";
    html_file.close();

    // Save CSS
    std::string final_css = output.imported_css + "\n" + output.css;
    if (!final_css.empty()) {
        std::ofstream css_file(base_filename + ".css");
        if (!css_file.is_open()) {
            throw std::runtime_error("Could not open file to save CSS: " + base_filename + ".css");
        }
        css_file << final_css;
        css_file.close();
    }

    // Save JS
    std::string final_js = output.js + "\n" + output.imported_js;
    if (!final_js.empty()) {
        std::ofstream js_file(base_filename + ".js");
        if (!js_file.is_open()) {
            throw std::runtime_error("Could not open file to save JS: " + base_filename + ".js");
        }
        js_file << final_js;
        js_file.close();
    }
}

} // namespace CHTL
