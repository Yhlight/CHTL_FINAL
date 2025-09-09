#include "CHTL.h"
#include "CHTLContext.h"
#include "CHTLLexer.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLCompiler::CHTLCompiler() {
    context = std::make_unique<CHTLContext>();
    lexer = std::make_unique<CHTLLexer>();
    // Initialize other components as needed
}

CHTLCompiler::~CHTLCompiler() = default;

bool CHTLCompiler::compile(const std::string& inputFile, const std::string& outputFile) {
    try {
        // Read input file
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            context->addError("Failed to open input file: " + inputFile);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string input = buffer.str();
        file.close();
        
        // Compile the input
        std::string output;
        bool success = compileString(input, output);
        
        if (success) {
            // Write output file
            std::ofstream outFile(outputFile);
            if (!outFile.is_open()) {
                context->addError("Failed to open output file: " + outputFile);
                return false;
            }
            
            outFile << output;
            outFile.close();
        }
        
        return success;
    } catch (const std::exception& e) {
        context->addError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

bool CHTLCompiler::compileString(const std::string& input, std::string& output) {
    try {
        // Clear previous errors
        context->clearErrors();
        context->clearWarnings();
        
        // Tokenize input
        auto tokens = lexer->tokenize(input);
        
        // Check for lexer errors
        auto lexerErrors = lexer->getErrors();
        for (const auto& error : lexerErrors) {
            context->addError("Lexer error: " + error);
        }
        
        if (!lexerErrors.empty()) {
            return false;
        }
        
        // TODO: Parse tokens into AST
        // TODO: Generate output from AST
        
        // For now, just echo the input as a placeholder
        output = "<!-- CHTL Compiler v1.0.0 -->\n";
        output += "<!-- Input:\n" + input + "\n-->\n";
        output += "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL Output</title>\n</head>\n<body>\n";
        output += "<p>CHTL compilation successful!</p>\n";
        output += "<p>Input length: " + std::to_string(input.length()) + " characters</p>\n";
        output += "<p>Token count: " + std::to_string(tokens.size()) + "</p>\n";
        output += "</body>\n</html>\n";
        
        return true;
    } catch (const std::exception& e) {
        context->addError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

void CHTLCompiler::setConfiguration(const std::map<std::string, std::string>& config) {
    context->setConfiguration(config);
}

void CHTLCompiler::loadConfiguration(const std::string& configFile) {
    context->loadConfiguration(configFile);
}

std::vector<std::string> CHTLCompiler::getErrors() const {
    return context->getErrors();
}

std::vector<std::string> CHTLCompiler::getWarnings() const {
    return context->getWarnings();
}

bool CHTLCompiler::loadModule(const std::string& modulePath) {
    // TODO: Implement module loading
    context->addWarning("Module loading not yet implemented: " + modulePath);
    return false;
}

std::vector<std::string> CHTLCompiler::getAvailableModules() const {
    // TODO: Implement module listing
    return {};
}

} // namespace CHTL