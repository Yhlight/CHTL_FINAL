#include "CodeMerger.h"
#include "CompilerDispatcher.h"

CodeMerger::CodeMerger(const CompilationResult& result) : result(result) {}

std::string CodeMerger::merge() {
    std::string final_html = result.base_html;

    // Inject CSS into the head
    if (!result.compiled_css.empty()) {
        std::string css_to_inject = "\n<style>\n" + result.compiled_css + "</style>\n";
        size_t head_pos = final_html.find("</head>");
        if (head_pos != std::string::npos) {
            final_html.insert(head_pos, css_to_inject);
        } else {
            // Fallback: prepend to the document if no head tag is found
            final_html.insert(0, css_to_inject);
        }
    }

    // Inject JS into the body
    if (!result.compiled_js.empty()) {
        std::string js_to_inject = "\n<script>\n" + result.compiled_js + "</script>\n";
        size_t body_pos = final_html.find("</body>");
        if (body_pos != std::string::npos) {
            final_html.insert(body_pos, js_to_inject);
        } else {
            // Fallback: append to the document if no body tag is found
            final_html.append(js_to_inject);
        }
    }

    return final_html;
}
