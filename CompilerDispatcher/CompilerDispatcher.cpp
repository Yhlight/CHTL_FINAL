#include "CompilerDispatcher.h"
#include "ImportVisitor.h"
#include "Loader.h"
#include "CHTLGenerator/Generator.h"
#include "CHTLJSGenerator/Generator.h"
#include <sstream>

CompilerDispatcher::CompilerDispatcher(const NodeList& ast, const ConfigurationContext& config)
    : ast(ast), config(config) {}

CompilationResult CompilerDispatcher::dispatch() {
    CompilationResult result;
    Loader loader;

    // 1. Find and process all imports
    ImportVisitor import_visitor;
    import_visitor.run(ast);
    const auto& imports = import_visitor.getImports();

    std::stringstream css_ss;
    std::stringstream js_ss;

    for (const auto* importNode : imports) {
        if (importNode->import_type == "Style") {
            css_ss << loader.readTextFile(importNode->path) << "\n";
        } else if (importNode->import_type == "JavaScript") {
            js_ss << loader.readTextFile(importNode->path) << "\n";
        }
    }
    result.compiled_css = css_ss.str();
    result.compiled_js = js_ss.str();


    // 2. Generate the base HTML from the AST
    try {
        Generator chtl_generator;
        result.base_html = chtl_generator.generate(ast);
    } catch (const std::exception& e) {
        result.base_html = "<!-- CHTL Compilation Failed: " + std::string(e.what()) + " -->";
    }

    return result;
}
