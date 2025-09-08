#include "CHTLLoader.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLParser/Parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace CHTL {

// Helper function for the production file provider
static std::string readFileFromDisk(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

CHTLLoader::CHTLLoader() : m_file_provider(readFileFromDisk) {}

CHTLLoader::CHTLLoader(FileProvider provider) : m_file_provider(std::move(provider)) {}

void CHTLLoader::queueImport(const std::string& current_file_path, const std::string& import_path) {
    std::filesystem::path current_dir = std::filesystem::path(current_file_path).parent_path();
    std::filesystem::path absolute_import_path = current_dir / import_path;

    std::string canonical_path = std::filesystem::weakly_canonical(absolute_import_path).string();

    if (m_parsed_files.find(canonical_path) == m_parsed_files.end()) {
        m_import_queue.push_back(canonical_path);
    }
}

std::unique_ptr<DocumentNode> CHTLLoader::loadAndParse(const std::string& entry_point_path, CHTLContext& context) {
    auto final_ast = std::make_unique<DocumentNode>();

    // Get the canonical path for the entry point to ensure consistent comparisons
    std::string canonical_entry_path = std::filesystem::weakly_canonical(entry_point_path).string();
    queueImport(".", entry_point_path);

    while(!m_import_queue.empty()) {
        std::string current_file = m_import_queue.front();
        m_import_queue.erase(m_import_queue.begin());

        if (m_parsed_files.count(current_file)) {
            continue;
        }

        m_parsed_files.insert(current_file);
        std::string source;
        try {
            source = m_file_provider(current_file);
        } catch (const std::exception& e) {
            // In a real compiler, we might want better error reporting here.
            // For now, just rethrow.
            throw std::runtime_error("Failed to read file '" + current_file + "': " + e.what());
        }

        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens, context, *this, current_file);
        std::unique_ptr<DocumentNode> file_ast = parser.parse();

        // We only care about the side-effects of parsing (populating the context).
        // The AST of the main file is returned, others are discarded after parsing.
        if (current_file == canonical_entry_path) {
            final_ast = std::move(file_ast);
        }
    }

    return final_ast;
}

} // namespace CHTL
