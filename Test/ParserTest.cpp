#include <gtest/gtest.h>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/Node.h"
#include <string>
#include <sstream>

// An AST visitor that creates a string representation of the tree.
class ASTStringifier : public INodeVisitor {
public:
    std::string stringify(Node& node) {
        node.accept(*this);
        return ss.str();
    }

    void visit(ProgramNode& node) override {
        ss << "Program\n";
        indent++;
        for (auto& child : node.children) {
            if(child) child->accept(*this);
        }
        indent--;
    }

    void visit(ElementNode& node) override {
        printIndent();
        ss << "Element<" << node.tagName << ">\n";
        indent++;
        for (auto& child : node.children) {
            if(child) child->accept(*this);
        }
        indent--;
    }

    void visit(AttributeNode& node) override {
        printIndent();
        ss << "Attr[" << node.key << ": ";
        node.value->accept(*this);
        ss << "]\n";
    }

    void visit(TextNode& node) override {
        printIndent();
        ss << "Text{";
        node.content->accept(*this);
        ss << "}\n";
    }

    void visit(StyleNode& node) override {
        printIndent();
        ss << "Style\n";
        indent++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indent--;
    }

    void visit(SelectorNode& node) override {
        printIndent();
        ss << "Selector<" << node.selector << ">\n";
        indent++;
        for (auto& prop : node.properties) {
            prop->accept(*this);
        }
        indent--;
    }

    void visit(ValueNode& node) override {
        ss << "'" << node.token.lexeme << "'";
    }

private:
    std::stringstream ss;
    int indent = 0;
    void printIndent() {
        for (int i = 0; i < indent; ++i) {
            ss << "  ";
        }
    }
};

TEST(ParserTest, BasicStructure) {
    std::string source = R"(
body {
    div {
        id: "main";
    }
}
)";
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    ASTStringifier stringifier;
    std::string result = stringifier.stringify(*ast);

    std::string expected =
R"(Program
  Element<body>
    Element<div>
      Attr[id: '"main"']
)";

    ASSERT_EQ(result, expected);
}

TEST(ParserTest, StyleBlockWithSelector) {
    std::string source = R"(
div {
    style {
        color: blue;
        .box {
            width: 100px;
        }
    }
}
)";
    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    CHTLParser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    ASTStringifier stringifier;
    std::string result = stringifier.stringify(*ast);

    std::string expected =
R"(Program
  Element<div>
    Style
      Attr[color: 'blue']
      Selector<.box>
        Attr[width: '100px']
)";

    ASSERT_EQ(result, expected);
}
