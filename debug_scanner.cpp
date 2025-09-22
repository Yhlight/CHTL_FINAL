#include "Scanner/CHTLUnifiedScanner.h"
#include "Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string file_path = "Test/ScannerRobustnessTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    
    std::cout << "Source content:" << std::endl;
    std::cout << source << std::endl;
    std::cout << "---" << std::endl;

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    std::cout << "Number of fragments: " << fragments.size() << std::endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        std::cout << "Fragment " << i << " (Type: ";
        switch (fragments[i].type) {
            case CHTL::FragmentType::CHTL:
                std::cout << "CHTL";
                break;
            case CHTL::FragmentType::CHTL_JS:
                std::cout << "CHTL_JS";
                break;
            case CHTL::FragmentType::CSS:
                std::cout << "CSS";
                break;
            case CHTL::FragmentType::JS:
                std::cout << "JS";
                break;
        }
        std::cout << "): " << std::endl;
        std::cout << "Content: '" << fragments[i].content << "'" << std::endl;
        std::cout << "---" << std::endl;
    }

    return 0;
}