#include "Loader.h"
#include "../Util/FileSystem/FileSystem.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include <iostream>

namespace CHTL {

Loader::Loader() {}

void Loader::loadImports(Program* program, Context& context) {
    for (const auto& stmt : program->statements) {
        if (auto importNode = dynamic_cast<ImportNode*>(stmt.get())) {
            processImport(importNode, context);
        }
    }
}

void Loader::processImport(ImportNode* importNode, Context& context) {
    const std::string& path = importNode->path;

    if (m_loaded_files.count(path)) {
        return;
    }

    if (!FileSystem::fileExists(path)) {
        std::cerr << "Error: Cannot find file to import: " << path << std::endl;
        return;
    }

    m_loaded_files.insert(path);
    std::string source;
    try {
        source = FileSystem::readFile(path);
    } catch (const std::exception& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return;
    }

    Lexer lexer(source);
    Parser parser(lexer, context);
    auto imported_program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        std::cerr << "Parser errors in imported file " << path << ":" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << "\t" << error << std::endl;
        }
        return;
    }

    loadImports(imported_program.get(), context);
}

} // namespace CHTL
