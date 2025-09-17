#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/StatementNodes.h"
#include "../CHTLNode/ExpressionNodes.h"
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <map>

class Generator {
public:
    explicit Generator(std::shared_ptr<Program> program);

    // The main entry point to start the generation process.
    void generate();

    // Get the final generated code.
    std::string getHtml() const;
    std::string getCss() const;

private:
    // Dispatcher method
    void generateNode(const std::shared_ptr<Node>& node);

    // Visitor methods for each node type
    void generateProgram(const std::shared_ptr<Program>& node);
    void generateElement(const std::shared_ptr<ElementNode>& node);
    void generateText(const std::shared_ptr<TextNode>& node);
    void generateStyle(const std::shared_ptr<StyleNode>& styleNode, std::map<std::string, std::string>& attributes);
    void generateComment(const std::shared_ptr<CommentNode>& node);

    // Helper for expression values
    std::string generateExpression(const std::shared_ptr<Expression>& node);

    // Helper for indentation
    void writeIndent();

    std::shared_ptr<Program> program;
    std::stringstream htmlStream;
    std::stringstream cssStream;
    int indentLevel;
};
