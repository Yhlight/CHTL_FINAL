#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <cassert>
#include <algorithm>

void test_simple_chtl() {
    std::cout << "Running test_simple_chtl..." << std::endl;
    std::string source = "div { text { \"Hello\" } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content == source);
    std::cout << "test_simple_chtl PASSED" << std::endl;
}

void test_single_html_fragment() {
    std::cout << "Running test_single_html_fragment..." << std::endl;
    std::string source = "div { text { \"Before\" } } [Origin] @Html { <p>Hello</p> } div { text { \"After\" } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    fragments.erase(std::remove_if(fragments.begin(), fragments.end(), [](const CHTL::CodeFragment& f) {
        return f.content.find_first_not_of(" \t\n\r") == std::string::npos;
    }), fragments.end());

    assert(fragments.size() == 3);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content == "div { text { \"Before\" } } ");
    assert(fragments[1].type == CHTL::FragmentType::HTML);
    assert(fragments[1].content == "<p>Hello</p>");
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    assert(fragments[2].content == " div { text { \"After\" } }");
    std::cout << "test_single_html_fragment PASSED" << std::endl;
}

void test_multiple_html_fragments() {
    std::cout << "Running test_multiple_html_fragments..." << std::endl;
    std::string source = "[Origin] @Html {<p>1</p>} CHTL1 [Origin] @Html {<p>2</p>} CHTL2";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    fragments.erase(std::remove_if(fragments.begin(), fragments.end(), [](const CHTL::CodeFragment& f) {
        return f.content.find_first_not_of(" \t\n\r") == std::string::npos;
    }), fragments.end());

    assert(fragments.size() == 4);
    assert(fragments[0].type == CHTL::FragmentType::HTML);
    assert(fragments[0].content == "<p>1</p>");
    assert(fragments[1].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].content == " CHTL1 ");
    assert(fragments[2].type == CHTL::FragmentType::HTML);
    assert(fragments[2].content == "<p>2</p>");
    assert(fragments[3].type == CHTL::FragmentType::CHTL);
    assert(fragments[3].content == " CHTL2");
    std::cout << "test_multiple_html_fragments PASSED" << std::endl;
}

void test_mixed_origin_blocks() {
    std::cout << "Running test_mixed_origin_blocks..." << std::endl;
    std::string source = "[Origin] @Html {<p>html</p>} [Origin] @Style { body { color: red; } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    fragments.erase(std::remove_if(fragments.begin(), fragments.end(), [](const CHTL::CodeFragment& f) {
        return f.content.find_first_not_of(" \t\n\r") == std::string::npos;
    }), fragments.end());

    assert(fragments.size() == 2);
    assert(fragments[0].type == CHTL::FragmentType::HTML);
    assert(fragments[0].content == "<p>html</p>");
    assert(fragments[1].type == CHTL::FragmentType::CHTL); // Treated as CHTL for now
    assert(fragments[1].content == "[Origin] @Style { body { color: red; } }");
    std::cout << "test_mixed_origin_blocks PASSED" << std::endl;
}

void test_origin_in_comment() {
    std::cout << "Running test_origin_in_comment..." << std::endl;
    std::string source = "div { /* [Origin] @Html { <p>not real</p> } */ }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content == source);
    std::cout << "test_origin_in_comment PASSED" << std::endl;
}

void test_nested_braces_in_html() {
    std::cout << "Running test_nested_braces_in_html..." << std::endl;
    std::string source = "[Origin] @Html { <script> if (true) { console.log(\"hello\"); } </script> }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::HTML);
    assert(fragments[0].content == "<script> if (true) { console.log(\"hello\"); } </script>");
    std::cout << "test_nested_braces_in_html PASSED" << std::endl;
}

void test_no_chtl_before() {
    std::cout << "Running test_no_chtl_before..." << std::endl;
    std::string source = "[Origin] @Html { <p>Hello</p> } div { text { \"After\" } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    fragments.erase(std::remove_if(fragments.begin(), fragments.end(), [](const CHTL::CodeFragment& f) {
        return f.content.find_first_not_of(" \t\n\r") == std::string::npos;
    }), fragments.end());

    assert(fragments.size() == 2);
    assert(fragments[0].type == CHTL::FragmentType::HTML);
    assert(fragments[0].content == "<p>Hello</p>");
    assert(fragments[1].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].content == " div { text { \"After\" } }");
    std::cout << "test_no_chtl_before PASSED" << std::endl;
}

void test_no_chtl_after() {
    std::cout << "Running test_no_chtl_after..." << std::endl;
    std::string source = "div { text { \"Before\" } } [Origin] @Html { <p>Hello</p> }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    fragments.erase(std::remove_if(fragments.begin(), fragments.end(), [](const CHTL::CodeFragment& f) {
        return f.content.find_first_not_of(" \t\n\r") == std::string::npos;
    }), fragments.end());

    assert(fragments.size() == 2);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content == "div { text { \"Before\" } } ");
    assert(fragments[1].type == CHTL::FragmentType::HTML);
    assert(fragments[1].content == "<p>Hello</p>");
    std::cout << "test_no_chtl_after PASSED" << std::endl;
}

void test_empty_file() {
    std::cout << "Running test_empty_file..." << std::endl;
    std::string source = "";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.empty());
    std::cout << "test_empty_file PASSED" << std::endl;
}

void test_only_origin_block() {
    std::cout << "Running test_only_origin_block..." << std::endl;
    std::string source = "[Origin] @Html { <p>Hello</p> }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::HTML);
    assert(fragments[0].content == "<p>Hello</p>");
    std::cout << "test_only_origin_block PASSED" << std::endl;
}


int main() {
    test_simple_chtl();
    test_single_html_fragment();
    test_multiple_html_fragments();
    test_mixed_origin_blocks();
    test_origin_in_comment();
    test_nested_braces_in_html();
    test_no_chtl_before();
    test_no_chtl_after();
    test_empty_file();
    test_only_origin_block();

    std::cout << "All scanner tests passed!" << std::endl;

    return 0;
}
