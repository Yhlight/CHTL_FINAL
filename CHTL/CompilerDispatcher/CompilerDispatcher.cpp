#include "CompilerDispatcher.h"
#include "CHTL/Scanner/CHTLUnifiedScanner.h"
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
#include <regex>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string initial_source)
    : initial_source(std::move(initial_source)) {}

// Helper to extract only CHTL-related source from fragments
std::string get_chtl_source_from_fragments(const std::vector<CodeFragment>& fragments) {
    std::string chtl_source;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_in_CSS) {
            chtl_source += fragment.content + "\n";
        }
    }
    return chtl_source;
}


FinalOutput CompilerDispatcher::dispatch() {
    // Pass 1: Discover all files transitively.
    std::deque<std::string> files_to_process;
    std::vector<std::string> processing_order;
    std::set<std::string> processed_files;

    files_to_process.push_back("<main>");
    processed_files.insert("<main>");

    std::map<std::string, std::string> file_contents;
    file_contents["<main>"] = this->initial_source;

    // Use an index-based loop since files_to_process grows
    for(size_t i = 0; i < files_to_process.size(); ++i) {
        std::string current_path = files_to_process[i];
        processing_order.push_back(current_path);

        std::string current_source = file_contents[current_path];

        // Use the scanner to get fragments first
        CHTLUnifiedScanner scanner(current_source);
        auto fragments = scanner.scan();
        std::string chtl_source = get_chtl_source_from_fragments(fragments);

        // Now parse only the CHTL source to discover imports
        CHTLLexer lexer(chtl_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(chtl_source, tokens, this->all_template_definitions);
        parser.suppress_not_found_errors = true; // Suppress errors during discovery pass

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

    // Pass 2: Template & Definition Population.
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
        std::cout << "--- Parsing for templates: " << path << " ---" << std::endl;
        const std::string& source = file_contents[path];

        CHTLUnifiedScanner scanner(source);
        auto fragments = scanner.scan();
        std::string chtl_source = get_chtl_source_from_fragments(fragments);

        CHTLLexer lexer(chtl_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(chtl_source, tokens, this->all_template_definitions);
        parser.parse(); // This populates the map.
    }


    // Pass 3: Final Generation
    std::cout << "--- Generating final output from <main> ---" << std::endl;
    CHTLUnifiedScanner final_scanner(this->initial_source);
    auto final_fragments = final_scanner.scan();

    std::string main_chtl_source = get_chtl_source_from_fragments(final_fragments);
    std::string css_from_blocks;
    std::string js_template_from_blocks;
    std::map<std::string, std::string> js_placeholders;

    // Process style and script blocks from the main file
    for(const auto& fragment : final_fragments) {
        if (fragment.type == FragmentType::STYLE_BLOCK) {
            // TODO: This should use the CHTL-in-CSS processor from the scanner.
            // For now, just extract the content.
            size_t open_brace = fragment.content.find('{');
            size_t close_brace = fragment.content.rfind('}');
            if (open_brace != std::string::npos && close_brace != std::string::npos) {
                css_from_blocks += fragment.content.substr(open_brace + 1, close_brace - open_brace - 1);
            }
        } else if (fragment.type == FragmentType::SCRIPT_BLOCK) {
            // TODO: This should use the CHTL-JS processor.
            size_t open_brace = fragment.content.find('{');
            size_t close_brace = fragment.content.rfind('}');
            if (open_brace != std::string::npos && close_brace != std::string::npos) {
                js_template_from_blocks += fragment.content.substr(open_brace + 1, close_brace - open_brace - 1);
            }
        }
    }


    CHTLLexer final_lexer(main_chtl_source);
    auto final_tokens = final_lexer.scanTokens();
    CHTLParser final_parser(main_chtl_source, final_tokens, this->all_template_definitions);
    auto main_ast = final_parser.parse();

    CHTLGenerator generator(this->all_template_definitions);
    auto result = generator.generate(main_ast.get());

    this->final_output.html = result.html;
    // For now, just concatenate. Real implementation would need merging.
    this->final_output.css = result.css + "\n" + css_from_blocks;
    this->final_output.js = js_template_from_blocks; // Simplified

    return final_output;
}

} // namespace CHTL
