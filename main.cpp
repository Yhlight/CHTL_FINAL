#include <iostream>
#include <vector>
#include <string>

// Include all our compiler components
#include "Util/FileSystem/FileSystemUtil.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLStore/TemplateStore.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"

int main() {
    const std::string filename = "test.chtl";
    std::string source;
    try {
        source = FileSystemUtil::readFile(filename);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // --- Setup ---
    TemplateStore templateStore;
    CHTLLoader loader(templateStore);

    // --- Parsing ---
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens, templateStore, loader);
    auto program = parser.parseProgram();

    // --- Error Checking ---
    const auto& errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "--- Parser found " << errors.size() << " errors: ---" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    // --- Generation ---
    Generator generator(templateStore);
    std::string html_output = generator.generate(program);
    std::cout << "--- Generated HTML Output: ---" << std::endl;
    std::cout << html_output << std::endl;

    return 0;
}
