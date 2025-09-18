#pragma once

#include "../CHTLLexer/Lexer.hpp"
#include "../CHTLNode/Node.hpp"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& l);

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

    Lexer& lexer;
    std::vector<std::string> errors;

    Token currentToken;
    Token peekToken;
};

} // namespace CHTL
