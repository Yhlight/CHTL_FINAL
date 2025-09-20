#include "CodeMerger.h"
#include <fstream>
#include <stdexcept>

namespace CHTL {

CodeMerger::CodeMerger(const FinalOutput& output) : output(output) {}

std::string CodeMerger::mergeToSingleFile() const {
    std::string result = "<html>\n<head>\n";

    if (!output.css.empty()) {
        result += "<style>\n" + output.css + "\n</style>\n";
    }

    result += "</head>\n<body>\n";
    result += output.html;

    if (!output.js.empty()) {
        result += "<script>\n" + output.js + "\n</script>\n";
    }

    result += "</body>\n</html>";
    return result;
}

std::string CodeMerger::mergeCssInline() const {
    std::string result = "<html>\n<head>\n";

    if (!output.css.empty()) {
        result += "<style>\n" + output.css + "\n</style>\n";
    }

    result += "</head>\n<body>\n";
    result += output.html;

    if (!output.js.empty()) {
         result += "<script src=\"output.js\"></script>\n";
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
    html_file << "<html>\n<head>\n";

    if (!output.css.empty()){
        html_file << "<link rel=\"stylesheet\" href=\"" << base_filename << ".css\">\n";
    }

    html_file << "</head>\n<body>\n"
              << output.html;

    if (!output.js.empty()){
        html_file << "<script src=\"" << base_filename << ".js\"></script>\n";
    }

    html_file << "</body>\n</html>";
    html_file.close();

    // Save CSS
    if (!output.css.empty()) {
        std::ofstream css_file(base_filename + ".css");
        if (!css_file.is_open()) {
            throw std::runtime_error("Could not open file to save CSS: " + base_filename + ".css");
        }
        css_file << output.css;
        css_file.close();
    }

    // Save JS
    if (!output.js.empty()) {
        std::ofstream js_file(base_filename + ".js");
        if (!js_file.is_open()) {
            throw std::runtime_error("Could not open file to save JS: " + base_filename + ".js");
        }
        js_file << output.js;
        js_file.close();
    }
}

void CodeMerger::saveCssInlined(const std::string& base_filename) const {
    // Save HTML with inlined CSS
    std::ofstream html_file(base_filename + ".html");
    if (!html_file.is_open()) {
        throw std::runtime_error("Could not open file to save HTML: " + base_filename + ".html");
    }
    html_file << mergeCssInline();
    html_file.close();

     // Save JS
    if (!output.js.empty()) {
        std::ofstream js_file(base_filename + ".js");
        if (!js_file.is_open()) {
            throw std::runtime_error("Could not open file to save JS: " + base_filename + ".js");
        }
        js_file << output.js;
        js_file.close();
    }
}

} // namespace CHTL
