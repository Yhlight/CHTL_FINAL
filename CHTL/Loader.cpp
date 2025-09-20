#include "Loader.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>

namespace CHTL {

Loader::Loader(std::shared_ptr<Configuration> config, const std::string& current_file_path)
    : m_config(config) {
    m_base_path = FileSystem::getDirectory(current_file_path);
}

std::string Loader::resolvePath(const std::string& path, const std::string& extension_hint) {
    // Simplified path resolution: only look relative to the current file.
    std::string full_path = m_base_path + path;

    if (FileSystem::fileExists(full_path)) {
        return full_path;
    }

    std::string path_with_ext = full_path + extension_hint;
    if (!extension_hint.empty() && FileSystem::fileExists(path_with_ext)) {
        return path_with_ext;
    }

    return ""; // Not found
}

LoadedContent Loader::load(const std::string& import_path, const std::string& import_type) {
    LoadedContent content;
    std::string resolved_path;

    if (import_type == "@Chtl") {
        resolved_path = resolvePath(import_path, ".chtl");
    } else {
        resolved_path = resolvePath(import_path, ""); // No specific extension
    }

    if (resolved_path.empty()) {
        std::cerr << "Error: Could not resolve import path '" << import_path << "'" << std::endl;
        return content;
    }

    std::string file_content = FileSystem::readFile(resolved_path);
    if (file_content.empty() && !FileSystem::fileExists(resolved_path)) {
         // readFile already prints an error, but we add context.
        std::cerr << "Error: Could not read imported file '" << resolved_path << "'" << std::endl;
        return content;
    }

    if (import_type == "@Chtl") {
        // Recursively parse the imported CHTL file
        CHTLLexer lexer(file_content, m_config);
        auto tokens = lexer.scanTokens();
        // The loader now takes ownership of the parser it creates
        m_last_parser = std::make_unique<CHTLParser>(file_content, tokens, resolved_path, m_config);

        content.ast_root = m_last_parser->parse();
        content.type = LoadedContent::Type::AST;
    } else {
        // For @Style, @JavaScript, etc., just return the raw text
        content.raw_text = file_content;
        content.type = LoadedContent::Type::RAW_TEXT;
    }

    return content;
}

CHTLParser& Loader::getMostRecentParser() {
    if (!m_last_parser) {
        throw std::runtime_error("No parser has been created by the loader yet.");
    }
    return *m_last_parser;
}

} // namespace CHTL
