#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include <iostream>
#include <vector>
#include <cassert>

void testBasicStructure() {
    std::string name = "Basic Structure";
    std::string input = "html { body {} }";
    Lexer lexer(input);
    Parser parser(lexer);
    NodeList ast = parser.parse();

    assert(ast.size() == 1);
    assert(ast[0]->getType() == NodeType::Element);
    ElementNode* root = static_cast<ElementNode*>(ast[0].get());
    assert(root->tagName == "html");
    assert(root->children.size() == 1);
    assert(root->children[0]->getType() == NodeType::Element);
    ElementNode* body = static_cast<ElementNode*>(root->children[0].get());
    assert(body->tagName == "body");
    assert(body->children.empty());
    std::cout << "[PASS] " << name << std::endl;
}

void testAttributes() {
    std::string name = "Attributes";
    std::string input = "div { id: main; class=\"container\"; }";
    Lexer lexer(input);
    Parser parser(lexer);
    NodeList ast = parser.parse();

    assert(ast.size() == 1);
    ElementNode* div = static_cast<ElementNode*>(ast[0].get());
    assert(div->tagName == "div");
    assert(div->attributes.size() == 2);
    assert(div->attributes[0].key == "id");
    assert(div->attributes[0].value == "main");
    assert(div->attributes[1].key == "class");
    assert(div->attributes[1].value == "container");
    std::cout << "[PASS] " << name << std::endl;
}

void testTextContent() {
    std::string name = "Text Content";
    std::string input = "p { text { Hello CHTL } }";
    Lexer lexer(input);
    Parser parser(lexer);
    NodeList ast = parser.parse();

    assert(ast.size() == 1);
    ElementNode* p = static_cast<ElementNode*>(ast[0].get());
    assert(p->tagName == "p");
    assert(p->children.size() == 1);
    assert(p->children[0]->getType() == NodeType::Text);
    TextNode* text = static_cast<TextNode*>(p->children[0].get());
    assert(text->content == "Hello CHTL");
    std::cout << "[PASS] " << name << std::endl;
}


void testErrorHandling() {
    std::string name = "Error Handling";
    // Missing semicolon after attribute
    std::string input = "div { id: main }";
    Lexer lexer(input);
    Parser parser(lexer);
    NodeList ast = parser.parse();

    assert(!parser.getErrors().empty());
    assert(parser.getErrors().size() == 1);
    // A bit fragile, but we can check for a substring. The int value for SEMICOLON is 12.
    std::string expected_error = "Expected next token to be " + std::to_string(static_cast<int>(TokenType::SEMICOLON));
    assert(parser.getErrors()[0].find(expected_error) != std::string::npos);
    std::cout << "[PASS] " << name << std::endl;
}

void runParserTests() {
    std::cout << "--- Running Parser Tests ---" << std::endl;
    testBasicStructure();
    testAttributes();
    testTextContent();
    testErrorHandling();
    std::cout << "Parser Tests Passed: 4/4" << std::endl;
}
