#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLResolver/TemplateResolver.h"
#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/AttributeNode.h"
#include "../CHTL/CHTLNode/RootNode.h"
#include "../CHTL/CHTLNode/StyleNode.h"
#include "../CHTL/CHTLNode/PropertyNode.h"
#include "../CHTL/CHTLNode/RuleNode.h"
#include "../CHTL/CHTLNode/TemplateDefinitionNode.h"
#include "../CHTL/CHTLNode/TemplateUsageNode.h"
#include <iostream>
#include <string>

// Re-using the AST Printer from the parser test to visualize the tree
class AstPrinter : public AstVisitor {
public:
    std::string print(std::shared_ptr<RootNode> root) {
        if (root) root->accept(*this);
        return result;
    }

    void visit(RootNode& node) override { result += "(Document\n"; indent++; for (auto& child : node.children) child->accept(*this); indent--; result += ")\n"; }
    void visit(ElementNode& node) override { result += std::string(indent * 2, ' ') + "(Element<" + node.tagName + ">\n"; indent++; for (auto& attr : node.attributes) attr->accept(*this); for (auto& child : node.children) child->accept(*this); indent--; result += std::string(indent * 2, ' ') + ")\n"; }
    void visit(AttributeNode& node) override { result += std::string(indent * 2, ' ') + "(Attribute " + node.key + "=\"" + node.value + "\")\n"; }
    void visit(TextNode& node) override { result += std::string(indent * 2, ' ') + "(Text \"" + node.text + "\")\n"; }
    void visit(StyleNode& node) override { result += std::string(indent*2, ' ') + "(Style\n"; indent++; for(auto& child : node.children) child->accept(*this); indent--; result += std::string(indent*2, ' ') + ")\n"; }
    void visit(PropertyNode& node) override { result += std::string(indent*2, ' ') + "(Property " + node.key + " = ...)\n"; }
    void visit(RuleNode& node) override { result += std::string(indent*2, ' ') + "(Rule " + node.selector + ")\n"; }
    void visit(TemplateDefinitionNode& node) override { result += std::string(indent*2, ' ') + "(TemplateDef " + node.name + ")\n"; }
    void visit(TemplateUsageNode& node) override { result += std::string(indent*2, ' ') + "(TemplateUsage " + node.name + ")\n"; }

private:
    std::string result;
    int indent = 0;
};


int main() {
    std::string source = R"(
[Template] @Style DefaultText {
    color: black;
}

[Template] @Element Box {
    div { text: "Box content"; }
}

html {
    body {
        h1 {
            style {
                @Style DefaultText;
            }
        }
        div {
            @Element Box;
        }
    }
}
)";

    try {
        std::cout << "--- Testing Resolver ---" << std::endl;

        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::shared_ptr<RootNode> rawAst = parser.parse();

        std::cout << "\n--- Raw AST ---" << std::endl;
        AstPrinter rawPrinter;
        std::cout << rawPrinter.print(rawAst) << std::endl;

        TemplateResolver resolver;
        std::shared_ptr<RootNode> resolvedAst = resolver.resolve(rawAst);

        std::cout << "\n--- Resolved AST ---" << std::endl;
        AstPrinter resolvedPrinter;
        std::cout << resolvedPrinter.print(resolvedAst) << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
