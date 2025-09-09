// CHTL 核心库实现
#include "CHTLCore.hpp"
#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLParser/Parser.hpp>
#include <CHTL/CHTLGenerator/CodeGenerator.hpp>
#include <CHTL/CHTLModule/ModuleManager.hpp>
#include <CHTL/CHTLScanner/UnifiedScanner.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {

CHTLCore::CHTLCore(bool defaultStruct) : useDefaultStruct_(defaultStruct) {
    // 初始化核心组件
    moduleManager_ = &ModuleManager::getInstance();
    scanner_ = std::make_unique<UnifiedScanner>();
}

CHTLCore::~CHTLCore() = default;

bool CHTLCore::compileFile(const std::string& inputPath, const std::string& outputPath) {
    try {
        // 读取输入文件
        std::ifstream inputFile(inputPath);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Cannot open input file " << inputPath << std::endl;
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(inputFile)),
                           std::istreambuf_iterator<char>());
        inputFile.close();
        
        // 使用统一扫描器分离代码
        auto codeBlocks = scanner_->scan(content);
        
        // 处理每个代码块
        std::ostringstream htmlOutput;
        std::ostringstream cssOutput;
        std::ostringstream jsOutput;
        
        for (const auto& block : codeBlocks) {
            if (block.type == CodeType::CHTL) {
                // 处理 CHTL 代码
                Lexer lexer(block.content);
                Parser parser(lexer);
                auto ast = parser.parse();
                
                if (parser.hasError()) {
                    std::cerr << "Parse error: " << parser.getError() << std::endl;
                    return false;
                }
                
                CodeGenerator generator(useDefaultStruct_);
                htmlOutput << generator.generateHTML(ast);
                cssOutput << generator.generateCSS(ast);
                jsOutput << generator.generateJavaScript(ast);
            } else if (block.type == CodeType::CHTL_JS) {
                // 处理 CHTL JS 代码
                // 这里可以添加 CHTL JS 的处理逻辑
                jsOutput << "// CHTL JS 代码\n" << block.content << "\n";
            } else if (block.type == CodeType::CSS) {
                // 处理 CSS 代码
                cssOutput << "/* CSS 代码 */\n" << block.content << "\n";
            } else if (block.type == CodeType::JAVASCRIPT) {
                // 处理 JavaScript 代码
                jsOutput << "// JavaScript 代码\n" << block.content << "\n";
            }
        }
        
        // 生成最终输出
        std::ostringstream finalOutput;
        finalOutput << htmlOutput.str();
        
        if (!cssOutput.str().empty()) {
            finalOutput << "\n<style>\n" << cssOutput.str() << "</style>\n";
        }
        
        if (!jsOutput.str().empty()) {
            finalOutput << "\n<script>\n" << jsOutput.str() << "</script>\n";
        }
        
        // 写入输出文件
        std::ofstream outputFile(outputPath);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Cannot create output file " << outputPath << std::endl;
            return false;
        }
        
        outputFile << finalOutput.str();
        outputFile.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool CHTLCore::compileString(const std::string& input, std::string& output) {
    try {
        // 使用统一扫描器分离代码
        auto codeBlocks = scanner_->scan(input);
        
        // 处理每个代码块
        std::ostringstream htmlOutput;
        std::ostringstream cssOutput;
        std::ostringstream jsOutput;
        
        for (const auto& block : codeBlocks) {
            if (block.type == CodeType::CHTL) {
                // 处理 CHTL 代码
                Lexer lexer(block.content);
                Parser parser(lexer);
                auto ast = parser.parse();
                
                if (parser.hasError()) {
                    output = "Parse error: " + parser.getError();
                    return false;
                }
                
                CodeGenerator generator(useDefaultStruct_);
                htmlOutput << generator.generateHTML(ast);
                cssOutput << generator.generateCSS(ast);
                jsOutput << generator.generateJavaScript(ast);
            } else if (block.type == CodeType::CHTL_JS) {
                // 处理 CHTL JS 代码
                jsOutput << "// CHTL JS 代码\n" << block.content << "\n";
            } else if (block.type == CodeType::CSS) {
                // 处理 CSS 代码
                cssOutput << "/* CSS 代码 */\n" << block.content << "\n";
            } else if (block.type == CodeType::JAVASCRIPT) {
                // 处理 JavaScript 代码
                jsOutput << "// JavaScript 代码\n" << block.content << "\n";
            }
        }
        
        // 生成最终输出
        output = htmlOutput.str();
        
        if (!cssOutput.str().empty()) {
            output += "\n<style>\n" + cssOutput.str() + "</style>\n";
        }
        
        if (!jsOutput.str().empty()) {
            output += "\n<script>\n" + jsOutput.str() + "</script>\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        output = "Error: " + std::string(e.what());
        return false;
    }
}

bool CHTLCore::registerModule(const std::string& name, ModuleType type, const std::string& path) {
    return moduleManager_->registerModule(name, type, path);
}

bool CHTLCore::loadModule(const std::string& name) {
    return moduleManager_->loadModule(name);
}

std::vector<CodeBlock> CHTLCore::scanCode(const std::string& content) {
    return scanner_->scan(content);
}

std::string CHTLCore::getVersion() const {
    return "CHTL Compiler v1.0.0";
}

} // namespace CHTL