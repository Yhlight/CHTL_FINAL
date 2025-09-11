#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLSemantic/SemanticAnalyzer.h"
#include "Context/ConfigurationContext.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL_JS/CHTLJSLexer/Lexer.h"
#include "CHTL_JS/CHTLJSParser/Parser.h"
#include "CHTL_JS/CHTLJSGenerator/Generator.h"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// This config population is still basic and will need to be improved later.
void populateConfigFromTokens(ConfigurationContext& config, const std::string& source) {
    Lexer lexer_for_config(source, config);
    auto tokens_for_config = lexer_for_config.tokenize();
    for (size_t i = 0; i < tokens_for_config.size(); ++i) {
        if (tokens_for_config[i].type == TokenType::ConfigurationKeyword) {
            if (i + 1 < tokens_for_config.size() && tokens_for_config[i+1].type == TokenType::OpenBrace) {
                i += 2;
                while (i < tokens_for_config.size() && tokens_for_config[i].type != TokenType::CloseBrace) {
                    if (tokens_for_config[i].type == TokenType::Identifier && i + 2 < tokens_for_config.size() && (tokens_for_config[i+1].type == TokenType::Equals)) {
                        const std::string& key = tokens_for_config[i].value;
                        const Token& valueToken = tokens_for_config[i+2];
                        if (valueToken.type >= TokenType::Identifier && valueToken.type <= TokenType::ConfigurationKeyword) {
                             if (key.rfind("KEYWORD_", 0) == 0) {
                                config.clearKeyword(key);
                                config.addKeyword(key, valueToken.value);
                            } else {
                                config.setString(key, valueToken.value);
                            }
                        }
                        i += 3;
                        if (i < tokens_for_config.size() && tokens_for_config[i].type == TokenType::Semicolon) i++;
                    } else { i++; }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string source = readFile(argv[1]);

    // 1. Configuration Pass (remains basic for now)
    ConfigurationContext config;
    populateConfigFromTokens(config, source);

    // 2. Scan with the new UnifiedScanner
    UnifiedScanner scanner(source);
    ScannedContent scanned_content = scanner.scan();

    // 3. CHTL Parsing
    // The lexer and parser now receive CHTL with placeholders
    Lexer lexer(scanned_content.chtl_content, config);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    NodeList ast = parser.parse();

    // 3.5. Semantic Analysis
    try {
        SemanticAnalyzer analyzer(ast);
        analyzer.analyze();
    } catch (const SemanticAnalyzer::SemanticError& e) {
        std::cerr << "Semantic Error: " << e.what() << std::endl;
        return 1;
    }

    // 4. Dispatch and Generate
    // The dispatcher will generate HTML containing placeholders
    CompilerDispatcher dispatcher(ast, scanned_content, config, argv[1]);
    CompilationResult compilation_result = dispatcher.dispatch();

    // 5. Merge and Decode Placeholders
    // The new merger takes the result and the scanned content to fill in the placeholders
    CodeMerger merger(compilation_result, scanned_content);
    std::string final_html = merger.merge();

    std::cout << final_html << std::endl;

    return 0;
}
