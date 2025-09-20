#include "CompilerDispatcher.h"
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

namespace CHTL {

void resolve_dependencies(
    const std::string& source,
    std::map<std::string, TemplateDefinitionNode>& all_templates,
    std::set<std::string>& processed_files,
    int depth
);

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

        std::string current_source = file_contents[current_path];

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
    // Parse files in reverse order of discovery. This ensures dependencies (which are discovered last)
    // are parsed first.
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
         std::cout << "--- Parsing for templates: " << path << " ---" << std::endl;
        const std::string& source = file_contents[path];
        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(source, tokens, this->all_template_definitions);
        parser.parse(); // This populates the map. Errors will be thrown if syntax is bad.
    }


    // Pass 3: Final Generation
    std::cout << "--- Generating final output ---" << std::endl;
    CHTLLexer final_lexer(this->initial_source);
    auto final_tokens = final_lexer.scanTokens();
    CHTLParser final_parser(this->initial_source, final_tokens, this->all_template_definitions);
    auto main_ast = final_parser.parse();

    CHTLGenerator generator(this->all_template_definitions);
    auto result = generator.generate(main_ast.get());

    this->final_output.html = result.html;
    this->final_output.css = result.css;

    return final_output;
}

} // namespace CHTL
