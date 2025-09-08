#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTLNode/DocumentNode.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string entry_file = argv[1];

    try {
        CHTL::CHTLContext context;
        CHTL::CHTLLoader loader; // Uses real filesystem

        std::unique_ptr<CHTL::DocumentNode> ast = loader.loadAndParse(entry_file, context);

        CHTL::Generator generator;
        std::string html_output = generator.generate(ast.get(), context);

        std::cout << html_output << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
