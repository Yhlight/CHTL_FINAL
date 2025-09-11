#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "Context/ConfigurationContext.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL_JS/CHTLJSLexer/Lexer.h"
#include "CHTL_JS/CHTLJSParser/Parser.h"
#include "CHTL_JS/CHTLJSGenerator/Generator.h"

void testCHTLJS() {
    std::cout << "\n--- Testing CHTL JS Generator ---\n";
    std::string source = R"~(
        {{my_button}}.listen {
            click: () => { console.log("Button clicked!"); },
            mouseenter: onMouseEnter
        };
    )~";

    CHTLJSLexer lexer(source);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    CHTLJSNodeList ast;
    try {
        CHTLJSParser parser(tokens);
        ast = parser.parse();
    } catch (const CHTLJSParser::ParseError& e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
        return;
    }

    CHTLJSGenerator generator;
    std::string js_output = generator.generate(ast);
    std::cout << js_output << std::endl;

    std::cout << "-------------------------------\n";
}


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
        testCHTLJS();
        return 1;
    }

    std::string source = readFile(argv[1]);

    // 1. Configuration Pass
    ConfigurationContext config;
    populateConfigFromTokens(config, source);

    // 2. Main Compilation
    UnifiedScanner scanner(source);
    ScannedContent scanned_content = scanner.scan();

    Lexer lexer(scanned_content.chtl_content, config);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    NodeList ast = parser.parse();

    // 3. Dispatch and Generate
    CompilerDispatcher dispatcher(ast, config);
    CompilationResult compilation_result = dispatcher.dispatch();

    // 4. Add global blocks
    for(const auto& pair : scanned_content.css_blocks) {
        compilation_result.compiled_css += pair.second;
    }
    for(const auto& pair : scanned_content.script_blocks) {
        compilation_result.compiled_js += pair.second;
    }

    // 5. Merge
    CodeMerger merger(compilation_result);
    std::string final_html = merger.merge();

    std::cout << final_html << std::endl;

    return 0;
}
