#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLResolver/CHTLTemplateResolver.h"
#include "../CHTLAnalyzer/CHTLSemanticAnalyzer.h"
#include "../CHTLGenerator/CHTLGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>

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

std::string CompilerDispatcher::compile(const std::string& entry_filepath) {
    std::string source = readFile(entry_filepath);
    if (source.empty()) {
        return "Error: Could not read file " + entry_filepath;
    }

    CHTLUnifiedScanner scanner;
    CodeFragment fragment = scanner.scanFile(entry_filepath, source);

    switch (fragment.type) {
        case FragmentType::CHTL:
            return compileCHTL(entry_filepath, fragment.content);
        case FragmentType::CHTL_JS:
            return compileCHTL_JS(entry_filepath, fragment.content);
        // Add cases for CSS and JS later
        default:
            return "Error: Unknown file type for " + entry_filepath;
    }
}

std::string CompilerDispatcher::compileCHTL(const std::string& filepath, const std::string& source) {
    // This re-implements the logic from the old main.cpp
    // For now, it doesn't use the loader, it just processes the single fragment.
    // The loader needs to be refactored to work with the dispatcher.
    // For now, we will just use the single-file pipeline.

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer);
    auto programNode = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        std::string errors = "Parser errors in " + filepath + ":\n";
        for(const auto& err : parser.Errors()) {
            errors += err + "\n";
        }
        return errors;
    }
    if (!programNode) {
        return "Error: Parsing failed for " + filepath;
    }

    CHTL::CHTLTemplateResolver resolver;
    resolver.Process(programNode);

    CHTL::CHTLSemanticAnalyzer analyzer;
    analyzer.Process(programNode);

    std::string finalHtml;
    for(const auto& stmt : programNode->statements) {
        CHTL::CHTLGenerator generator(stmt);
        finalHtml += generator.Generate();
    }
    return finalHtml;
}

std::string CompilerDispatcher::compileCHTL_JS(const std::string& filepath, const std::string& source) {
    return "/* CHTL JS Compiler not yet implemented. */";
}


} // namespace CHTL
