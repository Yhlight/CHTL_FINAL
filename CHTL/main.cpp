#include <iostream>
#include "CompilerDispatcher/CompilerDispatcher.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <entry_filename>" << std::endl;
        return 1;
    }

    std::string entry_filepath = argv[1];

    CHTL::CompilerDispatcher dispatcher;
    std::string output = dispatcher.compile(entry_filepath);

    std::cout << "--- CHTL Compiler Output ---" << std::endl;
    std::cout << output << std::endl;
    std::cout << "----------------------------" << std::endl;

    return 0;
}
