#include "CHTLLoader.h"
#include "../../Util/FileSystem/FileSystemUtil.h"
#include "../CHTLLexer/Lexer.h"
#include "../CHTLParser/Parser.h"
#include <iostream>

CHTLLoader::CHTLLoader(TemplateStore& templateStore) : m_templateStore(templateStore) {}

bool CHTLLoader::load(const ImportNode* importNode) {
    // Simplified logic: only handle full CHTL file imports for now.
    if (importNode->importTypeString != "@Chtl") {
        // For now, ignore other import types.
        return true;
    }

    std::string source;
    try {
        // Simplified path resolution: assume relative to current execution dir.
        source = FileSystemUtil::readFile(importNode->path);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error loading import file: " << e.what() << std::endl;
        return false;
    }

    // Create a new lexer and parser for the imported file.
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // The new parser will use the *same* template store, but we need to
    // handle the namespace. The spec says the default namespace is the filename.
    std::string defaultNamespace = importNode->path;
    // A simple way to get filename from path:
    size_t last_slash_idx = defaultNamespace.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) {
        defaultNamespace.erase(0, last_slash_idx + 1);
    }
    size_t period_idx = defaultNamespace.rfind('.');
    if (std::string::npos != period_idx) {
        defaultNamespace.erase(period_idx);
    }

    // Pass the calculated default namespace and a reference to this loader to the parser
    Parser parser(tokens, m_templateStore, *this, defaultNamespace);

    parser.parseProgram(); // This will populate the store.

    const auto& errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "--- Errors found in imported file: " << importNode->path << " ---" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return false;
    }

    return true;
}
