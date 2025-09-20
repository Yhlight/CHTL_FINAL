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

#include "CHTLContext.h"

std::unique_ptr<CHTLContext> CHTLCompiler::compile(const std::string& entry_path) {
    auto context = std::make_unique<CHTLContext>();
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

        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::LEFT_BRACKET && (i + 6) < tokens.size() &&
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

    // --- 2. Individual Parsing Phase ---
    std::cout << "\n--- Parsing Phase ---" << std::endl;
    std::reverse(ordered_files.begin(), ordered_files.end());
    for (const auto& path : ordered_files) {
        std::cout << "Parsing file: " << path << std::endl;
        std::string source;
         try {
            source = loader.load(path);
        } catch (const std::runtime_error& e) {
            std::cerr << "Warning: " << e.what() << std::endl;
            continue;
        }

        // Set default namespace to filename without extension
        std::filesystem::path p(path);
        context->current_namespace = p.stem().string();

        CHTLLexer lexer(source);
        auto tokens = lexer.scanTokens();
        CHTLParser parser(source, tokens, *context);
        auto ast = parser.parse();
        context->files[path] = std::move(ast);
    }

    return context;
}

} // namespace CHTL
