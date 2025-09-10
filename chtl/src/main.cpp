#include <iostream>
#include <fstream>
#include <string>
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "ast/ast_node.h"
#include "style/style_aggregator.h"
#include "import/import_manager.h"
#include "import/import_resolver.h"
#include "analysis/constraint_validator.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    
    // Read file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // Lexical analysis
        lexer::CHTLLexer lexer(content);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens:" << std::endl;
        for (const auto& token : tokens.get_tokens()) {
            if (token.type != lexer::TokenType::WHITESPACE && 
                token.type != lexer::TokenType::NEWLINE) {
                std::cout << token.to_string() << std::endl;
            }
        }
        
        // Syntax analysis
        parser::CHTLParser parser(tokens);
        auto ast = parser.parse();

        // Resolve imports
        import::ImportManager import_manager("."); // Base path is current directory
        import::ImportResolver import_resolver(import_manager);
        import_resolver.resolve_imports(ast);
        
        std::cout << "\nAST:" << std::endl;
        std::cout << ast->to_string() << std::endl;

        // Process styles
        style::StyleAggregator style_aggregator;
        style_aggregator.aggregate_styles(ast);

        std::cout << "\nAST after style processing:" << std::endl;
        std::cout << ast->to_string() << std::endl;

        // Validate constraints
        analysis::ConstraintValidator validator;
        if (!validator.validate(ast)) {
            std::cerr << "Constraint validation failed:" << std::endl;
            for (const auto& error : validator.get_errors()) {
                std::cerr << " - " << error << std::endl;
            }
            return 1; // Exit with an error code
        }
        
        // Generate HTML
        std::cout << "\nGenerated HTML:" << std::endl;
        std::cout << ast->to_html() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}