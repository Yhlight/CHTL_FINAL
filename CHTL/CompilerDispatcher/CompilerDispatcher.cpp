#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <algorithm>
#include <map>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

FinalOutput CompilerDispatcher::dispatch() {
    // Data structures for multi-file processing
    std::map<std::string, std::string> file_contents;
    std::map<std::string, std::vector<CodeFragment>> file_fragments;
    std::vector<std::string> processing_order;
    std::set<std::string> processed_files;
    std::deque<std::string> files_to_process;

    // --- Pass 1: Discover all files and fragment them ---
    std::cout << "--- Pass 1: Discovering and scanning files ---" << std::endl;

    file_contents["<main>"] = this->initial_source;
    files_to_process.push_back("<main>");
    processed_files.insert("<main>");

    while(!files_to_process.empty()) {
        std::string current_path = files_to_process.front();
        files_to_process.pop_front();
        processing_order.push_back(current_path);

        std::cout << "Scanning: " << current_path << std::endl;
        const std::string& current_source = file_contents[current_path];

        // Use the Unified Scanner to get fragments
        CHTLUnifiedScanner scanner(current_source);
        file_fragments[current_path] = scanner.scan();

        // Discover imports only from CHTL fragments
        for (const auto& fragment : file_fragments[current_path]) {
            if (fragment.type == FragmentType::CHTL) {
                CHTLLexer lexer(fragment.content);
                auto tokens = lexer.scanTokens();
                CHTLParser parser(fragment.content, tokens, this->all_template_definitions);

                std::vector<std::string> imports = parser.discoverImports();
                for (const auto& import_path : imports) {
                    if (processed_files.find(import_path) == processed_files.end()) {
                        std::cout << "  Discovered Import: " << import_path << std::endl;
                        processed_files.insert(import_path);
                        files_to_process.push_back(import_path);
                        file_contents[import_path] = FileSystem::readFile(import_path);
                    }
                }
            }
        }
    }

    // --- Pass 2: Template Population ---
    std::cout << "\n--- Pass 2: Populating templates ---" << std::endl;
    // Parse files in reverse order of discovery to handle dependencies correctly.
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
        std::cout << "Parsing for templates: " << path << std::endl;
        for (const auto& fragment : file_fragments[path]) {
            if (fragment.type == FragmentType::CHTL) {
                CHTLLexer lexer(fragment.content);
                auto tokens = lexer.scanTokens();
                CHTLParser parser(fragment.content, tokens, this->all_template_definitions);
                parser.parse(); // This populates the template map
            }
        }
    }

    // --- Pass 3: Final Generation from main file's fragments ---
    std::cout << "\n--- Pass 3: Generating final output ---" << std::endl;
    std::string final_html, final_css, final_js;

    for (const auto& fragment : file_fragments["<main>"]) {
        switch (fragment.type) {
            case FragmentType::CHTL: {
                std::cout << "Generating CHTL..." << std::endl;
                CHTLLexer lexer(fragment.content);
                auto tokens = lexer.scanTokens();
                CHTLParser parser(fragment.content, tokens, this->all_template_definitions);
                auto ast = parser.parse();
                CHTLGenerator generator(this->all_template_definitions);
                auto result = generator.generate(ast.get());
                final_html += result.html;
                final_css += result.css;
                // JS from CHTL generator (e.g., for local script blocks) can be handled later
                break;
            }
            case FragmentType::CSS: {
                std::cout << "Appending CSS..." << std::endl;
                // In the future, this would go to a CSS compiler/processor.
                // For now, we just append it directly.
                final_css += fragment.content;
                break;
            }
            case FragmentType::CHTL_in_CSS: {
                std::cout << "Generating CHTL in CSS..." << std::endl;
                // This is a CHTL fragment that needs to be compiled to CSS.
                // This requires a specialized CHTL-to-CSS generator path.
                // For now, we'll comment it out in the final output.
                final_css += "/* CHTL_in_CSS not yet implemented:\n" + fragment.content + "\n*/\n";
                break;
            }
            case FragmentType::JS: {
                 std::cout << "Appending JS..." << std::endl;
                // This case is for pure JS fragments extracted by the scanner.
                // In a real scenario, these would be re-assembled by the CodeMerger.
                // For now, we'll just append them.
                final_js += fragment.content + "\n";
                break;
            }
            case FragmentType::CHTL_JS: {
                std::cout << "Generating CHTL_JS..." << std::endl;
                // This is the stubbed CHTL JS pipeline
                CHTL_JS::CHTMLJSLexer js_lexer(fragment.content);
                auto js_tokens = js_lexer.scanTokens();

                CHTL_JS::CHTLJSParser js_parser(fragment.content, js_tokens);
                auto js_ast = js_parser.parse();

                CHTL_JS::CHTLJSGenerator js_generator;
                std::string compiled_js_with_placeholders = js_generator.generate(js_ast.get());

                // Substitute placeholders
                std::string final_compiled_js = compiled_js_with_placeholders;
                for (const auto& pair : fragment.js_placeholders) {
                    size_t pos = final_compiled_js.find(pair.first);
                    if (pos != std::string::npos) {
                        final_compiled_js.replace(pos, pair.first.length(), pair.second);
                    }
                }
                final_js += final_compiled_js;
                break;
            }
        }
    }

    this->final_output.html = final_html;
    this->final_output.css = final_css;
    this->final_output.js = final_js;

    return final_output;
}

} // namespace CHTL
