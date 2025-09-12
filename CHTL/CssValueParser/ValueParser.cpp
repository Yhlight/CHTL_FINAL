#include "ValueParser.h"
#include <stdexcept>

ValueParser::ValueParser(const std::vector<ValueToken>& tokens) : tokens(tokens) {}

std::shared_ptr<ExprNode> ValueParser::parse() {
    return parseExpression();
}

int ValueParser::getPrecedence(ValueTokenType type) {
    switch (type) {
        case ValueTokenType::TOKEN_PLUS:
        case ValueTokenType::TOKEN_MINUS:
            return 1;
        case ValueTokenType::TOKEN_STAR:
        case ValueTokenType::TOKEN_SLASH:
            return 2;
        case ValueTokenType::TOKEN_POWER:
            return 3;
        default:
            return 0;
    }
}

std::shared_ptr<ExprNode> ValueParser::parsePrefix() {
    ValueToken token = advance();
    if (token.type == ValueTokenType::TOKEN_IDENTIFIER) {
        // This could be a property reference
        if (peek().type == ValueTokenType::TOKEN_DOT) {
            advance(); // consume '.'
            ValueToken property = advance();
            if (property.type == ValueTokenType::TOKEN_IDENTIFIER) {
                auto node = std::make_shared<PropertyReferenceNode>();
                node->selector = token.text;
                node->propertyName = property.text;
                return node;
            } else {
                throw std::runtime_error("Expected property name after '.'.");
            }
        }
        // If it's just an identifier, it might be a variable or keyword later.
        // For now, it's an error in an arithmetic expression.
        throw std::runtime_error("Unexpected identifier in expression.");
    }

    if (token.type == ValueTokenType::TOKEN_NUMBER) {
        auto node = std::make_shared<NumericLiteralNode>();

        // Find the split point between number and unit
        size_t unit_start = std::string::npos;
        for (size_t i = 0; i < token.text.length(); ++i) {
            if (isalpha(token.text[i]) || token.text[i] == '%') {
                unit_start = i;
                break;
            }
        }

        if (unit_start == std::string::npos) {
            node->value = std::stod(token.text);
            node->unit = "";
        } else {
            node->value = std::stod(token.text.substr(0, unit_start));
            node->unit = token.text.substr(unit_start);
        }
        return node;
    }

    if (token.type == ValueTokenType::TOKEN_LPAREN) {
        std::shared_ptr<ExprNode> expr = parseExpression();
        if (peek().type != ValueTokenType::TOKEN_RPAREN) {
            throw std::runtime_error("Expected ')' after expression.");
        }
        advance(); // Consume ')'
        return expr;
    }

    throw std::runtime_error("Expected a number or '(' for a prefix expression.");
}

std::shared_ptr<ExprNode> ValueParser::parseInfix(std::shared_ptr<ExprNode> left, ValueToken opToken) {
    auto node = std::make_shared<BinaryOpNode>();
    node->left = left;
    node->op = opToken;
    node->right = parseExpression(getPrecedence(opToken.type));
    return node;
}

std::shared_ptr<ExprNode> ValueParser::parseExpression(int precedence) {
    std::shared_ptr<ExprNode> left = parsePrefix();

    while (precedence < getPrecedence(peek().type)) {
        ValueToken opToken = advance();
        left = parseInfix(left, opToken);
    }

    return left;
}

ValueToken ValueParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

ValueToken ValueParser::peek() {
    return tokens[current];
}

bool ValueParser::isAtEnd() {
    return peek().type == ValueTokenType::TOKEN_EOF;
}
