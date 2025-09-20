#include "CHTLCompiler.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/TemplateDeclarationNode.h"
#include "CHTLNode/CustomDeclarationNode.h"
#include <iostream>
#include <queue>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

namespace CHTL {

CHTLCompiler::CHTLCompiler() {}

std::unique_ptr<BaseNode> CHTLCompiler::compile(const std::string& entry_path) {
    std::queue<std::string> files_to_discover;
    std::vector<std::string> ordered_files;

    this->parsed_files.clear();
    files_to_discover.push(entry_path);
    this->parsed_files.insert(std::filesystem::canonical(entry_path).string());

    // --- 1. Discovery Phase: Find all unique files by lexing ---
    std::cout << "--- Discovery Phase ---" << std::endl;
    while (!files_to_discover.empty()) {
        std::string current_path = files_to_discover.front();
        files_to_discover.pop();

        std::cout << "Discovering imports in: " << current_path << std::endl;
        ordered_files.push_back(current_path);

        std::string source;
        try {
            source = loader.load(current_path);
        } catch (const std::runtime_error& e) {
            std::cerr << "Warning: " << e.what() << std::endl;
            continue;
        }

        // Only lex the file and manually search for import statements.
        // This is robust and avoids any parsing errors during discovery.
        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::LEFT_BRACKET && (i + 5) < tokens.size() &&
                tokens[i+1].type == TokenType::IMPORT &&
                tokens[i+2].type == TokenType::RIGHT_BRACKET &&
                tokens[i+3].type == TokenType::AT &&
                tokens[i+4].type == TokenType::CHTL &&
                tokens[i+5].type == TokenType::FROM &&
                tokens[i+6].type == TokenType::STRING) {

                std::string import_path_str = tokens[i+6].lexeme;
                std::filesystem::path parent_path = std::filesystem::path(current_path).parent_path();
                std::filesystem::path import_path(import_path_str);
                std::filesystem::path canonical_path = std::filesystem::canonical(parent_path / import_path);

                if (this->parsed_files.find(canonical_path.string()) == this->parsed_files.end()) {
                    files_to_discover.push(canonical_path.string());
                    this->parsed_files.insert(canonical_path.string());
                }
            }
        }
    }

    // --- 2. Concatenation and Final Parse Phase ---
    std::cout << "\n--- Final Parse Phase ---" << std::endl;
    std::string final_source;
    std::reverse(ordered_files.begin(), ordered_files.end());
    for (const auto& path : ordered_files) {
        std::cout << "Concatenating source from: " << path << std::endl;
        final_source += loader.load(path) + "\n";
    }

    CHTLLexer final_lexer(final_source);
    auto final_tokens = final_lexer.scanTokens();
    CHTLParser final_parser(final_source, final_tokens, this->template_definitions);
    auto final_ast = final_parser.parse();

    // Filter out the import nodes from the final AST before generation.
    if (auto* root = dynamic_cast<ElementNode*>(final_ast.get())) {
        auto& children = root->children;
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [](const std::unique_ptr<BaseNode>& node) {
                    return dynamic_cast<ImportNode*>(node.get()) != nullptr;
                }),
            children.end()
        );
    }

    return final_ast;
}

} // namespace CHTL
