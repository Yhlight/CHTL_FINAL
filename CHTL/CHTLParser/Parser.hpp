#pragma once

#include "../CHTLLexer/Lexer.hpp"
#include "../CHTLNode/Node.hpp"
#include "../CHTLContext/CHTLContext.hpp"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& l, CHTLContext& ctx);

    std::unique_ptr<Program> parseProgram();
    const std::vector<std::string>& getErrors() const;

private:
    void nextToken();
    bool expectPeek(TokenType t);
    std::unique_ptr<Node> parseNode();
    std::unique_ptr<ElementNode> parseElementNode();
    void parseAttributes(ElementNode* element);
    void parseStyleBlock(ElementNode* parent);
    std::unique_ptr<CommentNode> parseCommentNode();
    std::unique_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    std::vector<std::unique_ptr<Node>> parseTemplateUsage();
    void expandElementTemplate(std::vector<std::unique_ptr<Node>>& nodes, const std::string& templateName);
    std::unique_ptr<StylePropertyNode> parseStyleProperty();
    void expandStyleTemplate(ElementNode* parent, const std::string& templateName);
    std::unique_ptr<InheritanceNode> parseInheritanceStatement();

    Lexer& lexer;
    CHTLContext& context;
    std::vector<std::string> errors;

    Token currentToken;
    Token peekToken;
};

} // namespace CHTL
