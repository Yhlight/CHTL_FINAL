#include "CompilerDispatcher.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../Scanner/CHTLUnifiedScanner.h"
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

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

FinalOutput CompilerDispatcher::dispatch() {
    // Pass 1: Discover all files transitively.
    std::deque<std::string> files_to_process;
    std::vector<std::string> processing_order;
    std::set<std::string> processed_files;

    files_to_process.push_back("<main>");
    processed_files.insert("<main>");

    std::map<std::string, std::string> file_contents;
    file_contents["<main>"] = this->initial_source;

    while(!files_to_process.empty()) {
        std::string current_path = files_to_process.front();
        files_to_process.pop_front();
        processing_order.push_back(current_path);

        const std::string& current_source = file_contents[current_path];

        CHTLLexer lexer(current_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(current_source, tokens, this->all_template_definitions);

        std::vector<std::string> imports = parser.discoverImports();
        for (const auto& import_path : imports) {
            if (processed_files.find(import_path) == processed_files.end()) {
                std::cout << "--- Discovered Import: " << import_path << " ---" << std::endl;
                processed_files.insert(import_path);
                files_to_process.push_back(import_path);
                file_contents[import_path] = FileSystem::readFile(import_path);
            }
        }
    }

    // Pass 2: Template Population.
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
        std::cout << "--- Discovering templates in: " << path << " ---" << std::endl;
        const std::string& source = file_contents[path];
        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(source, tokens, this->all_template_definitions);
        parser.discoverTemplates();
    }

    // Pass 3: Final Generation using the Unified Scanner
    std::cout << "--- Generating final output from fragments ---" << std::endl;
    CHTLUnifiedScanner scanner(this->initial_source);
    auto fragments = scanner.scan();

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            CHTLLexer lexer(fragment.content);
            auto tokens = lexer.scanTokens();
            CHTLParser parser(fragment.content, tokens, this->all_template_definitions);
            auto ast = parser.parse();

            CHTLGenerator generator(this->all_template_definitions);
            auto result = generator.generate(ast.get());

            this->final_output.html += result.html;
            // CSS from CHTL fragments (e.g. local style blocks) should also be collected
            this->final_output.css += result.css;

        } else if (fragment.type == FragmentType::CSS) {
            this->final_output.css += fragment.content;
        } else if (fragment.type == FragmentType::JS) {
            this->final_output.js += fragment.content;
        } else if (fragment.type == FragmentType::CHTL_JS) {
            CHTLJS::CHTLJSLexer js_lexer(fragment.content);
            auto js_tokens = js_lexer.scanTokens();

            CHTLJS::CHTLJSParser js_parser(js_tokens);
            auto js_ast_nodes = js_parser.parse();

            CHTLJS::CHTLJSGenerator js_generator;
            this->final_output.js += js_generator.generate(js_ast_nodes);
        }
    }

    return final_output;
}

} // namespace CHTL
