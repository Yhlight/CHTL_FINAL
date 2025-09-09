#include "CompilerDispatcher.h"
#include "../CHTLLoader/Loader.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include <iostream>
#include <string>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {}

void CompilerDispatcher::findAndCompileScripts(Node* node, std::string& js_buffer) {
    if (auto el = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : el->children) {
            if (auto script_node = dynamic_cast<LocalScriptNode*>(child.get())) {
                js_buffer += m_js_compiler.compile(script_node->raw_content);
            } else {
                findAndCompileScripts(child.get(), js_buffer);
            }
        }
    }
}

std::string CompilerDispatcher::compile(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer, m_context);
    auto program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        std::string error_messages = "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            error_messages += "\t" + error + "\n";
        }
        return error_messages;
    }

    Loader loader;
    loader.loadImports(program.get(), m_context);

    // After loading, find and compile all local scripts
    std::string js_from_scripts;
    findAndCompileScripts(program.get(), js_from_scripts);

    Generator generator(m_context);
    GenerationResult gen_result = generator.generate(program.get());

    // Combine JS from generator (global scripts) and compiled local scripts
    std::string all_js = gen_result.global_js + js_from_scripts;

    CodeMerger merger;
    return merger.merge(gen_result.html, gen_result.global_css, all_js);
}

} // namespace CHTL
