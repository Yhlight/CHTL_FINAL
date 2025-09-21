#include "CompilerDispatcher.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../CHTLNode/ImportNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/StringUtil/StringUtil.h" // Include StringUtil
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <algorithm>
#include <sstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher(std::string root_path, std::map<std::string, TemplateDefinitionNode>& templates)
    : root_path(std::move(root_path)), all_template_definitions(templates) {}


FinalOutput CompilerDispatcher::dispatch() {
    // Pass 1: Discover all files and their aliases transitively.
    std::deque<std::string> files_to_process;
    std::vector<std::string> processing_order;
    std::set<std::string> processed_files;
    std::map<std::string, std::string> file_contents;
    std::map<std::string, std::string> file_aliases;

    files_to_process.push_back(this->root_path);
    processed_files.insert(this->root_path);
    file_contents[this->root_path] = FileSystem::readFile(this->root_path);

    int head = 0;
    while(head < files_to_process.size()) {
        std::string current_path = files_to_process[head++];
        processing_order.push_back(current_path);

        std::string current_source = file_contents[current_path];

        CHTLLexer lexer(current_source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(current_source, tokens, this->all_template_definitions);
        parser.suppress_not_found_errors = true;

        auto imports = parser.discoverImports();
        for (const auto& import_node : imports) {
            if (import_node->type == ImportType::CHTL) {
                if (processed_files.find(import_node->path) == processed_files.end()) {
                    std::cout << "--- Discovered CHTL Import: " << import_node->path << " ---" << std::endl;
                    processed_files.insert(import_node->path);
                    files_to_process.push_back(import_node->path);
                    file_contents[import_node->path] = FileSystem::readFile(import_node->path);
                    if (!import_node->alias.empty()) {
                        file_aliases[import_node->path] = import_node->alias;
                    }
                }
            } else if (import_node->type == ImportType::STYLE) {
                std::cout << "--- Found Style Import: " << import_node->path << " ---" << std::endl;
                final_output.imported_css += FileSystem::readFile(import_node->path) + "\n";
            } else if (import_node->type == ImportType::JAVASCRIPT) {
                 std::cout << "--- Found JavaScript Import: " << import_node->path << " ---" << std::endl;
                final_output.imported_js += FileSystem::readFile(import_node->path) + "\n";
            } else if (import_node->type == ImportType::HTML) {
                if (!import_node->alias.empty()) {
                    std::cout << "--- Found Named HTML Import: " << import_node->path << " as " << import_node->alias << " ---" << std::endl;
                    final_output.named_html_snippets[import_node->alias] = FileSystem::readFile(import_node->path);
                }
            }
        }
    }

    // Pass 2: Template Population, aware of namespaces and aliases.
    std::reverse(processing_order.begin(), processing_order.end());
    for (const auto& path : processing_order) {
         std::cout << "--- Parsing for templates: " << path << " ---" << std::endl;
        const std::string& source = file_contents[path];
        std::string default_ns;
        if (file_aliases.count(path)) {
            default_ns = file_aliases.at(path);
        } else {
            default_ns = StringUtil::getFilename(path);
        }

        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(source, tokens, this->all_template_definitions, default_ns);
        parser.parse();
    }


    // Pass 3: Final Generation
    std::cout << "--- Generating final output ---" << std::endl;

    std::stringstream combined_source;
    // This logic is flawed for final generation, as it doesn't account for namespaces correctly.
    // The correct way is to parse the main file again with the populated template map.
    const std::string& final_source = file_contents[this->root_path];

    CHTLLexer final_lexer(final_source);
    auto final_tokens = final_lexer.scanTokens();
    // The final parse does not need a default namespace alias.
    CHTLParser final_parser(final_source, final_tokens, this->all_template_definitions);
    auto main_ast = final_parser.parse();

    CHTLGenerator generator(this->all_template_definitions, this->final_output.named_html_snippets);
    generator.generate(main_ast.get());

    // The generator handles the main HTML and CSS. The dispatcher pre-filled the imported parts.
    this->final_output.html = generator.getHtml();
    this->final_output.css = generator.getCss();

    return this->final_output;
}

} // namespace CHTL
