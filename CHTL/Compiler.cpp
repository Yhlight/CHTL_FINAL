#include "Compiler.hpp"
#include "CHTLLexer/Lexer.hpp"
#include "CHTLParser/Parser.hpp"
#include "CHTLNode/ImportNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/NamespaceNode.hpp"
#include "FileSystem/FileSystem.hpp"
#include <algorithm>
#include <filesystem>

namespace CHTL {

Compiler::Compiler(const std::string& base_dir) : m_base_dir(base_dir) {}

std::string Compiler::compile(const std::string& entry_path) {
    std::vector<NodePtr> main_ast = processFile(entry_path);

    std::filesystem::path entry_path_fs(entry_path);
    std::string entry_dir = entry_path_fs.parent_path().string();
    if (entry_dir.empty()) entry_dir = ".";

    resolveImports(main_ast, entry_dir);

    m_context.collect(main_ast);

    HtmlGenerator generator;
    return generator.generate(main_ast, m_context);
}

std::vector<NodePtr> Compiler::processFile(const std::string& path) {
    // Prevent circular dependencies
    if (std::find(m_processed_files.begin(), m_processed_files.end(), path) != m_processed_files.end()) {
        return {}; // Return empty AST if already processed
    }
    m_processed_files.push_back(path);

    std::string source = FileSystem::readFile(path);
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    return parser.parse();
}

void Compiler::resolveImports(const std::vector<NodePtr>& ast, const std::string& current_dir) {
    for (const auto& node : ast) {
        if (auto import_node = std::dynamic_pointer_cast<ImportNode>(node)) {
            std::filesystem::path import_path(import_node->path.lexeme);
            std::filesystem::path current_dir_path(current_dir);
            std::filesystem::path full_path = current_dir_path / import_path;

            std::vector<NodePtr> imported_ast = processFile(full_path.string());

            std::string next_dir = full_path.parent_path().string();
            if (next_dir.empty()) next_dir = ".";

            resolveImports(imported_ast, next_dir); // Recurse for nested imports
            m_context.collect(imported_ast);
        } else if (auto elem_node = std::dynamic_pointer_cast<ElementNode>(node)) {
            resolveImports(elem_node->children, current_dir);
        } else if (auto ns_node = std::dynamic_pointer_cast<NamespaceNode>(node)) {
            resolveImports(ns_node->body, current_dir);
        }
    }
}

} // namespace CHTL
