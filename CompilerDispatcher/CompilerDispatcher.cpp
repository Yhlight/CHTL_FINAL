#include "CompilerDispatcher.h"
#include "UnifiedScanner.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include "CHTLJSLexer/Lexer.h"
#include "CHTLJSParser/Parser.h"
#include "CHTLJSGenerator/Generator.h"
#include <sstream>

CompilerDispatcher::CompilerDispatcher(const ScannedContent& content) : content(content) {}

CompilationResult CompilerDispatcher::dispatch() {
    CompilationResult result;

    // 1. Compile CHTL content
    try {
        Lexer chtl_lexer(content.chtl_content);
        auto chtl_tokens = chtl_lexer.tokenize();
        Parser chtl_parser(chtl_tokens);
        auto chtl_ast = chtl_parser.parse();
        Generator chtl_generator;
        result.base_html = chtl_generator.generate(chtl_ast);
    } catch (const std::exception& e) {
        result.base_html = "<!-- CHTL Compilation Failed: " + std::string(e.what()) + " -->";
    }

    // 2. "Compile" CSS content
    std::stringstream css_ss;
    for (const auto& pair : content.css_blocks) {
        css_ss << pair.second << "\n";
    }
    result.compiled_css = css_ss.str();

    // 3. "Compile" JS content (for now, just concatenate)
    std::stringstream js_ss;
    for (const auto& pair : content.script_blocks) {
        js_ss << pair.second << "\n";
    }
    result.compiled_js = js_ss.str();

    return result;
}
