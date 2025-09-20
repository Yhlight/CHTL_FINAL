#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../CHTLNode/ImportNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <algorithm>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

// Helper to get only the CHTL source from a file's content
std::string getChtlSource(const std::string& full_source) {
    CHTLUnifiedScanner scanner(full_source);
    auto fragments = scanner.scan();
    std::stringstream chtl_stream;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            chtl_stream << fragment.content;
        }
    }
    return chtl_stream.str();
}


CompilationResult CompilerDispatcher::dispatch() {
    // --- Pass 1: Discover all files transitively ---
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

        std::string current_source = file_contents[current_path];
        std::string chtl_only_source = getChtlSource(current_source);

        CHTLLexer lexer(chtl_only_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(chtl_only_source, tokens, this->all_template_definitions);
        parser.suppress_not_found_errors = true;

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

    // --- Pass 2: Template Population ---
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
        std::cout << "--- Parsing for templates: " << path << " ---" << std::endl;
        const std::string& source = file_contents[path];
        std::string chtl_only_source = getChtlSource(source);

        CHTLLexer lexer(chtl_only_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(chtl_only_source, tokens, this->all_template_definitions);
        parser.parse();
    }

    // --- Pass 3: Final Generation ---
    std::cout << "--- Generating final output from <main> ---" << std::endl;
    CHTLUnifiedScanner main_scanner(this->initial_source);
    auto main_fragments = main_scanner.scan();

    std::stringstream CHTL_source_stream;
    std::stringstream css_source_stream;
    std::stringstream js_source_stream;

    for (const auto& fragment : main_fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL:
                CHTL_source_stream << fragment.content;
                break;
            case FragmentType::CSS:
                css_source_stream << fragment.content;
                break;
            case FragmentType::JS:
                js_source_stream << fragment.content;
                break;
            case FragmentType::CHTL_JS:
                js_source_stream << fragment.content;
                break;
        }
    }

    std::string combined_CHTL = CHTL_source_stream.str();
    CHTLLexer final_lexer(combined_CHTL);
    auto final_tokens = final_lexer.scanTokens();
    CHTLParser final_parser(combined_CHTL, final_tokens, this->all_template_definitions);
    auto main_ast = final_parser.parse();

    CHTLGenerator generator(this->all_template_definitions);
    auto result = generator.generate(main_ast.get());

    this->final_output.html = result.html;
    this->final_output.css = result.css + css_source_stream.str();
    this->final_output.js = result.js + js_source_stream.str();

    return final_output;
}

} // namespace CHTL
