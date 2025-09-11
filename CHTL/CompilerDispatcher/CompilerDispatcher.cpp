#include "CompilerDispatcher.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLResolver/CHTLTemplateResolver.h"
#include "../CHTLAnalyzer/CHTLSemanticAnalyzer.h"
#include "../CHTLGenerator/CHTLGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {

static std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string CompilerDispatcher::compile(const std::string& entry_filepath, bool default_struct) {
    CHTLUnifiedScanner scanner;
    CodeFragment fragment = scanner.scanFile(entry_filepath, "");

    switch (fragment.type) {
        case FragmentType::CHTL:
            return compileCHTL(entry_filepath, "", default_struct);
        case FragmentType::CHTL_JS:
            return compileCHTL_JS(entry_filepath, "");
        default:
            return "Error: Unknown file type for " + entry_filepath;
    }
}

std::string CompilerDispatcher::compileCHTL(const std::string& filepath, const std::string& source, bool default_struct) {
    CHTLLoader loader;
    auto programNode = loader.Load(filepath);

    if (!programNode) {
        return "Error: Loading/Parsing failed.";
    }

    CHTLTemplateResolver resolver;
    resolver.Process(programNode);

    CHTLSemanticAnalyzer analyzer;
    analyzer.Process(programNode);

    std::string finalHtml;
    for(const auto& stmt : programNode->statements) {
        CHTLGenerator generator(stmt, default_struct);
        finalHtml += generator.Generate();
    }
    return finalHtml;
}

std::string CompilerDispatcher::compileCHTL_JS(const std::string& filepath, const std::string& source) {
    return "/* CHTL JS Compiler not yet implemented. */";
}

} // namespace CHTL
