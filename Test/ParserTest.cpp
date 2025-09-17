#include <gtest/gtest.h>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/Node.h"
#include <string>
#include <sstream>

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
        if (node.value) {
            if (auto* literal = dynamic_cast<LiteralNode*>(node.value.get())) {
                ss << "Literal<'" << literal->token.lexeme << "'>";
            } else if (dynamic_cast<BinaryOpNode*>(node.value.get())) {
                ss << "BinaryOp<...>";
            }
        } else {
            ss << "(null)";
        }
        ss << "]\n";
    }

    void visit(TextNode& node) override {
        printIndent();
        ss << "Text{";
        if (auto* literal = dynamic_cast<LiteralNode*>(node.content.get())) {
            ss << "Literal<'" << literal->token.lexeme << "'>";
        }
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

    void visit(TemplateDefinitionNode& node) override {
        printIndent();
        ss << "Definition<" << node.node_type.lexeme << " " << node.template_type.lexeme << " " << node.name.lexeme << ">\n";
        indent++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indent--;
    }

    void visit(TemplateUsageNode& node) override {
        printIndent();
        ss << "Usage<" << node.template_type.lexeme << " " << node.name.lexeme << ">";
        if (!node.body.empty()) {
            ss << " with body\n";
            indent++;
            for (auto& rule : node.body) {
                rule->accept(*this);
            }
            indent--;
        } else {
            ss << "\n";
        }
    }

    void visit(DeleteNode& node) override {
        printIndent();
        ss << "Delete<" << node.identifier.lexeme << ">\n";
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

TEST(ParserTest, CustomStyleDefinitionAndUsage) {
    std::string source = R"(
[Custom] @Style TextSet {
    color,
    font-size;
}

div {
    style {
        @Style TextSet {
            color: red;
            delete font-size;
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
  Definition<[Custom] @Style TextSet>
    Attr[color: (null)]
    Attr[font-size: (null)]
  Element<div>
    Style
      Usage<@Style TextSet> with body
        Attr[color: Literal<'red'>]
        Delete<font-size>
)";

    ASSERT_EQ(result, expected);
}
