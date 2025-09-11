#include "CompilerDispatcher.h"
#include "ImportVisitor.h"
#include "Loader/ModuleLoader.h"
#include "CHTLGenerator/Generator.h"
#include "CHTL_JS/CHTLJSLexer/Lexer.h"
#include "CHTL_JS/CHTLJSParser/Parser.h"
#include "CHTL_JS/CHTLJSGenerator/Generator.h"
#include <sstream>
#include <iostream>

CompilerDispatcher::CompilerDispatcher(const NodeList& ast, const ScannedContent& content, const ConfigurationContext& config, const std::string& base_path)
    : ast(ast), content(content), config(config), base_path(base_path) {}

CompilationResult CompilerDispatcher::dispatch() {
    CompilationResult result;
    ModuleLoader loader(base_path);

    // 1. Find and process all imports (This logic is primitive and needs expanding later)
    ImportVisitor import_visitor;
    import_visitor.run(ast);
    const auto& imports = import_visitor.getImports();
    std::stringstream css_ss;
    std::stringstream js_ss;
    for (const auto* importNode : imports) {
        auto loaded_module = loader.load(importNode);
        if (loaded_module.success) {
            if (importNode->import_type == "Style") {
                css_ss << loaded_module.content << "\n";
            } else if (importNode->import_type == "JavaScript") {
                js_ss << loaded_module.content << "\n";
            }
        }
    }
    result.compiled_css = css_ss.str();
    if (js_ss.tellp() > 0) {
        result.compiled_scripts["__CHTL_IMPORTED_JS__"] = js_ss.str();
    }

    // 2. Compile all script placeholders
    for (const auto& pair : content.js_placeholders) {
        const std::string& placeholder = pair.first;
        const std::string& source = pair.second;
        try {
            CHTLJSLexer lexer(source);
            auto tokens = lexer.tokenize();

            CHTLJSParser parser(tokens);
            auto js_ast = parser.parse();

            CHTLJSGenerator generator;
            result.compiled_scripts[placeholder] = generator.generate(js_ast);

        } catch (const std::exception& e) {
            std::cerr << "CHTL JS Compilation Failed for placeholder " << placeholder << ": " << e.what() << std::endl;
            result.compiled_scripts[placeholder] = "/* CHTL JS Compilation Failed: " + std::string(e.what()) + " */";
        }
    }


    // 3. Generate the base HTML from the CHTL AST
    try {
        Generator chtl_generator;
        NodeList ast_copy = ast;
        result.base_html = chtl_generator.generate(ast_copy);
    } catch (const std::exception& e) {
        result.base_html = "<!-- CHTL Compilation Failed: " + std::string(e.what()) + " -->";
    }

    return result;
}
