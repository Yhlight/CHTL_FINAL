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
        if (auto* literal = dynamic_cast<LiteralNode*>(node.value.get())) {
            ss << "Literal<'" << literal->token.lexeme << "'>";
        } else if (dynamic_cast<BinaryOpNode*>(node.value.get())) {
            ss << "BinaryOp<...>";
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
        ss << "TemplateDefinition<" << node.template_type.lexeme << " " << node.name.lexeme << ">\n";
        indent++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indent--;
    }

    void visit(TemplateUsageNode& node) override {
        printIndent();
        ss << "TemplateUsage<" << node.template_type.lexeme << " " << node.name.lexeme << ">\n";
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


TEST(ParserTest, TemplateDefinitionAndUsage) {
    std::string source = R"(
[Template] @Style DefaultText {
    color: black;
    font-size: 16px;
}

div {
    style {
        @Style DefaultText;
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
  TemplateDefinition<@Style DefaultText>
    Attr[color: Literal<'black'>]
    Attr[font-size: Literal<'16px'>]
  Element<div>
    Style
      TemplateUsage<@Style DefaultText>
)";

    ASSERT_EQ(result, expected);
}
