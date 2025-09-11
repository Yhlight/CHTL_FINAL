#pragma once

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/Node.h"
#include <vector>
#include <stdexcept>

class CHTLJSParser {
public:
    CHTLJSParser(const std::vector<CHTLJSToken>& tokens);
    CHTLJSNodeList parse();

    // Error handling
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

private:
    CHTLJSNodePtr parseStatement();
    CHTLJSNodePtr parseVirDeclaration();
    CHTLJSNodePtr parseListenBlock();
    CHTLJSNodePtr parseAnimateBlock();
    CHTLJSNodePtr parseFileLoaderBlock();
    // ... other parsing methods for router, etc.

    // Helper methods
    const CHTLJSToken& peek() const;
    const CHTLJSToken& advance();
    bool isAtEnd() const;
    const CHTLJSToken& consume(CHTLJSTokenType type, const std::string& message);
    bool match(const std::vector<CHTLJSTokenType>& types);

    ParseError error(const CHTLJSToken& token, const std::string& message);

    const std::vector<CHTLJSToken>& tokens;
    size_t current = 0;
};
