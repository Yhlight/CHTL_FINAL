#include "CHTLJS.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace CHTLJS {

CHTLJS::CHTLJS() : initialized(false) {
    initialize();
}

CHTLJS::~CHTLJS() {
    cleanup();
}

void CHTLJS::initialize() {
    try {
        context = std::make_unique<CHTLJSContext>();
        lexer = std::make_unique<CHTLJSLexer>("");
        parser = std::make_unique<CHTLJSParser>(TokenList());
        generator = std::make_unique<CHTLJSGenerator>();
        
        initialized = true;
        lastError.clear();
    } catch (const std::exception& e) {
        lastError = "Initialization failed: " + std::string(e.what());
        initialized = false;
    }
}

void CHTLJS::cleanup() {
    context.reset();
    lexer.reset();
    parser.reset();
    generator.reset();
    initialized = false;
}

std::string CHTLJS::compile(const std::string& source) {
    if (!initialized) {
        lastError = "CHTL JS not initialized";
        return "";
    }
    
    try {
        // 词法分析
        lexer->setSource(source);
        auto tokens = lexer->tokenize();
        
        if (context->isDebugMode()) {
            std::cout << "=== Tokens ===" << std::endl;
            tokens.print();
            std::cout << "=============" << std::endl;
        }
        
        // 语法分析
        parser->setTokens(tokens);
        auto ast = parser->parse();
        
        if (context->isDebugMode()) {
            std::cout << "=== AST ===" << std::endl;
            std::cout << ast->toString() << std::endl;
            std::cout << "===========" << std::endl;
        }
        
        // 代码生成
        generator->setMinify(context->isMinifyOutput());
        std::string result = generator->generate(ast);
        
        lastError.clear();
        return result;
        
    } catch (const std::exception& e) {
        lastError = "Compilation failed: " + std::string(e.what());
        return "";
    }
}

std::string CHTLJS::compileFile(const std::string& filePath) {
    if (!initialized) {
        lastError = "CHTL JS not initialized";
        return "";
    }
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            lastError = "Failed to open file: " + filePath;
            return "";
        }
        
        std::string source((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        file.close();
        
        return compile(source);
        
    } catch (const std::exception& e) {
        lastError = "File compilation failed: " + std::string(e.what());
        return "";
    }
}

void CHTLJS::setDebugMode(bool debug) {
    if (context) {
        context->setDebugMode(debug);
    }
}

void CHTLJS::setMinifyOutput(bool minify) {
    if (context) {
        context->setMinifyOutput(minify);
    }
}

void CHTLJS::setOutputPath(const std::string& path) {
    if (context) {
        context->setOutputPath(path);
    }
}

bool CHTLJS::isInitialized() const {
    return initialized;
}

std::string CHTLJS::getLastError() const {
    return lastError;
}

bool CHTLJS::hasError() const {
    return !lastError.empty();
}

CHTLJSContext* CHTLJS::getContext() const {
    return context.get();
}

void CHTLJS::reset() {
    cleanup();
    initialize();
}

void CHTLJS::clear() {
    if (context) {
        context->clear();
    }
    lastError.clear();
}

void CHTLJS::printContext() const {
    if (context) {
        context->printContext();
    }
}

void CHTLJS::printTokens(const std::string& source) const {
    if (!lexer) return;
    
    try {
        lexer->setSource(source);
        auto tokens = lexer->tokenize();
        tokens.print();
    } catch (const std::exception& e) {
        std::cerr << "Error printing tokens: " << e.what() << std::endl;
    }
}

void CHTLJS::printAST(const std::string& source) const {
    if (!lexer || !parser) return;
    
    try {
        lexer->setSource(source);
        auto tokens = lexer->tokenize();
        parser->setTokens(tokens);
        auto ast = parser->parse();
        
        std::cout << "=== AST ===" << std::endl;
        std::cout << ast->toString() << std::endl;
        std::cout << "===========" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error printing AST: " << e.what() << std::endl;
    }
}

} // namespace CHTLJS