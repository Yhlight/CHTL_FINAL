#include "ChtlJsParser.h"
#include "../CHTLJSNode/CjsSelectorNode.h"
#include "../CHTLJSNode/CjsPassthroughNode.h"

ChtlJsParser::ChtlJsParser(const std::vector<ChtlJsToken>& tokens) : tokens(tokens) {}

ChtlJsToken ChtlJsParser::peek() const {
    return tokens[current];
}

ChtlJsToken ChtlJsParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool ChtlJsParser::isAtEnd() const {
    return peek().type == ChtlJsTokenType::END_OF_FILE;
}

std::vector<std::shared_ptr<CjsBaseNode>> ChtlJsParser::parse() {
    std::vector<std::shared_ptr<CjsBaseNode>> ast;

    while (!isAtEnd()) {
        ChtlJsToken token = peek();
        if (token.type == ChtlJsTokenType::L_DOUBLE_BRACE) {
            advance(); // consume {{
            std::string selector_content;
            while(peek().type != ChtlJsTokenType::R_DOUBLE_BRACE && !isAtEnd()) {
                selector_content += advance().value;
            }
            if (isAtEnd()) { /* handle error */ }
            advance(); // consume }}
            ast.push_back(std::make_shared<CjsSelectorNode>(selector_content));
        } else {
            // Everything else becomes a passthrough node.
            ast.push_back(std::make_shared<CjsPassthroughNode>(advance().value));
        }
    }
    return ast;
}
