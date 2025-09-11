#include "CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ImportNode.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <deque>

namespace CHTL {

// Helper function to read a file into a string
static std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::shared_ptr<ProgramNode> CHTLLoader::Load(const std::string& entry_filepath) {
    auto final_program = std::make_shared<ProgramNode>();
    std::deque<std::string> file_queue;

    // Start with the absolute path of the entry file
    file_queue.push_back(std::filesystem::absolute(entry_filepath).string());

    while(!file_queue.empty()) {
        std::string current_filepath = file_queue.front();
        file_queue.pop_front();

        if (m_processed_files.count(current_filepath)) {
            continue; // Already processed, skip to avoid cycles
        }

        std::string source = readFile(current_filepath);
        if (source.empty()) {
            m_error_messages.push_back("Could not open or read file: " + current_filepath);
            continue;
        }
        m_processed_files.insert(current_filepath);

        CHTL::CHTLLexer lexer(source);
        CHTL::CHTLParser parser(lexer);
        auto program = parser.ParseProgram();

        if (!parser.Errors().empty()) {
            for(const auto& err : parser.Errors()) {
                m_error_messages.push_back(current_filepath + ": " + err);
            }
        }
        if (!program) {
             m_error_messages.push_back("Parser returned null program for: " + current_filepath);
             continue;
        }

        auto current_dir = std::filesystem::path(current_filepath).parent_path();
        for (const auto& stmt : program->statements) {
            if (auto import_node = std::dynamic_pointer_cast<ImportNode>(stmt)) {
                auto import_path = current_dir / import_node->path;
                file_queue.push_back(import_path.lexically_normal().string());
            } else {
                // Add all other statements (templates, elements) to the final program
                final_program->statements.push_back(stmt);
            }
        }
    }

    return final_program;
}

} // namespace CHTL
