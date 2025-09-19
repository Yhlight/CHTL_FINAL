#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLNode/RootNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include <iostream>
#include <cassert>

// Dummy Visitor to check the AST structure
class ASTChecker : public CHTL::Visitor {
public:
    int element_nodes = 0;
    int text_nodes = 0;
    int comment_nodes = 0;
    int import_nodes = 0;
    int namespace_nodes = 0;

    void visit(CHTL::ElementNode& node) override {
        element_nodes++;
        for(auto& child : node.children) {
            child->accept(*this);
        }
    }
    void visit(CHTL::TextNode& node) override { text_nodes++; }
    void visit(CHTL::StyleNode& node) override {}
    void visit(CHTL::OriginNode& node) override {}
    void visit(CHTL::ScriptNode& node) override {}
    void visit(CHTL::CommentNode& node) override { comment_nodes++; }
    void visit(CHTL::ImportNode& node) override { import_nodes++; }
    void visit(CHTL::NamespaceNode& node) override {
        namespace_nodes++;
        for(auto& child : node.children) {
            child->accept(*this);
        }
    }
    void visit(CHTL::ConfigNode& node) override {}
    void visit(CHTL::RootNode& node) override {
        for(auto& child : node.children) {
            child->accept(*this);
        }
    }
};

void runParserTest(const std::string& testName, const std::string& source) {
    std::cout << "Running parser test: " << testName << "..." << std::endl;

    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source, tokens, "test.chtl");
    auto root = parser.parse();

    assert(root != nullptr);

    ASTChecker checker;
    root->accept(checker);

    // Basic assertions, can be expanded
    if (testName == "Simple Element") {
        assert(checker.element_nodes == 1);
        std::cout << "PASS: " << testName << std::endl;
    } else if (testName == "Nested Elements") {
        assert(checker.element_nodes == 2);
        std::cout << "PASS: " << testName << std::endl;
    } else if (testName == "Top-level Declarations") {
        assert(checker.import_nodes == 1);
        assert(checker.namespace_nodes == 1);
        assert(checker.element_nodes == 1);
        std::cout << "PASS: " << testName << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
}


int main() {
    runParserTest("Simple Element", "div {}");
    runParserTest("Nested Elements", "body { div {} }");
    runParserTest("Top-level Declarations", R"(
        [Import] @Chtl from "lib.chtl";

        [Namespace] my_space {
            // content here
        }

        html {}
    )");

    std::cout << "All parser tests completed." << std::endl;
    return 0;
}
