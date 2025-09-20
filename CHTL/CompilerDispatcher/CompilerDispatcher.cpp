#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
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

FinalOutput CompilerDispatcher::dispatch() {
    // Step 1: Scan the initial source file to separate it into fragments.
    std::cout << "--- Scanning for code fragments ---" << std::endl;
    CHTLUnifiedScanner unified_scanner(this->initial_source);
    std::vector<CodeFragment> main_fragments = unified_scanner.scan();

    std::stringstream combined_chtl_source;
    for (const auto& fragment : main_fragments) {
        if (fragment.type == FragmentType::CHTL) {
            combined_chtl_source << fragment.content;
        } else if (fragment.type == FragmentType::CSS) {
            this->final_output.css += fragment.content;
        } else if (fragment.type == FragmentType::JS) {
            this->final_output.js += fragment.content;
        }
    }

    std::string main_chtl_source = combined_chtl_source.str();

    // Step 2: Discover all imported files from CHTL fragments.
    std::cout << "--- Discovering imports from CHTL code ---" << std::endl;
    std::deque<std::string> files_to_process;
    std::vector<std::string> processing_order;
    std::set<std::string> processed_files;

    files_to_process.push_back("<main>");
    processed_files.insert("<main>");

    std::map<std::string, std::string> file_contents;
    file_contents["<main>"] = main_chtl_source;

    while(!files_to_process.empty()) {
        std::string current_path = files_to_process.front();
        files_to_process.pop_front();
        processing_order.push_back(current_path);

        std::string current_source = file_contents[current_path];

        // In imported files, we also need to separate CHTL from CSS/JS
        CHTLUnifiedScanner import_scanner(current_source);
        auto import_fragments = import_scanner.scan();
        std::stringstream import_chtl_source;
        for(const auto& frag : import_fragments) {
            if(frag.type == FragmentType::CHTL) {
                import_chtl_source << frag.content;
            } else if (frag.type == FragmentType::CSS) {
                this->final_output.css += frag.content;
            } else if (frag.type == FragmentType::JS) {
                this->final_output.js += frag.content;
            }
        }

        CHTLLexer lexer(import_chtl_source.str());
        auto tokens = lexer.scanTokens();
        CHTLParser parser(import_chtl_source.str(), tokens, this->all_template_definitions);

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

    // Step 3: Template Population.
    std::cout << "--- Parsing all CHTL for templates ---" << std::endl;
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
        std::cout << "--- Parsing for templates: " << path << " ---" << std::endl;
        const std::string& source_with_css_js = file_contents[path];

        CHTLUnifiedScanner template_scanner(source_with_css_js);
        auto template_fragments = template_scanner.scan();
        std::stringstream template_chtl_source;
        for(const auto& frag : template_fragments) {
            if(frag.type == FragmentType::CHTL) {
                template_chtl_source << frag.content;
            }
        }

        const std::string source = template_chtl_source.str();
        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(source, tokens, this->all_template_definitions);
        parser.parse(); // This populates the map.
    }


    // Step 4: Final Generation from main CHTL source
    std::cout << "--- Generating final output from main file ---" << std::endl;
    CHTLLexer final_lexer(main_chtl_source);
    auto final_tokens = final_lexer.scanTokens();
    CHTLParser final_parser(main_chtl_source, final_tokens, this->all_template_definitions);
    auto main_ast = final_parser.parse();

    CHTLGenerator generator(this->all_template_definitions);
    auto result = generator.generate(main_ast.get());

    this->final_output.html = result.html;
    // Append generated CSS to the CSS from global style blocks
    this->final_output.css = result.css + this->final_output.css;

    return final_output;
}

} // namespace CHTL
