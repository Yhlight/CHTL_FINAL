Parser::Selector Parser::parseSelector() {
    Selector selector;
    selector.tagName = currentToken.lexeme;
    consume(TokenType::IDENTIFIER, "Expect a tag name for the selector.");

    if (match(TokenType::LEFT_BRACKET)) {
        if (check(TokenType::NUMBER)) {
            try {
                selector.index = std::stoi(currentToken.lexeme);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Parse Error: Invalid number for index at line " << currentToken.line << std::endl;
                selector.index = -1; // Reset to invalid
            } catch (const std::out_of_range& e) {
                std::cerr << "Parse Error: Index number out of range at line " << currentToken.line << std::endl;
                selector.index = -1; // Reset to invalid
            }
            advance();
        } else {
            std::cerr << "Parse Error: Expect a number inside brackets for index." << std::endl;
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after index.");
    }
    return selector;
}
