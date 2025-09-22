#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm> // For std::remove

// Helper function to trim leading/trailing whitespace for comparison
std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// Helper function to remove all whitespace for comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}


int main() {
    std::string file_path = "Test/StyleScannerTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    if (source.empty()) {
        std::cerr << "Failed to read test file: " << file_path << std::endl;
        return 1;
    }

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    std::cout << "--- SCANNER OUTPUT ---" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        std::cout << "Fragment " << i << " | Type: " << static_cast<int>(fragments[i].type)
                  << " | Placeholder: " << fragments[i].placeholder_id << "\n---CONTENT---\n"
                  << fragments[i].content << "\n---END CONTENT---\n" << std::endl;
    }
    std::cout << "----------------------" << std::endl;

    // --- Expected Results ---
    // Fragment 0: CHTL (before style block)
    // Fragment 1: CHTL (first CHTL decl in CSS)
    // Fragment 2: CHTL (second CHTL decl in CSS)
    // Fragment 3: CSS (the style block with placeholders)
    // Fragment 4: CHTL (after style block)

    assert(fragments.size() == 5 && "Test Failed: Expected exactly 5 fragments.");

    // --- Assertions for each fragment ---

    // Fragment 0: CHTL
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    std::string expected_frag0 = "div { text: \"This is outside the style block\"; }";
    assert(remove_whitespace(fragments[0].content) == remove_whitespace(expected_frag0));

    // Fragment 1: CHTL (width property)
    assert(fragments[1].type == CHTL::FragmentType::CHTL);
    std::string expected_frag1 = "width: 100% + 20px;";
    assert(trim(fragments[1].content) == expected_frag1);
    assert(fragments[1].placeholder_id.find("_CHTL_CSS_PLACEHOLDER_0_") != std::string::npos);

    // Fragment 2: CHTL (@Style)
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    std::string expected_frag2 = "@Style BoxStyle;";
    assert(trim(fragments[2].content) == expected_frag2);
    assert(fragments[2].placeholder_id.find("_CHTL_CSS_PLACEHOLDER_1_") != std::string::npos);

    // Fragment 3: CSS
    assert(fragments[3].type == CHTL::FragmentType::CSS);
    std::string expected_frag3 = R"css(
    body {
        font-family: "Arial", sans-serif;
        margin: 0;
    }

    .container {
        padding: 10px;
        border: 1px solid #ccc;
        _CHTL_CSS_PLACEHOLDER_0_; /* CHTL in parent rule */
        background-color: #f0f0f0;
    }

    @media (min-width: 600px) {
        .container {
            _CHTL_CSS_PLACEHOLDER_1_; /* CHTL in nested rule */
            border-color: #999;
        }
    }
    )css";

    std::string actual_no_space = remove_whitespace(fragments[3].content);
    std::string expected_no_space = remove_whitespace(expected_frag3);

    if (actual_no_space != expected_no_space) {
        std::cerr << "CSS FRAGMENT MISMATCH!" << std::endl;
        std::cerr << "--- EXPECTED (no whitespace) ---\n" << expected_no_space << "\n----------------\n" << std::endl;
        std::cerr << "--- ACTUAL (no whitespace) ---\n" << actual_no_space << "\n--------------\n" << std::endl;
    }
    assert(actual_no_space == expected_no_space && "CSS Fragment content mismatch");


    // Fragment 4: CHTL
    assert(fragments[4].type == CHTL::FragmentType::CHTL);
    std::string expected_frag4 = "p { text: \"This is after the style block\"; }";
    assert(remove_whitespace(fragments[4].content) == remove_whitespace(expected_frag4));

    std::cout << "StyleScannerTest PASSED!" << std::endl;

    return 0;
}
