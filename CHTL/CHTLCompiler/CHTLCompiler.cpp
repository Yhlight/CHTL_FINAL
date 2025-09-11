#include "CHTLCompiler.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/ImportNode.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace CHTL {

CHTLCompiler::CHTLCompiler() {}

// Helper to read file content
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string CHTLCompiler::Compile(const std::string& source, const std::string& basePath) {
    m_context.SetBasePath(basePath);

    // 1. Parse the main source file
    CHTL::CHTLLexer mainLexer(source);
    auto mainTokens = mainLexer.Tokenize();
    CHTL::CHTLParser mainParser(mainTokens, source, m_context);
    auto mainAst = mainParser.Parse();
    m_context.MarkAsProcessed("main"); // Mark the initial source as processed

    // 2. Process imports iteratively
    while (true) {
        auto importsToProcess = m_context.GetImports();
        m_context.ClearImports(); // Clear the list for the next level of imports

        if (importsToProcess.empty()) {
            break; // No more imports to process
        }

        for (const auto* importNode : importsToProcess) {
            std::filesystem::path fullPath = m_context.GetBasePath();
            fullPath /= importNode->GetPath();
            std::string canonicalPath = std::filesystem::canonical(fullPath).string();

            if (m_context.IsProcessed(canonicalPath)) {
                continue; // Skip already processed file
            }

            std::string fileContent = readFile(canonicalPath);
            CHTL::CHTLLexer lexer(fileContent);
            auto tokens = lexer.Tokenize();
            CHTL::CHTLParser parser(tokens, fileContent, m_context);
            auto ast = parser.Parse();
            m_context.AddAst(std::move(ast)); // Keep the AST alive
            m_context.MarkAsProcessed(canonicalPath);
        }
    }

    // 3. Generate the final output from the main AST
    CHTL::CHTLGenerator generator(m_context);
    return generator.Generate(mainAst);
}

} // namespace CHTL
