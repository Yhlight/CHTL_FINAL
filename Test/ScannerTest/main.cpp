#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <cassert>
#include <vector>

void print_fragments(const std::vector<CHTL::CodeFragment>& fragments) {
    for (const auto& frag : fragments) {
        std::cout << "--- Fragment ---" << std::endl;
        std::cout << "Type: ";
        switch(frag.type) {
            case CHTL::FragmentType::CHTL: std::cout << "CHTL"; break;
            case CHTL::FragmentType::CSS: std::cout << "CSS"; break;
            case CHTL::FragmentType::JS: std::cout << "JS"; break;
            case CHTL::FragmentType::CHTL_JS: std::cout << "CHTL_JS"; break;
        }
        std::cout << "\nContent:\n---\n" << frag.content << "\n---" << std::endl;
    }
}

int main() {
    std::cout << "--- Running Scanner Test ---" << std::endl;

    std::string source = R"(
        // This is some CHTL code
        div {
            id: "main-div";
        }

        style {
            body {
                background-color: #eee;
            }
        }

        text { "Some more CHTL" }

        script {
            console.log("Hello from a script block!");
            // another comment
        }

        div { class: "footer" }
    )";

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    print_fragments(fragments);

    assert(fragments.size() == 5);
    std::cout << "Assertion Passed: Correct number of fragments (5)." << std::endl;

    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].type == CHTL::FragmentType::CSS);
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    assert(fragments[3].type == CHTL::FragmentType::JS);
    assert(fragments[4].type == CHTL::FragmentType::CHTL);
    std::cout << "Assertion Passed: All fragment types are correct." << std::endl;

    // A simple check on content
    assert(fragments[1].content.find("background-color") != std::string::npos);
    assert(fragments[3].content.find("Hello from a script block!") != std::string::npos);
    std::cout << "Assertion Passed: Fragment contents are correct." << std::endl;


    std::cout << "\n--- Scanner Test Passed! ---" << std::endl;

    return 0;
}
