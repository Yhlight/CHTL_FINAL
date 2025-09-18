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
    std::unique_ptr<Node> parseNode();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode(); // Special handling for 'text' element content

    Lexer& lexer;
    std::vector<std::string> errors;

    Token currentToken;
    Token peekToken;
};

} // namespace CHTL
